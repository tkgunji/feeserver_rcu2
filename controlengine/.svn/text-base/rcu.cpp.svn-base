#include <cerrno>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <fstream>
#include <typeinfo>
#include "rcu.hpp"
#include "rcu_issue.h"
#include "dcb.hpp"
#include "ser.hpp"

/******************************************************************************/

CtrRcu::CtrRcu():fpRcu(NULL),fpActel(NULL){
  AddTransition(new CETransition(kActionGoStandby, kStateStandby, std::string(kActionGoStandby_name), NULL, NULL, 5, kStateError, kStateFailure, kStateRunning, kStateDownloading, kStateIdle));
  AddTransition(new CETransition(kActionGoIdle, kStateIdle, std::string(kActionGoIdle_name), CtrRcuGO_IDLE, this, 5, kStateError, kStateFailure, kStateRunning, kStateDownloading, kStateStandby));
  AddTransition(new CETransition(kActionConf, kStateDownloading, std::string(kActionConf_name), NULL, NULL, 3, kStateStandby, kStateIdle, kStateRunning));
  AddTransition(new CETransition(kActionConfigureDone, kStateRunning, std::string(kActionConfigureDone_name), NULL, NULL, 1, kStateDownloading));
  AddTransition(new CETransition(kActionStart, kStateConfigured, std::string(kActionStart_name), NULL, NULL, 1, kStateRunning));
  AddTransition(new CETransition(kActionStop, kStateRunning, std::string(kActionStop_name), NULL, NULL, 1, kStateConfigured));
  AddTransition(new CETransition(kActionStartDaqConf, kStateConfDdl, std::string(kActionStartDaqConf_name), NULL, NULL, 4, kStateDownloading, kStateStandby, kStateIdle, kStateRunning));
  AddTransition(new CETransition(kActionEndDaqConf, kStateRunning, std::string(kActionEndDaqConf_name), NULL, NULL, 1, kStateConfDdl));

  if     (!fpMsgbuffer && fHwType==1) fpMsgbuffer=new DevMsgbufferDcb();
  else if(!fpMsgbuffer && fHwType==2) fpMsgbuffer=new DevMsgbufferDummy();
  if(fpMsgbuffer) CE_Event("DevMsgbuffer of type '%s' created at %p with HwType %i\n", typeid(*fpMsgbuffer).name(), fpMsgbuffer, fHwType);
  else CE_Warning("DevMsgbuffer not created at %p with HwType %i\n", fpMsgbuffer, fHwType);

  if(!fpRcu && fpMsgbuffer) fpRcu=new DevRcu(subDevices.size(), this, fpMsgbuffer);
  if(fpRcu){
    AddSubDevice(fpRcu);
    CE_Event("DevRcu '%s' (%i) of type '%s' created at %p with DevMsgbuffer at %p\n", fpRcu->GetName().c_str(), fpRcu->GetDeviceId(), typeid(*fpRcu).name(), fpRcu, fpMsgbuffer);
  }
  else CE_Warning("DevRcu not created at %p with DevMsgbuffer %p\n", fpRcu, fpMsgbuffer);

  char* envVar=getenv("FEESERVER_ACTEL");
  if(envVar && strncmp(envVar,"SET",strlen("SET"))==0 && !fpActel && fpMsgbuffer){
    fpActel=new DevActel(subDevices.size(), this, fpMsgbuffer);
    AddSubDevice(fpActel);
    CE_Event("DevActel '%s' (%i) of type '%s' created at %p with DevMsgbuffer at %p\n", fpActel->GetName().c_str(), fpActel->GetDeviceId(), typeid(*fpActel).name(), fpActel, fpMsgbuffer);
  }
  else CE_Warning("DevActel not created at %p with DevMsgbuffer %p\n", fpActel, fpMsgbuffer);

  if     (!fpFecaccess && fpMsgbuffer && fHwType==1) fpFecaccess=new DevFecaccessRcuI2c(fpMsgbuffer);
  else if(!fpFecaccess && fpMsgbuffer && fHwType==2) fpFecaccess=new DevFecaccessDummy (fpMsgbuffer);
  if(fpFecaccess) CE_Event("DevFecaccess of type '%s' created at %p with DevMsgbuffer at %p and HwType %i\n", typeid(*fpFecaccess).name(), fpFecaccess, fpMsgbuffer, fHwType);
  else CE_Warning("DevFecaccess not created at %p with DevMsgbuffer at %p and fHwType=%i\n", fpFecaccess, fpMsgbuffer, fHwType);
}

CtrRcu::~CtrRcu(){
  fpMsgbuffer=NULL; // deleted in sub-device handling
  fpRcu=NULL;
  fpActel=NULL;
  fpFecaccess=NULL;
  for(uint32_t i=0;i<32;i++){
    fpFecs[i]=NULL;
  }
}

int CtrRcu::CtrRcuGO_IDLE(CEStateMachine* callbackObject){
  CE_Debug("CtrRcuGO_IDLE called\n");
  if(!callbackObject){
    CE_Error("No state machine\n");
    return -1;
  }
  (dynamic_cast<CtrRcu*>(callbackObject))->fpRcu->SetAfl(0);
  return 0;
}

/******************************************************************************/

std::string DevRcu::imemFileName;
std::string DevRcu::regCfgFileName;
DevRcu::DevRcu(int id,  CEStateMachine* pParent, DevMsgbuffer* msgbuffer): Dev("RCU", id, pParent),fpMsgbuffer(msgbuffer){
  AddTransition(new CETransition(kActionGoStandby, kStateStandby, std::string(kActionGoStandby_name), NULL, NULL, 2, kStateError, kStateFailure));
  imemFileName=Ctr::tmpDir+"/FEESERVER_IMEM";
  regCfgFileName=Ctr::tmpDir+"/FEESERVER_REG_CFG";
}
int DevRcu::ReSynchronizeLocal(){
  if(!fpMsgbuffer) return kStateError;
  return kStateStandby;
}
int DevRcu::GetGroupId(){
  return FEESVR_CMD_RCU;
}
int DevRcu::HighLevelHandler(const char* pCommand, std::vector<uint32_t>& rb){
  uint32_t cmd=0;
  int keySize=0;
  if(!pCommand) return 0;
  if (strncmp(pCommand, "RCU_WRITE_AFL", keySize=strlen("RCU_WRITE_AFL"))==0) cmd=RCU_WRITE_AFL;
  else if (strncmp(pCommand, "RCU_READ_AFL", keySize=strlen("RCU_READ_AFL"))==0) cmd=RCU_READ_AFL;
  else if (strncmp(pCommand, "RCU_WRITE_MEMORY", keySize=strlen("RCU_WRITE_MEMORY"))==0) cmd=RCU_WRITE_MEMORY;
  else if (strncmp(pCommand, "RCU_READ_FW_VERSION", keySize=strlen("RCU_READ_FW_VERSION"))==0) cmd=RCU_READ_FW_VERSION;
  else if (strncmp(pCommand, "RCU_RESET", keySize=strlen("RCU_RESET"))==0) cmd=RCU_RESET;
  else if (strncmp(pCommand, "RCU_SYNC_SCLK_L1TTC", keySize=strlen("RCU_SYNC_SCLK_L1TTC"))==0) cmd=RCU_SYNC_SCLK_L1TTC;
  else if (strncmp(pCommand, "RCU_ALTRO_INSTRUCTION_EXECUTE", keySize=strlen("RCU_ALTRO_INSTRUCTION_EXECUTE"))==0) cmd=RCU_ALTRO_INSTRUCTION_EXECUTE;
  else if (strncmp(pCommand, "RCU_ALTRO_INSTRUCTION_CLEAR", keySize=strlen("RCU_ALTRO_INSTRUCTION_CLEAR"))==0) cmd=RCU_ALTRO_INSTRUCTION_CLEAR;
  else if (strncmp(pCommand, "RCU_ALTRO_INSTRUCTION_WRITE_FILE", keySize=strlen("RCU_ALTRO_INSTRUCTION_WRITE_FILE"))==0) cmd=RCU_ALTRO_INSTRUCTION_WRITE_FILE;
  else if (strncmp(pCommand, "RCU_ALTRO_INSTRUCTION_READ_FILE", keySize=strlen("RCU_ALTRO_INSTRUCTION_READ_FILE"))==0) cmd=RCU_ALTRO_INSTRUCTION_READ_FILE;
  else if (strncmp(pCommand, "RCU_CONFGFEC", keySize=strlen("RCU_CONFGFEC"))==0) cmd=RCU_CONFGFEC;
  else if (strncmp(pCommand, "RCU_WRITE_EN_INT_BA", keySize=strlen("RCU_WRITE_EN_INT_BA"))==0) cmd=RCU_WRITE_EN_INT_BA;
  else if (strncmp(pCommand, "RCU_READ_EN_INT_BA", keySize=strlen("RCU_READ_EN_INT_BA"))==0) cmd=RCU_READ_EN_INT_BA;
  else if (strncmp(pCommand, "REG_CFG_EXECUTE", keySize=strlen("REG_CFG_EXECUTE"))==0) cmd=REG_CFG_EXECUTE;
  else if (strncmp(pCommand, "REG_CFG_EXECUTE_SINGLE", keySize=strlen("REG_CFG_EXECUTE_SINGLE"))==0) cmd=REG_CFG_EXECUTE_SINGLE;
  else if (strncmp(pCommand, "REG_CFG_CLEAR", keySize=strlen("REG_CFG_CLEAR"))==0) cmd=REG_CFG_CLEAR;
  else if (strncmp(pCommand, "REG_CFG_WRITE_FILE", keySize=strlen("REG_CFG_WRITE_FILE"))==0) cmd=REG_CFG_WRITE_FILE;
  else if (strncmp(pCommand, "REG_CFG_READ_FILE", keySize=strlen("REG_CFG_READ_FILE"))==0) cmd=REG_CFG_READ_FILE;
  else if (strncmp(pCommand, "RCU_DRIVER_RELOAD", keySize=strlen("RCU_DRIVER_RELOAD"))==0) cmd=RCU_DRIVER_RELOAD;
  else if (strncmp(pCommand, "RCU_DRIVER_UNLOAD", keySize=strlen("RCU_DRIVER_UNLOAD"))==0) cmd=RCU_DRIVER_UNLOAD;
  else if (strncmp(pCommand, "RCU_DRIVER_LOAD", keySize=strlen("RCU_DRIVER_LOAD"))==0) cmd=RCU_DRIVER_LOAD;
  else if (strncmp(pCommand, "RCU_TEST_ERROR", keySize=strlen("RCU_TEST_ERROR"))==0) cmd=RCU_TEST_ERROR;

  if(!cmd || !keySize) return 0;
  const char* pBuffer=pCommand;
  pBuffer+=keySize;
  int len=strlen(pCommand);
  len-=keySize;
  while(*pBuffer==' ' && *pBuffer!=0 && len>0) {pBuffer++; len--;} // skip all blanks
  const char* pPayload=NULL;
  uint32_t iPayloadSize=0;
  uint32_t data=0;
  uint32_t parameter=0;
  switch (cmd){
  case RCU_WRITE_MEMORY:
    {
      const char* pBlank=NULL;
      if(sscanf(pBuffer, "0x%x", &parameter)>0 && (pBlank=strchr(pBuffer, ' '))!=NULL && sscanf(pBlank, " 0x%x", &data)>0){
        pPayload=(const char*)&data;
        iPayloadSize=sizeof(data);
      }
    }
    break;
  default:
    sscanf(pBuffer, "%d", &data);
    pPayload=(const char*)&data;
    iPayloadSize=sizeof(data);
    parameter=data;
    break;
  }
  return issue(cmd, parameter, pPayload, iPayloadSize, rb)>=0;
}
int DevRcu::SetAfl(uint32_t value){
  CE_Debug("Seting AFL to value %x\n", value);
  int nRet=0;
  uint32_t afl=0x0;
  nRet=fpMsgbuffer->SingleRead(V2_FECActiveList_RO, &afl);
  if (nRet<0){
    CE_Warning("can not read RCU AFL %#x: SingleRead failed (%d)\n", V2_FECActiveList_RO, nRet);
    return -1;
  }
  if(value==afl) return 0;
  CEStateMachine* pParent=GetParentDevice();
  if(!pParent) return -3;
  int tmpState=kStateElse;
  if(value>afl) tmpState=kStateRampUp;
  else          tmpState=kStateRampDown;
  int orgState=pParent->GetCurrentState();
  pParent->ChangeState(tmpState);
  for(uint32_t i=0;i<32;i++){
    CE_Debug("n=%x, i=%x, value=%x, afl=%x\n", 0, i, value, afl);
    if((value&(0x1<<i))^(afl&(0x1<<i))){
      if((value&(0x1<<i))==0) afl=afl&~(0x1<<i);
      else                    afl=afl| (0x1<<i);
      CE_Debug("n=%x, i=%x, value=%x, afl=%x\n", 1, i, value, afl);
      nRet=fpMsgbuffer->SingleWrite(V2_FECActiveList, afl);
      if (nRet<0){
        CE_Error("SingleWrite failed to write %p to RCU AFL with error %i\n", V2_FECActiveList, nRet);
        return -2;
      }
      else CE_Debug("Set AFL to %#x\n", afl);
      usleep(300000);
    }
  }
  nRet=fpMsgbuffer->SingleWrite(V2_CMDRESETFEC, 0);
  if (nRet<0){
    CE_Error("SingleWrite failed to write %p to V2_CMDRESETFEC with error %i\n", V2_CMDRESETFEC, nRet);
    return -3;
  }
  pParent->ChangeState(orgState);
  return 1;
}
int DevRcu::RcuAdcConf(uint32_t adc){
  fpMsgbuffer->SingleWrite(0x801a,adc+0x11);
  fpMsgbuffer->SingleWrite(0x801a,adc+0x51);
  fpMsgbuffer->SingleWrite(0x801a,adc+0x5533);
  fpMsgbuffer->SingleWrite(0x801a,adc+0x4b23);
  return 0;
}
int DevRcu::RcuAdcRead(Ser::TceServiceData* pData, int32_t adc, int32_t unused, void* parameter){
  if(!parameter) return -1;
  DevRcu* rcu=(DevRcu*) parameter;
  int nRet=0;
  uint32_t rawValue=0;
  nRet|=rcu->fpMsgbuffer->SingleWrite(0x801a,adc+0xff000000);
  nRet|=rcu->fpMsgbuffer->SingleRead(0x800d,&rawValue);
  rawValue=rawValue>>6;
  //factor=0.00390625=1/256=0.015625*0.25=1/64*1/4
  //rawValue&=0xffc0;
  if(nRet>=0) pData->fVal=rawValue*0.25;
  else pData->fVal=nRet;
  return nRet;
}
int DevRcu::ArmorLocal(){
  std::string name=GetName();
  fServices.push_back(new SerMbAddrS(name+"_AFL"          , V2_FECActiveList_RO, 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"_FECERR_A"     , V2_FECErrA_RO      , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"_FECERR_B"     , V2_FECErrB_RO      , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"_FWVERSION"    , RCUFwVersion       , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"_DCS_FWVERSION", DCSFwVersion       , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"_DCS_OLDMODE"  , DCSOldMode         , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"_SLC_STATUS"   , FECErrReg          , 1, 0, fpMsgbuffer));
  std::vector<uint32_t> temp;
  issue(REG_CFG_READ_FILE, 0, 0, 0, temp);
  issue(RCU_ALTRO_INSTRUCTION_READ_FILE, 0, 0, 0, temp);
  RcuAdcConf(0);
  RcuAdcConf(4);
  name=GetName()+"_TEMP1";
  Ser::RegisterService(Ser::eDataTypeFloat, name.c_str(), 0.5, DevRcu::RcuAdcRead, 0, 0, 0, this);
  name=GetName()+"_TEMP2";
  Ser::RegisterService(Ser::eDataTypeFloat, name.c_str(), 0.5, DevRcu::RcuAdcRead, 0, 4, 0, this);
  return 0;
}
int DevRcu::issue(uint32_t cmd, uint32_t parameter, const char* pData, int iDataSize, std::vector<uint32_t>& rb){
  int iResult=0;
  CE_Debug("CErcu::TranslateRcuCommand cmd=%#x parameter=%#x datasize=%d\n", cmd, parameter, iDataSize);
  switch (cmd) {
  case RCU_ALTRO_INSTRUCTION_STORE:
    {
      uint32_t* mem=(uint32_t*)pData;
      for(uint32_t i=0;i<(parameter/2);i++) fAltroInstr[(mem[i*2]&0x37FFFF)] = mem[i*2+1];
      iResult=parameter*4;
    }
    break;
  case RCU_ALTRO_INSTRUCTION_EXECUTE:
    {
      uint32_t someError=0;
      uint32_t stickyErrorRepeat=2;
      do{
        uint32_t* imem=new uint32_t[V2_ALTROInstMEM_SIZE];
        uint32_t* rmem=new uint32_t[V2_ALTROResultMEM_SIZE];
        if(!imem || !rmem){
          CE_Error("Failed to allocate memory for instruction and result memory.\n");
          if(imem){delete[] imem; imem=0;}
          if(rmem){delete[] rmem; rmem=0;}
          SetErrorState();
          return 0;
        }
        uint32_t index=0;
        uint32_t end=0;
        uint32_t afl=0;
        fpMsgbuffer->SingleRead(V2_FECActiveList_RO,&afl);
        std::map<uint32_t,uint32_t>::iterator it;
        it=fAltroInstr.begin();
        while(true){
          if(it==fAltroInstr.end()){
            if(index==0) break;
            imem[index]=0x380000;
            index++;
            end=1;
          }
          else{
            uint32_t fec=((it->first>>12)&0x1f);
            if(((afl>>fec)&0x1)){
              imem[index]=(*it).first;
              index++;
              imem[index]=(*it).second;
              index++;
            }
            it++;
          }
          if(index>=V2_ALTROInstMEM_SIZE || end==1){
            uint32_t errorRes=0;
            uint32_t errorA=0;
            uint32_t errorB=0;
            uint32_t errorBusy=0;
            uint32_t i=0;
            uint32_t j=0;
            do{
              uint32_t ready=0;
              j=0;
              while(j<128 && ready==0){
                fpMsgbuffer->SingleRead(V2_BUSBSY, &errorBusy);
                ready=1;
                if((errorBusy&0x1)!=0) ready=0;
                j++;
              }
              if(j>=128 && ready==0) CE_Error("Altro bus still busy (%#x) )after %d attempts. Trying anyway.\n", errorBusy, j);
              fpMsgbuffer->MultipleWrite(V2_ALTROInstMEM, imem, V2_ALTROInstMEM_SIZE);
              fpMsgbuffer->SingleWrite(V2_CMDExecALTRO, 0);
              fpMsgbuffer->MultipleRead(V2_ALTROResultMEM, V2_ALTROResultMEM_SIZE, rmem);
              for(j=0;j<index;j++) if((rmem[j]&0x100000)!=0){errorRes=rmem[j]; break;}
              fpMsgbuffer->SingleRead(V2_FECErrA_RO,&errorA);
              fpMsgbuffer->SingleRead(V2_FECErrB_RO,&errorB);
              fpMsgbuffer->SingleRead(V2_BUSBSY,&errorBusy);
              i++;
            }
            while(i<4 && (errorA!=0 || errorB!=0 || (errorRes&0x100000)!=0 || (errorBusy&0x1)!=0));
            if(i>=4 && (errorRes&100000)!=0) {someError=1; CE_Error("Execution of instruction memory failed; result memory line %#x: %#x (stickyErrorRepeat=%#x)\n", j, errorRes, stickyErrorRepeat);}
            if(i>=4 && errorA!=0)   {someError=1; CE_Error("Execution of instruction memory failed; FEC A error register: %#x (stickyErrorRepeat=%#x)\n", errorA, stickyErrorRepeat);}
            if(i>=4 && errorB!=0)   {someError=1; CE_Error("Execution of instruction memory failed; FEC B error register: %#x (stickyErrorRepeat=%#x)\n", errorB, stickyErrorRepeat);}
            if(i>=4 && (errorBusy&0x2)!=0){someError=1; CE_Error("Execution of instruction memory failed; BusBusy register: %#x (stickyErrorRepeat=%#x)\n", errorBusy, stickyErrorRepeat);}
            index=0;
          }
        }
        if(imem){delete[] imem; imem=0;}
        if(rmem){delete[] rmem; rmem=0;}
        uint32_t errorA=0;
        uint32_t errorB=0;
        fpMsgbuffer->SingleRead(V2_FECErrA_RO,&errorA);
        fpMsgbuffer->SingleRead(V2_FECErrB_RO,&errorB);
        if((errorA&0x1) || (errorB&0x1)){
          stickyErrorRepeat--;
          if(!stickyErrorRepeat) {someError=1; CE_Error("Execution of instruction memory failed; unrecoverable sticky bit found, FEC A error register: %#x, FEC B error register: %#x\n", errorA, errorB);}
          else                   {someError=0; fpMsgbuffer->SingleWrite(V2_CMDRESET,0);}
        }
        else stickyErrorRepeat=0;
        if(someError) {SetErrorState(); CE_Error("Entering error state\n");}
      }
      while(stickyErrorRepeat>0);
      iResult=0;
    }
    break;
  case RCU_ALTRO_INSTRUCTION_CLEAR:
    fAltroInstr.clear();
    iResult=0;
    break;
  case RCU_ALTRO_INSTRUCTION_READ_FILE:
    {
      std::ifstream imemFile(imemFileName.c_str(), std::ios::in|std::ios::binary);
      if(imemFile.fail()){
        CE_Warning("Opening file '%s' failed.\n", imemFileName.c_str());
        break;
      }
      imemFile.seekg(0, std::ios::end);
      uint32_t size = imemFile.tellg()/4; 
      imemFile.seekg (0, std::ios::beg);
      uint32_t* imem=new uint32_t[size];
      if(!imem){
        CE_Error("Failed to allocate memory for instruction memory.\n");
        delete[] imem; imem=0;
        return 0;
      }
      imemFile.read((char*)imem, size*4);
      imemFile.close();
      for(uint32_t index=0;index<(size/2);index++){
        fAltroInstr[imem[index*2]]=imem[index*2+1];
      }
      if(imem){delete[] imem; imem=0;}
      iResult=0;
    }
    break;
  case RCU_ALTRO_INSTRUCTION_WRITE_FILE:
    {
      uint32_t size=fAltroInstr.size()*2;
      uint32_t* imem=new uint32_t[size];
      if(!imem){
        CE_Error("Failed to allocate memory for instruction memory.\n");
        delete[] imem; imem=0;
        return 0;
      }
      std::ofstream imemFile(imemFileName.c_str(), std::ios::out|std::ios::binary);
      iResult=0;
      if(imemFile.fail()){
        CE_Error("Opening file '%s' failed.\n", imemFileName.c_str());
        SetErrorState();
        break;
      }
      std::map<uint32_t,uint32_t>::iterator it;
      it=fAltroInstr.begin();
      for(uint32_t index=0;index<(size/2);index++){
        imem[index*2  ]=(*it).first;
        imem[index*2+1]=(*it).second;
        it++;
      }
      imemFile.write((char*)imem, size*4);
      imemFile.close();
      if(imem){delete[] imem; imem=0;}
    }
    break;
  case RCU_ALTRO_INSTRUCTION_FILE_NAME:
    {
      imemFileName=pData;
      CE_Event("Set RCU_ALTRO_INSTRUCTION_FILE_NAME to %s.\n", imemFileName.c_str());
      std::ifstream imemFile(imemFileName.c_str(), std::ios::in|std::ios::binary);
      iResult=parameter*4;
      if(imemFile.fail()){
        CE_Error("Opening file '%s' failed.\n", imemFileName.c_str());
        SetErrorState();
        break;
      }
      imemFile.close();
    }
    break;
  case RCU_EXEC_INSTRUCTION:
    {
      uint32_t* mem=(uint32_t*)pData;
      fpMsgbuffer->MultipleWrite(V2_ALTROInstMEM, mem, parameter);
      fpMsgbuffer->SingleWrite(V2_CMDExecALTRO, 0);
      iResult=parameter*4;
    }
    break;
  case RCU_EXEC:
    {
      fpMsgbuffer->SingleWrite(V2_CMDExecALTRO, parameter);
      iResult=0;
    }
    break;
  case RCU_STOP_EXEC:
    {
      fpMsgbuffer->SingleWrite(V2_CMDAbortALTRO, 0);
      iResult=0;
    }
    break;
  case RCU_WRITE_ALTRO_PEDMEM:
    {
      uint32_t* pedData=(uint32_t*)pData;
      uint32_t imem[4096];
      for (uint32_t i=0;i<1024;i++){                         // time bin 0 ... 1023
        imem[4*i+0] = (2<<20) | (parameter<<5) | 0xd;     // PMADD
        imem[4*i+1] = i;                                  // time bin 0 ... 1023
        imem[4*i+2] = (2<<20) | (parameter<<5) | 0x7;     // PMDTA
        imem[4*i+3] = (pedData[i/3]>>((i%3)*10)) & 0x3ff; // value for this time bin
      }
    //imem[4*1024] = 0x380000; // no space for ENDSEQ; will be executed anyway
      fpMsgbuffer->MultipleWrite(V2_ALTROInstMEM, imem, 4096);
      fpMsgbuffer->SingleWrite(V2_CMDExecALTRO, 0);
      iResult=342*4;
    }
    break;
  case REG_CFG_STORE:
    {
      uint32_t* regCfg=(uint32_t*)pData;
      for(uint32_t i=0;i<(parameter/2);i++) fRegCfg[(regCfg[i*2])] = regCfg[i*2+1];
      iResult=parameter*4;
    }
    break;
  case REG_CFG_EXECUTE:
    {
      std::map<uint32_t,uint32_t>::iterator it;
      it=fRegCfg.begin();
      for(uint32_t index=0;index<(fRegCfg.size());index++){
        if((*it).first==V2_FECActiveList) SetAfl((*it).second); 
        else fpMsgbuffer->SingleWrite((*it).first, (*it).second);
        it++;
      }
      iResult=0;
    }
    break;
  case REG_CFG_EXECUTE_SINGLE:
    {
      std::map<uint32_t,uint32_t>::iterator it=fRegCfg.find(*(uint32_t*)pData);
      iResult=4;
      if(it==fRegCfg.end()){
        CE_Error("Register 0x%x not found by REG_CFG_EXECUTE_SINGLE in memory.\n", *(uint32_t*)pData);
        SetErrorState();
        break;
      }
      if(*(uint32_t*)pData==V2_FECActiveList) SetAfl(fRegCfg[*(uint32_t*)pData]); 
      else fpMsgbuffer->SingleWrite(*(uint32_t*)pData, fRegCfg[*(uint32_t*)pData]);
    }
    break;
  case REG_CFG_CLEAR:
    fRegCfg.clear();
    iResult=0;
    break;
  case REG_CFG_READ_FILE:
    {
      std::ifstream regCfgFile(regCfgFileName.c_str(), std::ios::in|std::ios::binary);
      if(regCfgFile.fail()){
        CE_Warning("Opening file '%s' failed.\n", regCfgFileName.c_str());
        break;
      }
      regCfgFile.seekg(0, std::ios::end);
      uint32_t size = regCfgFile.tellg()/4; 
      regCfgFile.seekg (0, std::ios::beg);
      uint32_t* regCfg=new uint32_t[size];
      if(!regCfg){
        CE_Error("Failed to allocate memory for register configuration.\n");
        delete[] regCfg; regCfg=0;
        return 0;
      }
      regCfgFile.read((char*)regCfg, size*4);
      regCfgFile.close();
      for(uint32_t index=0;index<(size/2);index++){
        fRegCfg[regCfg[index*2]]=regCfg[index*2+1];
      }
      if(regCfg){delete[] regCfg; regCfg=0;}
      iResult=0;
    }
    break;
  case REG_CFG_WRITE_FILE:
    {
      uint32_t size=fRegCfg.size()*2;
      uint32_t* regCfg=new uint32_t[size];
      if(!regCfg){
        CE_Error("Failed to allocate memory for register configuration.\n");
        delete[] regCfg; regCfg=0;
        return 0;
      }
      std::ofstream regCfgFile(regCfgFileName.c_str(), std::ios::out|std::ios::binary);
      iResult=0;
      if(regCfgFile.fail()){
        CE_Error("Opening file '%s' failed.\n", regCfgFileName.c_str());
        SetErrorState();
        break;
      }
      std::map<uint32_t,uint32_t>::iterator it;
      it=fRegCfg.begin();
      for(uint32_t index=0;index<(size/2);index++){
        regCfg[index*2  ]=(*it).first;
        regCfg[index*2+1]=(*it).second;
        it++;
      }
      regCfgFile.write((char*)regCfg, size*4);
      regCfgFile.close();
      if(regCfg){delete[] regCfg; regCfg=0;}
    }
    break;
  case REG_CFG_FILE_NAME:
    {
      regCfgFileName=pData;
      CE_Event("Set REG_CFG_FILE_NAME to '%s'\n", regCfgFileName.c_str());
      std::ifstream regCfgFile(regCfgFileName.c_str(), std::ios::in|std::ios::binary);
      iResult=parameter*4;
      if(regCfgFile.fail()){
        CE_Error("Opening file '%s' failed\n", regCfgFileName.c_str());
        SetErrorState();
        break;
      }
      regCfgFile.close();
    }
    break;
  case RCU_WRITE_MEMORY:
    fpMsgbuffer->SingleWrite(parameter,*(uint32_t*)pData);
    iResult=4;
    break;
  case RCU_READ_MEMORY:
    {
      uint32_t rbsize=rb.size();
      rb.resize(rbsize+1);
      fpMsgbuffer->SingleRead(parameter, &rb[rbsize]);
      iResult=4;
    }
    break;
  case RCU_WRITE_MEMBLOCK://write block to rcu memory, words to write given by parameter, address given by first 32 bit word of pData, data is following
    {
      uint32_t* mem=(uint32_t*)pData;
      fpMsgbuffer->MultipleWrite(mem[0], mem+1, parameter);
      iResult=parameter*4;
    }
    break;
  case RCU_READ_MEMBLOCK://read block from rcu memory, words to read given by parameter, address given by first 32 bit word of pData
    {
      uint32_t* mem=(uint32_t*)pData;
      uint32_t rbsize=rb.size();
      rb.resize(rbsize+parameter);
      fpMsgbuffer->MultipleRead(mem[0], parameter, &rb[rbsize]);
      iResult=4;
    }
    break;
  case RCU_WRITE_INSTRUCTION:
    {
      uint32_t* mem=(uint32_t*)pData;
      fpMsgbuffer->MultipleWrite(V2_ALTROInstMEM, mem, parameter);
      iResult=parameter*4;
    }
    break;
  case RCU_READ_INSTRUCTION:
    {
      uint32_t rbsize=rb.size();
      rb.resize(rbsize+parameter);
      fpMsgbuffer->MultipleRead(V2_ALTROInstMEM, parameter, &rb[rbsize]);
      iResult=0;
    }
    break;
  case RCU_WRITE_RESULT:
    {
      uint32_t errorBusy=0;
      uint32_t ready=0;
      uint32_t j=0;
      while(j<128 && ready==0){
        fpMsgbuffer->SingleRead(V2_BUSBSY, &errorBusy);
        ready=1;
        if((errorBusy&0x1)!=0) ready=0;
        j++;
      }
      if(j>=128 && ready==0) CE_Error("Altro bus still busy (%#x) )after %d attempts. Trying anyway.\n", errorBusy, j);
      uint32_t* mem=(uint32_t*)pData;
      fpMsgbuffer->MultipleWrite(V2_ALTROResultMEM, mem, parameter);
      iResult=parameter*4;
    }
    break;
  case RCU_READ_RESULT:
    {
      uint32_t errorBusy=0;
      uint32_t ready=0;
      uint32_t j=0;
      while(j<128 && ready==0){
        fpMsgbuffer->SingleRead(V2_BUSBSY, &errorBusy);
        ready=1;
        if((errorBusy&0x1)!=0) ready=0;
        j++;
      }
      if(j>=128 && ready==0) CE_Error("Altro bus still busy (%#x) )after %d attempts. Trying anyway.\n", errorBusy, j);
      uint32_t rbsize=rb.size();
      rb.resize(rbsize+parameter);
      fpMsgbuffer->MultipleRead(V2_ALTROResultMEM, parameter, &rb[rbsize]);
      iResult=0;
    }
    break;
  case RCU_WRITE_HEADER:
    {
      uint32_t* mem=(uint32_t*)pData;
      fpMsgbuffer->MultipleWrite(DataHEADER, mem, parameter);
      iResult=parameter*4;
    }
    break;
  case RCU_READ_HEADER:
    {
      uint32_t rbsize=rb.size();
      rb.resize(rbsize+parameter);
      fpMsgbuffer->MultipleRead(DataHEADER, parameter, &rb[rbsize]);
      iResult=0;
    }
    break;
  case WRITE_ALTRO_HLMEM:
    {
      uint32_t* mem=(uint32_t*)pData;
      fpMsgbuffer->MultipleWrite(V2_ALTROHlMEM, mem, parameter);
      iResult=parameter*4;
    }
    break;
  case READ_ALTRO_HLMEM:
    {
      uint32_t rbsize=rb.size();
      rb.resize(rbsize+parameter);
      fpMsgbuffer->MultipleRead(V2_ALTROHlMEM, parameter, &rb[rbsize]);
      iResult=0;
    }
    break;
  case RCU_WRITE_ACL:
    {
      uint32_t* mem=(uint32_t*)pData;
      fpMsgbuffer->MultipleWrite(V2_ALTROACL, mem, parameter);
      iResult=parameter*4;
    }
    break;
  case RCU_READ_ACL:
    {
      uint32_t rbsize=rb.size();
      rb.resize(rbsize+parameter);
      fpMsgbuffer->MultipleRead(V2_ALTROACL, parameter, &rb[rbsize]);
      iResult=0;
    }
    break;
  case RCU_WRITE_TRGCFG:
    fpMsgbuffer->SingleWrite(V2_AltroTrCfg,*(uint32_t*)pData);
    iResult=4;
    break;
  case RCU_READ_TRGCFG:
    {
      uint32_t rbsize=rb.size();
      rb.resize(rbsize+1);
      fpMsgbuffer->SingleRead(V2_AltroTrCfg, &rb[rbsize]);
      iResult=0;
    }
    break;
  case RCU_WRITE_PMCFG:
    fpMsgbuffer->SingleWrite(AltroPmCfg,*(uint32_t*)pData);
    iResult=4;
    break;
  case RCU_READ_PMCFG:
    {
      uint32_t rbsize=rb.size();
      rb.resize(rbsize+1);
      fpMsgbuffer->SingleRead(AltroPmCfg, &rb[rbsize]);
      iResult=0;
    }
    break;
  case RCU_WRITE_AFL:
    SetAfl(*(uint32_t*)pData);
    iResult=4;
    break;
  case RCU_READ_AFL:
    {
      uint32_t rbsize=rb.size();
      rb.resize(rbsize+1);
      fpMsgbuffer->SingleRead(V2_FECActiveList_RO, &rb[rbsize]);
      iResult=0;
    }
    break;
  case RCU_RESET:
    {
      uint32_t cmd=0;
      switch(parameter){
      case 1:  cmd=V2_CMDRESETFEC; break;
      case 2:  cmd=V2_CMDRESETRCU; break;
      default: cmd=V2_CMDRESET;    break;
      }
      fpMsgbuffer->SingleWrite(cmd,0);
      sleep(1);
      iResult=0;
    }
    break;
  case RCU_L1TRG_SELECT:
    {
      uint32_t cmd=0;
      switch(parameter){
      case 1:  cmd=CMDL1TTC; break;
      case 2:  cmd=CMDL1I2C; break;
      default: cmd=CMDL1CMD; break;
      }
      fpMsgbuffer->SingleWrite(cmd,0);
      iResult=0;
    }
    break;
  case RCU_SEND_L1_TRIGGER:
    fpMsgbuffer->SingleWrite(CMDL1,0);
    iResult=0;
    break;
  case RCU_READ_FW_VERSION:
    {
      uint32_t rbsize=rb.size();
      rb.resize(rbsize+1);
      fpMsgbuffer->SingleRead(RCUFwVersion, &rb[rbsize]);
      iResult=0;
    }
    break;
  case RCU_SYNC_SCLK_L1TTC:
    fpMsgbuffer->SingleWrite(V2_CMDSCLKsync,0);
    iResult=0;
    break;
  case WRITE_TTC_L1_LATENCY:
    fpMsgbuffer->SingleWrite(TTCL1Latency,*(uint32_t*)pData);
    iResult=4;
    break;
  case READ_TTC_L1_LATENCY:
    {
      uint32_t rbsize=rb.size();
      rb.resize(rbsize+1);
      fpMsgbuffer->SingleRead(TTCL1Latency, &rb[rbsize]);
      iResult=0;
    }
    break;
  case WRITE_TTC_L1_MSG_LATENCY:
    fpMsgbuffer->SingleWrite(TTCL1MsgLatency,*(uint32_t*)pData);
    iResult=4;
    break;
  case READ_TTC_L1_MSG_LATENCY:
    {
      uint32_t rbsize=rb.size();
      rb.resize(rbsize+1);
      fpMsgbuffer->SingleRead(TTCL1MsgLatency, &rb[rbsize]);
      iResult=0;
    }
    break;
  case WRITE_ALTROIF:
    fpMsgbuffer->SingleWrite(V2_AltroIf,*(uint32_t*)pData);
    iResult=4;
     break;
  case READ_ALTROIF:
    {
      uint32_t rbsize=rb.size();
      rb.resize(rbsize+1);
      fpMsgbuffer->SingleRead(V2_AltroIf, &rb[rbsize]);
      iResult=0;
    }
    break;
  case WRITE_RDO_MOD:
    fpMsgbuffer->SingleWrite(V2_RDOMod,*(uint32_t*)pData);
    iResult=4;
    break;
  case READ_RDO_MOD:
    {
      uint32_t rbsize=rb.size();
      rb.resize(rbsize+1);
      fpMsgbuffer->SingleRead(V2_RDOMod, &rb[rbsize]);
      iResult=0;
    }
    break;
  case WRITE_TTCCONTROL:
    fpMsgbuffer->SingleWrite(TTCControl,*(uint32_t*)pData);
    iResult=4;
    break;
  case READ_TTCCONTROL:
    {
      uint32_t rbsize=rb.size();
      rb.resize(rbsize+1);
      fpMsgbuffer->SingleRead(TTCControl, &rb[rbsize]);
      iResult=0;
    }
    break;
  case WRITE_TTC_ROICONFIG1:
    fpMsgbuffer->SingleWrite(TTCROIConfig1,*(uint32_t*)pData);
    iResult=4;
    break;
  case READ_TTC_ROICONFIG1:
    {
      uint32_t rbsize=rb.size();
      rb.resize(rbsize+1);
      fpMsgbuffer->SingleRead(TTCROIConfig1, &rb[rbsize]);
      iResult=0;
    }
    break;
  case WRITE_TTC_ROICONFIG2:
    fpMsgbuffer->SingleWrite(TTCROIConfig2,*(uint32_t*)pData);
    iResult=4;
    break;
  case READ_TTC_ROICONFIG2:
    {
      uint32_t rbsize=rb.size();
      rb.resize(rbsize+1);
      fpMsgbuffer->SingleRead(TTCROIConfig2, &rb[rbsize]);
      iResult=0;
    }
    break;
  case WRITE_TTC_L2_LATENCY:
    fpMsgbuffer->SingleWrite(TTCL2Latency,*(uint32_t*)pData);
    iResult=4;
    break;
  case READ_TTC_L2_LATENCY:
    {
      uint32_t rbsize=rb.size();
      rb.resize(rbsize+1);
      fpMsgbuffer->SingleRead(TTCL2Latency, &rb[rbsize]);
      iResult=0;
    }
    break;
  case WRITE_TTC_ROILATENCY:
    fpMsgbuffer->SingleWrite(TTCRoILatency,*(uint32_t*)pData);
    iResult=4;
    break;
  case READ_TTC_ROILATENCY:
    {
      uint32_t rbsize=rb.size();
      rb.resize(rbsize+1);
      fpMsgbuffer->SingleRead(TTCRoILatency, &rb[rbsize]);
      iResult=0;
    }
    break;
  case READ_TTC_PREPULSECOUNTER:
    {
      uint32_t rbsize=rb.size();
      rb.resize(rbsize+1);
      fpMsgbuffer->SingleRead(TTCPrePulseCnt, &rb[rbsize]);
      iResult=0;
    }
    break;
  case READ_TTC_BCIDLOCAL:
    {
      uint32_t rbsize=rb.size();
      rb.resize(rbsize+1);
      fpMsgbuffer->SingleRead(TTCBCIDLocal, &rb[rbsize]);
      iResult=0;
    }
    break;
  case READ_TTC_L0COUNTER:
    {
      uint32_t rbsize=rb.size();
      rb.resize(rbsize+1);
      fpMsgbuffer->SingleRead(TTCL0Counter, &rb[rbsize]);
      iResult=0;
    }
    break;
  case READ_TTC_L1COUNTER:
    {
      uint32_t rbsize=rb.size();
      rb.resize(rbsize+1);
      fpMsgbuffer->SingleRead(TTCL1Counter, &rb[rbsize]);
      iResult=0;
    }
    break;
  case READ_TTC_L1MSGCOUNTER:
    {
      uint32_t rbsize=rb.size();
      rb.resize(rbsize+1);
      fpMsgbuffer->SingleRead(TTCL1MsgCounter, &rb[rbsize]);
      iResult=0;
    }
    break;
  case READ_TTC_L2ACOUNTER:
    {
      uint32_t rbsize=rb.size();
      rb.resize(rbsize+1);
      fpMsgbuffer->SingleRead(TTCL2aCounter, &rb[rbsize]);
      iResult=0;
    }
    break;
  case READ_TTC_L2RCOUNTER:
    {
      uint32_t rbsize=rb.size();
      rb.resize(rbsize+1);
      fpMsgbuffer->SingleRead(TTCL2rCounter, &rb[rbsize]);
      iResult=0;
    }
    break;
  case READ_TTC_ROICOUNTER:
    {
      uint32_t rbsize=rb.size();
      rb.resize(rbsize+1);
      fpMsgbuffer->SingleRead(TTCRoICounter, &rb[rbsize]);
      iResult=0;
    }
    break;
  case WRITE_ALTROCFG1:
    fpMsgbuffer->SingleWrite(V2_AltroCfg1,*(uint32_t*)pData);
    iResult=4;
    break;
  case READ_ALTROCFG1:
    {
      uint32_t rbsize=rb.size();
      rb.resize(rbsize+1);
      fpMsgbuffer->SingleRead(V2_AltroCfg1, &rb[rbsize]);
      iResult=0;
    }
    break;
  case WRITE_ALTROCFG2:
    fpMsgbuffer->SingleWrite(V2_AltroCfg2,*(uint32_t*)pData);
    iResult=4;
    break;
  case READ_ALTROCFG2:
    {
      uint32_t rbsize=rb.size();
      rb.resize(rbsize+1);
      fpMsgbuffer->SingleRead(V2_AltroCfg2, &rb[rbsize]);
      iResult=0;
    }
    break;
  case WRITE_RCUID:
    fpMsgbuffer->SingleWrite(V2_RCUId,*(uint32_t*)pData);
    iResult=4;
    break;
  case READ_RCUID:
    {
      uint32_t rbsize=rb.size();
      rb.resize(rbsize+1);
      fpMsgbuffer->SingleRead(V2_RCUId, &rb[rbsize]);
      iResult=0;
    }
    break;
  case RCU_CONFGFEC:
    fpMsgbuffer->SingleWrite(CONFGFEC,1);
    sleep(1);
    iResult=0;
    break;
  case RCU_WRITE_EN_INT_BA:
    fpMsgbuffer->SingleWrite(FECINTmode, *(uint32_t*)pData);
    iResult=4;
    break;
  case RCU_READ_EN_INT_BA:
    {
      uint32_t rbsize=rb.size();
      rb.resize(rbsize+1);
      fpMsgbuffer->SingleRead(FECINTmode, &rb[rbsize]);
      iResult=0;
    }
    break;
  case RCU_DRIVER_RELOAD:
    if(fpMsgbuffer->DriverReload()<0){
      CE_Error("RCU driver reload failed with error\n");
    }
    iResult=0;
    break;
  case RCU_DRIVER_UNLOAD:
    if(fpMsgbuffer->DriverUnload()<0){
      CE_Error("RCU driver unload failed with error\n");
    }
    iResult=0;
    break;
  case RCU_DRIVER_LOAD:
    if(fpMsgbuffer->DriverLoad()<0){
      CE_Error("RCU driver load failed with error\n");
    }
    iResult=0;
    break;
  case RCU_TEST_ERROR:
    CE_Debug("Rcu testing error state\n");
    SetErrorState();
    break;
  default:
    CE_Warning("unrecognized command id (%#x)\n", cmd);
    iResult=-ENOSYS;
    break;
  }
  return iResult;
}

/******************************************************************************/

DevFecaccessRcuI2c::DevFecaccessRcuI2c(DevMsgbuffer* msgbuffer): DevFecaccess(msgbuffer){
}
int DevFecaccessRcuI2c::AccessFecReg(uint32_t &data, uint32_t fec, uint32_t reg, uint32_t rnw){
  int nRet=0;
  uint32_t valid=0;
  nRet=GetFecAflStatus(valid, fec);
  if(nRet<0){
    CE_Warning("checking FEC %d failed, %d (HW address)\n", fec, nRet);
    return nRet;
  }
  if(valid==0){
//    Not really an "error"
//    CE_Warning("accessing FEC %d failed, not turned on\n", fec);
    return -2120;
  }
  uint32_t rawData=0;
  //Reset error register
  nRet=fpMsgbuffer->SingleWrite(FECResetErrReg, 0x0);
  if(nRet<0){
    CE_Warning("can not write RCU FECResetErrReg %#x: SingleWrite failed (%d)\n", FECResetErrReg, nRet);
    return -2121;
  }
  //Reset result register
  nRet=fpMsgbuffer->SingleWrite(FECResetResReg, 0x0);
  if(nRet<0){
    CE_Warning("can not write RCU FECResetResReg %#x: SingleWrite failed (%d)\n", FECResetResReg, nRet);
    return -2122;
  }
  uint32_t bcast    = 0x00000000;
  uint32_t base     = 0x00000000;
  uint32_t SCaddReg = (reg<<MSMCommand8_BCRegAdr) | (fec<<MSMCommand8_FECAdr) | (bcast<<MSMCommand8_bcast) | (rnw<<MSMCommand8_rnw) | (base<<MSMCommand8_base);
  //Set address register
  nRet=fpMsgbuffer->SingleWrite(FECSCAddREG, SCaddReg);
  if (nRet<0) {
    CE_Warning("can not write RCU SCaddReg 0x8005: SingleWrite failed (%d)\n", nRet);
    return -2123;
  }
  //Set data register
  nRet=fpMsgbuffer->SingleWrite(FECSCDataREG, data);
  if(nRet<0){
    CE_Warning("can not write RCU SCdataReg 0x8006: SingleWrite failed (%d)\n", nRet);
    return -2124;
  }
  //Execute command
  nRet=fpMsgbuffer->SingleWrite(FECSCCmdREG, 0x0);
  if(nRet<0){
    CE_Warning("can not write RCU SCDcmdReg 0x8010: SingleWrite failed (%d)\n", nRet);
    return -2125;
  }
  //Read error register
  rawData=0;
  nRet=fpMsgbuffer->SingleRead(FECErrReg, &rawData);
  if(nRet<0){
    CE_Warning("can not read RCU FECErrReg %#x: SingleRead failed (%d)\n", FECErrReg, nRet);
    return -2126;
  }
  //Error if FEC not active
  if(rawData&(0x1<<0)){
    CE_Warning("instruction to not active FEC %d (HW address)\n", fec);
    return -2127;
  }
  //Error if no acknowledge
  if (rawData&(0x1<<1)) {
    CE_Warning("no acknowledge from FEC %d (HW address)\n", fec);
    return -2128;
  }
  //Reading result
  rawData=0,
  nRet=fpMsgbuffer->SingleRead(FECResultREG, &rawData);
  if(nRet<0){
    CE_Warning("can not read RCU FECResultREG %#x: SingleRead failed\n", FECResultREG, nRet);
    return -2129;
  }
  data=(rawData & 0xffff);
  //Error if answer from wrong FEC
  if((rnw!=0) && (rawData>>16)!=fec){
    CE_Warning("FEC mismatch, addressed %d, got back %d (HW address)\n", fec, (rawData>>16));
    return -2130;
  }
  return 0;
}

DevFecaccessRcuGtl::DevFecaccessRcuGtl(DevMsgbuffer* msgbuffer): DevFecaccess(msgbuffer){
}
int DevFecaccessRcuGtl::AccessFecReg(uint32_t &data, uint32_t fec, uint32_t reg, uint32_t rnw){
  //To be implemented...
  return -2140;
}

/******************************************************************************/

//Constructor
DevActel::DevActel(int id,  CEStateMachine* pParent, DevMsgbuffer* msgbuffer):Dev("ACTEL", id, pParent),fpMsgbuffer(msgbuffer){
  AddTransition(new CETransition(kActionGoStandby, kStateStandby, std::string(kActionGoStandby_name), DevActelGO_STANDBY, this, 3, kStateError, kStateFailure, kStateDownloading));
  AddTransition(new CETransition(kActionStart, kStateRunning, std::string(kActionStart_name), NULL, NULL, 1, kStateStandby));
  AddTransition(new CETransition(kActionStop, kStateStandby, std::string(kActionStop_name), NULL, NULL, 1, kStateRunning));
  AddTransition(new CETransition(kActionGoReadback, kStateReadback, std::string(kActionGoReadback_name), NULL, NULL, 1, kStateStandby));
  AddTransition(new CETransition(kActionQuitReadback, kStateStandby, std::string(kActionQuitReadback_name), NULL, NULL, 1, kStateReadback));
  AddTransition(new CETransition(kActionGoScrubbing, kStateScrubbing, std::string(kActionGoScrubbing_name), NULL, NULL, 1, kStateStandby));
  AddTransition(new CETransition(kActionQuitScrubbing, kStateStandby, std::string(kActionQuitScrubbing_name), NULL, NULL, 1, kStateScrubbing));
}

int DevActel::ReSynchronizeLocal(){
  int iLastCommand = 0;
  int iStatusReg = 0;
  int iErrorReg = 0;
  uint32_t u32rawData = 0;
  int iCycle1 = 0;
  int iCycle2 = 0;
  if (!fpMsgbuffer) return kStateError;
  int current = GetCurrentState();
  if (current == kStateElse) return kStateStandby;

  iLastCommand = LastActiveCommand();
  iStatusReg = CheckStatusReg();
  iErrorReg = CheckErrorReg();
  CE_Debug("StatusReg: %#x, ErrorReg: %#x\n", iStatusReg, iErrorReg);
  SingleRead(ACTELReadbackCycleCounter, &u32rawData);
  iCycle1 = (int)(u32rawData & 0xffff);
  CE_Debug("iCycle1: %d\n", iCycle1);
  usleep(1000000);
  SingleRead(ACTELReadbackCycleCounter, &u32rawData);
  iCycle2 = (int)(u32rawData & 0xffff);
  CE_Debug("iCycle2: %d\n", iCycle2);

  if(iErrorReg!=0 || iStatusReg>=8) return kStateFailure;
  else if(iLastCommand == 5 && iCycle1 != iCycle2) return kStateReadback;
  else if(iLastCommand == 3 && iCycle1 != iCycle2) return kStateScrubbing;
  else if(current == kStateDownloading) return kStateDownloading;
  else return kStateStandby;
  
  return current;
}

//Entry function for the Statemachine
//Hardware situation should be found out here, and the proper state entered, Services registered
int DevActel::ArmorLocal(){
  int iResult = 0;
  std::string name=GetName();
  name+="_";
  fServices.push_back(new SerMbAddrS(name+"FWVERSION"      , ACTELFwVersion           , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"STATUS"         , ACTELStatusReg           , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"ERROR"          , ACTELErrorReg            , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"CYCLES"         , ACTELReadbackCycleCounter, 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"NOFSEU"         , ACTELReadbackErrorCount  , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"FRAMEERR"       , ACTELLastErrorFrameNumber, 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"LASTFRAME"      , ACTELLastFrameNumber     , 1, 0, fpMsgbuffer));
#ifdef ENABLE_RCUFLASH_FWVERSION
  fServices.push_back(new SerMbAddrS(name+"FLASH_FWVERSION", 0                        , 1, 0, fpMsgbuffer, 3));
#endif //ENABLE_RCUFLASH_FWVERSION
  return iResult;
}

int DevActel::LeaveStateIDLE(){
  int iResult = 0;
  int FwVersion = 0;
  
  //When we leave state off, we should
  //evaluate the hardware that we are dealing with

  FwVersion = EvaluateFirmwareVersion();
  CE_Debug("Actel FW Version: %#x\n", FwVersion);
  if(FwVersion == ACTELCurrentFwVersion){
    CE_Debug("Actel Device: Firmware Version: %#x\n", FwVersion);        
  }
  else{
    if(FwVersion < ACTELCurrentFwVersion){
      CE_Warning("Actel FPGA has old Firmware version %#x\n", FwVersion);
    }
    else{
      CE_Warning("Actel FPGA has unknown Firmware version %#x\n", FwVersion);
    }
  }
  return iResult;
}

int DevActel::LeaveStateRUNNING(){
  int iResult = 0;
  CE_Debug("DevActel::LeaveStateRUNNING called\n");    
  iResult = Abort();

  return iResult;
}

int DevActel::EnterStateSCRUBBING(){
  int iResult = 0;
  int iLastCommand = 0;
  uint32_t u32rawData = 0;
  int iCycle1 = 0;
  int iCycle2 = 0;
  CE_Debug("DevActel::EnterStateSCRUBBING called, entering Scrubbing state\n");    
  iLastCommand = LastActiveCommand();
  CheckStatusReg();
  CheckErrorReg();
  SingleRead(ACTELReadbackCycleCounter, &u32rawData);
  iCycle1 = (int)(u32rawData & 0xffff);
  CE_Debug("iCycle1: %d\n", iCycle1);
  usleep(1000000);
  SingleRead(ACTELReadbackCycleCounter, &u32rawData);
  iCycle2 = (int)(u32rawData & 0xffff);
  CE_Debug("iCycle2: %d\n", iCycle2);

  if(iLastCommand != 5 || iCycle1 == iCycle2){
  ClearErrorReg();
  ClearStatusReg();
  Abort();
  iResult = SingleWrite(ACTELReset, 0x0);  
  CE_Debug("DevActel::Starting Readback on Actel\n");    
  iResult = Scrub(0);
  }
  return iResult;
}

int DevActel::LeaveStateSCRUBBING(){
  int iResult = 0;
  for(int i=0;i<3;i++) iResult = Abort();
  return iResult;
}

int DevActel::EnterStateREADBACK(){
  int iResult = 0;
  int iLastCommand = 0;
  uint32_t u32rawData = 0;
  int iCycle1 = 0;
  int iCycle2 = 0;
  CE_Debug("DevActel::EnterStateREADBACK called, entering Readback state\n");
  iLastCommand = LastActiveCommand();
  CheckStatusReg();
  CheckErrorReg();
  SingleRead(ACTELReadbackCycleCounter, &u32rawData);
  iCycle1 = (int)(u32rawData & 0xffff);
  CE_Debug("iCycle1: %d\n", iCycle1);
  usleep(1000000);
  SingleRead(ACTELReadbackCycleCounter, &u32rawData);
  iCycle2 = (int)(u32rawData & 0xffff);
  CE_Debug("iCycle2: %d\n", iCycle2);

  if(iLastCommand != 5 || iCycle1 == iCycle2){
  ClearErrorReg();
  ClearStatusReg();
  Abort();
  iResult = SingleWrite(ACTELReset, 0x0);  
  CE_Debug("DevActel::Starting Readback on Actel\n");    
  iResult = Readback(0);
  }
  return iResult;
}

int DevActel::LeaveStateREADBACK(){
  int iResult = 0;
  for(int i=0;i<3;i++){
    iResult = Abort();
  }
  return iResult;
}

int DevActel::DevActelGO_STANDBY(CEStateMachine* callbackObject){
  CE_Debug("DevActelGO_STANDBY called\n");
  if(!callbackObject){
    CE_Error("No state machine\n");
    return -1;
  }
  int iResult = 0;
  iResult = (dynamic_cast<DevActel*>(callbackObject))->ClearErrorReg();
  iResult = (dynamic_cast<DevActel*>(callbackObject))->ClearStatusReg();
  (dynamic_cast<DevActel*>(callbackObject))->Abort();
  iResult = (dynamic_cast<DevActel*>(callbackObject))->SingleWrite(ACTELReset, 0x0);  
  return iResult;
}

int DevActel::issue(uint32_t cmd, uint32_t parameter, const char* pData, int iDataSize, std::vector<uint32_t>& rb){
  int iResult = 0;
  CE_Debug("DevActel::TranslateActelCommand cmd=%#x parameter=%#x datasize=%d\n", cmd, parameter, iDataSize);
  switch (cmd) {
  case RCU_WRITE_FRAMES:
    CE_Debug("Actel Device: received RCU_WRITE_FRAMES command\n");
    iResult = WriteFramesFlash(pData, (int)parameter);
    break;
  case RCU_WRITE_FLASH:
    CE_Debug("Actel Device: received RCU_WRITE_FLASH command\n");
    iResult = WriteFramesFlash(pData, (int)parameter);
    break;
  case ACTEL_CLEAR_ERRORREG:
    CE_Debug("Actel Device: received ACTEL_CLEAR_ERRORREG command\n");
    iResult = ClearErrorReg();
    break;
  case ACTEL_CLEAR_STATUSREGS:
    CE_Debug("Actel Device: received ACTEL_CLEAR_STATUSREG command\n");
    iResult = ClearStatusReg();
    break;
  case ACTEL_RESET:
    CE_Debug("Actel Device: received ACTEL_RESET command\n");
    iResult = SingleWrite(ACTELReset, 0x0);
    break;
  case RCU_INIT_CONF:
    CE_Debug("Actel Device: received RCU_INIT_CONF command\n");
    Abort();
    iResult = ClearErrorReg();
    iResult = ClearStatusReg();
    iResult = SingleWrite(ACTELReset, 0x0);  
    iResult = InitialConfiguration();
    break;
  case RCU_ERASE_FLASH:
    CE_Debug("Actel Device: received RCU_ERASE_FLASH command\n");
    RcuEraseFlash();
    break;
  case RCU_WRITE_INITCONF:
    CE_Debug("Actel Device: received RCU_WRITE_INITCONF command\n");
    iResult = WriteFlashBlock(pData, RCU_WRITE_INITCONF);    
    break;
  case RCU_WRITE_SCRUBBING:
    CE_Debug("Actel Device: received RCU_WRITE_SCRUBBING command\n");
    iResult = WriteFlashBlock(pData, RCU_WRITE_SCRUBBING);
    break;
  case RCU_READ_FPGA_CONF:
    /** to be implemented **/
    break; 
  case RCU_WRITE_FPGA_CONF:
    /** to be implemented **/
    break;
  case RCU_SET_AUTOCONF:
    switch((int)parameter){
    case 0x0:
      iResult = SetAutomaticConfiguration(0x0);
      break;
    case 0x1:
      iResult = SetAutomaticConfiguration(0xA);
      break;
    }
    break;
  case RCU_DELETE_XILINX:
    iResult = SetAutomaticConfiguration(0xA);
    iResult = SingleWrite(ACTELSmCommandReg, 0x1);
    break;
  case ACTEL_TEST_ERROR:
    CE_Debug("Actel testing error state\n");
    SetErrorState();
    break;
  default:
    CE_Debug("DevActel: unrecognized command id (%#x)\n", cmd);
    iResult=-ENOSYS;
  }  
  return iResult;
}

int DevActel::RcuEraseFlash(){
  int iResult = 0;
  int orgState=fState;
  ChangeState(kStateDownloading);
  //  usleep(1000000);
  iResult = FlashErase(-1, 0);
  if(iResult < 0){
    CE_Warning("DevActel:: Error erasing the Flash: %d", iResult);
  }
  ChangeState(orgState);
  return iResult;
}



/*****************************************************************
 ******         Selectmap commands                        ********
 ****************************************************************/

/** Do init procedure of the Xilinx **/
int DevActel::XilinxInit(){
  int iResult = 0;
  uint32_t u32rawData;
  u32rawData = 0x1;

  iResult = SingleWrite(ACTELSmCommandReg, u32rawData);
  if(iResult<0){
    CE_Debug("Actel Device: rcuSingleWrite failed\n");
    iResult =-EIO;
    return iResult;
  }  
  
  return iResult;
}

/** Do startup procedure of the Xilinx **/
int DevActel::XilinxStartup(){
  int iResult = 0;
  uint32_t u32rawData;
  u32rawData = 0x2;

  iResult = SingleWrite(ACTELSmCommandReg, u32rawData);
  if(iResult<0){
    CE_Debug("Actel Device: rcuSingleWrite failed\n");
    iResult =-EIO;
    return iResult;
  }  
  
  return iResult;
}

/** Issue abort sequence **/
int DevActel::XilinxAbort(){
  int iResult = 0;
  uint32_t u32rawData;
  u32rawData = 0x10;

  iResult = SingleWrite(ACTELSmCommandReg, u32rawData);
  if(iResult<0){
    CE_Debug("Actel Device: rcuSingleWrite failed\n");
    iResult =-EIO;
    return iResult;
  }  
  
  return iResult;
}

/*****************************************************************
 ******         helper functions                          ********
 ****************************************************************/

int DevActel::CheckErrorReg(){
  int iResult = 0;
  int iErrorReg = 0;
  uint32_t u32rawData;
  
  //And then we check the Error Register
  iResult = SingleRead(ACTELErrorReg, &u32rawData);
  if(iResult<0){
    CE_Debug("Actel Device: rcuSingleRead failed\n");
    iResult =-EIO;
    return iResult;
  }
  if((u32rawData & 0x0080) != 0){
    CE_Debug("Actel Device: Parity Error in Selectmap RAM detected: %#x\n", u32rawData);
    iErrorReg += 64;
  }
  if((u32rawData & 0x0040) != 0){
    CE_Debug("Actel Device: Parity Error in Flash RAM detected: %#x\n", u32rawData);
    iErrorReg += 128;
  }

  return iErrorReg;
}


//return the last active command from the Actel
int DevActel::LastActiveCommand(){
  int iResult = 0;
  int iLastCommand = 0;
  uint32_t u32rawData;
  
  iResult = SingleRead(ACTELStatusReg, &u32rawData);
  if(iResult<0){
    CE_Debug("Actel Device: rcuSingleRead failed\n");
    iResult =-EIO;
    return iResult;
  }
  switch(u32rawData & 0xF){
  case 0xA:     //Initial Configuration
    iLastCommand = 1;
    break;
  case 0xB:     //Scrubbing
    iLastCommand = 2;
    break;
  case 0xC:     //Scrubbing continously
    iLastCommand = 3;
    break;
  case 0xD:     //Readback and Verification
    iLastCommand = 4;
    break;
  case 0xE:     //Readback and Verification continously
    iLastCommand = 5;
    break;
  default://should not happen
    break;  
  }  
  return iLastCommand;
}

int DevActel::CheckStatusReg(){
  int iResult = 0;
  int iStatusReg = 0;
  uint32_t u32rawData = 0;

  iResult = SingleRead(ACTELStatusReg, &u32rawData);
  if(iResult<0){
    CE_Debug("Actel Device: rcuSingleRead failed\n");
    iResult =-EIO;
    return iResult;
}
  /**Null pointer in flash: Bit 7 **/
  if((u32rawData & 0x0080) != 0){
    iStatusReg += 8;
    CE_Debug("Actel Device: Flash contains no pointer value: %#x\n", u32rawData);
}
  /** Configuration Controller busy: Bit 6 **/
  if((u32rawData & 0x0040) != 0){
    iStatusReg += 4;
    CE_Debug("Actel Device: Configuration controller is busy with ongoing task: %#x\n", u32rawData);
  }
  /** Smap busy: Bit 4 **/
  if((u32rawData & 0x0010) != 0){
    iStatusReg += 1;
    CE_Debug("Actel Device: Selectmap Interface is busy: %#x\n", u32rawData);
  }
  return iStatusReg;
}

/**************************************************
 ***************************************************/
//call from evaluate hardware
//find out which version of the firmware we are dealing with
uint32_t DevActel::EvaluateFirmwareVersion(){
  int iResult = 0;
  uint32_t u32rawData = 0;
  iResult = SingleRead(ACTELFwVersion, &u32rawData);
  if(iResult<0){
    CE_Debug("Actel Device: rcuSingleRead failed\n");
    iResult =-EIO;
  }
  
  return 0x0000ffff&u32rawData;
}

//some smaller helper functions here
//**********************************

//Disable the Automatic Configuration
int DevActel::SetAutomaticConfiguration(uint32_t u32rawData){
  int iResult = 0;

  iResult = SingleWrite(ACTELStopPowerUpCodeReg, u32rawData);
  if(iResult<0){
    CE_Debug("Actel Device: rcuSingleWrite failed\n");
    iResult =-EIO;
    return iResult;
  }
  
  return iResult;
}


/**
* Check the Actel status register for a Null pointer in Flash,
* but this only the initial configuration part.
*
**/
int DevActel::CheckFlash(){
  int iResult = CheckStatusReg();
  //if there is a nullpointer in the flash
  if(iResult == -8 || iResult == -12 || iResult == -13){
    CE_Debug("Actel Device: Nullpointer in Flash memory! Flash memory not properly configured.");
    return -ENXIO;
  }
  return iResult;
}



/*****************************************************************
 ******      Actel firmware commands                      ********
 ****************************************************************/

int DevActel::ClearErrorReg(){
  int iResult = 0;
  uint32_t u32rawData;
  u32rawData = 0x1;

  iResult = SingleWrite(ACTELCommandReg, u32rawData);
  if(iResult<0){
    CE_Debug("Actel Device: rcuSingleWrite failed\n");
    iResult =-EIO;
    return iResult;
  }
  
  return iResult;
}

int DevActel::ClearStatusReg(){
  int iResult = 0;
  uint32_t u32rawData;
  u32rawData = 0x2;

  iResult = SingleWrite(ACTELCommandReg, u32rawData);
  if(iResult<0){
    CE_Debug("Actel Device: rcuSingleWrite failed\n");
    iResult =-EIO;
    return iResult;
  }
  
  return iResult;
}

//Do one initial Configuration of the Xilinx from the Flash
int DevActel::InitialConfiguration(){
  int iResult = 0;
  uint32_t u32rawData;
  u32rawData = 0x4; //initial configuration

  //ensure that the Flash is filled
  //  iResult = CheckFlash();
  
  if(iResult >= 0){
    iResult = SingleWrite(ACTELCommandReg, u32rawData);
    if(iResult<0){
      CE_Debug("Actel Device: rcuSingleWrite failed\n");
      return -EIO;
    }
  }
  else{
    CE_Debug("Actel Device: Flash memory not configured when trying initial configuration\n");
    return -EIO;
  }
  return iResult;
}

/**
 * Do scrubbing one time
 **/
int DevActel::Scrub(){
  int iResult = 0;
  uint32_t u32rawData;
  u32rawData = 0x10;

  iResult = ClearStatusReg();

  iResult = SingleWrite(ACTELCommandReg, u32rawData);
  if(iResult<0){
    CE_Debug("Actel Device: rcuSingleWrite failed\n");
    iResult =-EIO;
    return iResult;
  }
  
  return iResult;
}

/**
 * Scrub for more then one time, 0 for continously
 **/
int DevActel::Scrub(int times){
  int iResult = 0;
  uint32_t u32rawData;
  u32rawData = (uint32_t)times;

  //Clear the status register and counters
  iResult = ClearStatusReg();

  //write how many times scrubbing should be done to the data register
  iResult = SingleWrite(ACTELDataReg, u32rawData);
  if(iResult<0){
    CE_Debug("Actel Device: rcuSingleWrite failed\n");
    iResult =-EIO;
    return iResult;
  }  
  
  u32rawData = 0x20;  
  
  //execute scrubbing
  iResult = SingleWrite(ACTELCommandReg, u32rawData);
  if(iResult<0){
    CE_Debug("Actel Device: rcuSingleWrite failed\n");
    iResult =-EIO;
    return iResult;
  }
  
  return iResult;
}

/**
 * readback all frames one time
 **/
int DevActel::Readback(){
  int iResult = 0;
  uint32_t u32rawData;
  u32rawData = 0x100;

  iResult = ClearStatusReg();

  iResult = SingleWrite(ACTELCommandReg, u32rawData);
  if(iResult<0){
    CE_Debug("Actel Device: rcuSingleWrite failed\n");
    iResult =-EIO;
    return iResult;
  }

  return iResult;
}

/**
 * readback a given number of times, 0 for continously
 **/
int DevActel::Readback(int times){
  int iResult = 0;
  uint32_t u32rawData;
  u32rawData = (uint32_t)times;

  iResult = ClearStatusReg();
  iResult = SingleWrite(ACTELDataReg, u32rawData);
  if(iResult<0){
    CE_Debug("Actel Device: rcuSingleWrite failed\n");
    iResult =-EIO;
    return iResult;
  }
  u32rawData = 0x200;
  iResult = SingleWrite(ACTELCommandReg, u32rawData);
  if(iResult<0){
    CE_Debug("Actel Device: rcuSingleWrite failed\n");
    iResult =-EIO;
    return iResult;
  }
  return iResult;
}

/**
 * Abort any ongoing operation on the Actel
 **/
int DevActel::Abort(){
  int iResult = 0;
  uint32_t u32rawData;
  u32rawData = 0x8000;
 
 iResult = SingleWrite(ACTELCommandReg, u32rawData);
  if(iResult<0){
    CE_Debug("Actel Device: rcuSingleWrite failed\n");
    iResult =-EIO;
    return iResult;
  }

  return iResult;
}

/*********************************
 *** IssueHandler functions    ***
 ********************************/

int DevActel::GetGroupId(){
  return FEESVR_CMD_RCUCONF;
  
}

int DevActel::HighLevelHandler(const char* pCommand, std::vector<uint32_t>& rb)
{
  int iResult=0;
  uint32_t cmd=0;
  int keySize=0;
  int len=strlen(pCommand);
  const char* pBuffer=pCommand;
  if (pCommand) {
    //CE_Debug("DevActel checks command %s\n", pCommand);
    if (strncmp(pCommand, "RCU_INIT_CONF", keySize=strlen("RCU_INIT_CONF"))==0)
      cmd=RCU_INIT_CONF;
    if (strncmp(pCommand, "RCU_ERASE_FLASH", keySize=strlen("RCU_ERASE_FLASH"))==0)
      cmd=RCU_ERASE_FLASH;
    if (strncmp(pCommand, "ACTEL_CLEAR_ERRORREG", keySize=strlen("ACTEL_CLEAR_ERRORREG"))==0)
      cmd=ACTEL_CLEAR_ERRORREG;
    if (strncmp(pCommand, "ACTEL_CLEAR_STATUSREGS", keySize=strlen("ACTEL_CLEAR_STATUSREGS"))==0)
      cmd=ACTEL_CLEAR_STATUSREGS;
    if (strncmp(pCommand, "RCU_SET_AUTOCONF", keySize=strlen("RCU_SET_AUTOCONF"))==0)
      cmd=RCU_SET_AUTOCONF;
    if (strncmp(pCommand, "ACTEL_RESET", keySize=strlen("ACTEL_RESET"))==0)
      cmd=ACTEL_RESET;
    if (strncmp(pCommand, "RCU_DELETE_XILINX", keySize=strlen("RCU_DELETE_XILINX"))==0)
      cmd=RCU_DELETE_XILINX;
    if (strncmp(pCommand, "ACTEL_TEST_ERROR", keySize=strlen("ACTEL_TEST_ERROR"))==0)
      cmd=ACTEL_TEST_ERROR;
  
    if (cmd>0 && keySize>0) {
      pBuffer+=keySize;
      len-=keySize;
      while (*pBuffer==' ' && *pBuffer!=0 && len>0) {pBuffer++; len--;} // skip all blanks
      uint32_t data=0;
      const char* pPayload=NULL;
      int iPayloadSize=0;
      switch (cmd) {
      case RCU_INIT_CONF:
      case RCU_ERASE_FLASH:
      case ACTEL_CLEAR_ERRORREG:
      case ACTEL_CLEAR_STATUSREGS:
      case ACTEL_RESET:
      case RCU_DELETE_XILINX:
      case ACTEL_TEST_ERROR:
	break;
      default:
	if (sscanf(pBuffer, "%d", &data)>0) {
	  pPayload=(const char*)&data;
	  iPayloadSize=sizeof(data);  
	} else {
	  CE_Error("error scanning high-level command %s\n", pCommand);
	  iResult=-EPROTO;
	}
	break;
      }
      if (iResult>=0) {
	if ((iResult=issue(cmd, data, pPayload, iPayloadSize, rb))>=0) {
	  iResult=1;
	}
      }
    }
  } else {
    iResult=-EINVAL;
  }
  return iResult;  
}

int DevActel::SingleWrite(uint32_t address, uint32_t data)
{
  if (fpMsgbuffer)
    return fpMsgbuffer->SingleWrite(address, data);
  return -ENODEV;
}

int DevActel::SingleRead(uint32_t address, uint32_t* pData)
{
  if (fpMsgbuffer)
    return fpMsgbuffer->SingleRead(address, pData);
  return -ENODEV;
}

int DevActel::MultipleWrite(uint32_t address, uint32_t* pData, int iSize, int iDataSize)
{
  if (fpMsgbuffer)
    return fpMsgbuffer->MultipleWrite(address, pData, iSize, iDataSize);
  return -ENODEV;
}

int DevActel::MultipleRead(uint32_t address, int iSize,uint32_t* pData)
{
  if (fpMsgbuffer)
    return fpMsgbuffer->MultipleRead(address, iSize,pData);
  return -ENODEV;
}

int DevActel::FlashWrite(uint32_t address, uint32_t* pData, int iSize, int iDataSize){
  if (fpMsgbuffer) return fpMsgbuffer->MultipleWrite(address, pData, iSize, iDataSize, 3);
  return -ENODEV;
}


int DevActel::FlashRead(uint32_t address, int iSize,uint32_t* pData){
  if (fpMsgbuffer) return fpMsgbuffer->MultipleRead(address, iSize, pData, 3);
  return -ENODEV;
}


int DevActel::FlashErase(int startSec, int stopSec){
  if (fpMsgbuffer) return fpMsgbuffer->FlashErase(startSec, stopSec);

  return -ENODEV;
}

int DevActel::WriteFlashBlock(const char* pDataOrig, unsigned int mode){
  int iResult = 0;
  int offset = 0;
  uint32_t addr,
    info_addr,
    start_addr,
    stop_addr,
    rcu_fw_version,
    payload,
    nop_addr,
    data;
  uint32_t lower_addr,
    upper_addr;
  uint32_t *pData;  

  const char* pSrc = pDataOrig;
  
  int orgState=fState;
  ChangeState(kStateDownloading);

  addr = 0;
  info_addr = 0;
  start_addr = 0;
  stop_addr = 0;
  payload = 0;
  lower_addr = 0;
  upper_addr = 0;
  
  data=*((uint32_t*)pSrc);
  CE_Debug("Addr: %#x\n", data);
  addr = data;

  pSrc+=sizeof(uint32_t);
  data = *((uint32_t*)pSrc);
  CE_Debug("Info_Addr: %#x\n", data);
  info_addr = data;

  pSrc+=sizeof(uint32_t);
  data = *((uint32_t*)pSrc);
  CE_Debug("Start_Addr: %#x\n", data);
  start_addr = data;

  pSrc+=sizeof(uint32_t);
  data = *((uint32_t*)pSrc);
  CE_Debug("Payload: %#x\n", data);
  payload = data;
 
  /** calculating the stopaddress from playload and startaddress
   *  To work around a bug from Xilinx ISE, there need to be
   *  a couple of NOP commands added to the end of the config file,
   *  in order for the FPGA to initialize correctly. So we add these
   *  extra commands to the payload and write them after the configblock.
   **/

  stop_addr = CalculatkActionStopAddress((payload + 14), start_addr);
  nop_addr = CalculatkActionStopAddress(payload, start_addr);

   //setting up pointer to info space
  upper_addr = GetUpperAddress(info_addr);
  CE_Debug("Upper info_addr: %#x\n", upper_addr);
  pData = &upper_addr;
  iResult = FlashWrite(addr, pData, 1, 2);
  CE_Debug("iResult FlashWrite: %d\n", iResult);

  lower_addr = GetLowerAddress(info_addr);
  CE_Debug("Lower info_addr: %#x\n", lower_addr);
  pData = &lower_addr;
  iResult = FlashWrite(++addr, pData, 1, 2);
  CE_Debug("iResult FlashWrite: %d\n", iResult);

  //setting up start and stopaddresses in info space
  upper_addr = GetUpperAddress(start_addr);
  CE_Debug("Upper start_addr: %#x\n", upper_addr);
  pData = &upper_addr;
  iResult = FlashWrite(info_addr, pData, 1, 2);
  CE_Debug("iResult FlashWrite: %d\n", iResult);

  lower_addr = GetLowerAddress(start_addr);
  CE_Debug("Lower start_addr: %#x\n", lower_addr);
  pData = &lower_addr;
  iResult = FlashWrite(++info_addr, pData, 1, 2);
  CE_Debug("iResult FlashWrite: %d\n", iResult);

  upper_addr = GetUpperAddress(stop_addr);
  CE_Debug("Upper stop_addr: %#x\n", upper_addr);
  pData = &upper_addr;
  iResult = FlashWrite(++info_addr, pData, 1, 2);
  CE_Debug("iResult FlashWrite: %d\n", iResult);

  lower_addr = GetLowerAddress(stop_addr);
  CE_Debug("Lower stop_addr: %#x\n", lower_addr);
  pData = &lower_addr;
  iResult = FlashWrite(++info_addr, pData, 1, 2);
  CE_Debug("iResult FlashWrite: %d\n", iResult);

  if(mode == RCU_WRITE_INITCONF){
    //Read the Firmwareversion from the file
    pSrc+=sizeof(uint32_t);
    data = *((uint32_t*)pSrc);
    CE_Debug("RCU Firmwareversion: %#x\n", data);
    rcu_fw_version = data;

    upper_addr = GetUpperAddress(rcu_fw_version);
    CE_Debug("Upper RCU FW Version: %#x\n", upper_addr);
    pData = &upper_addr;
    iResult = FlashWrite(++info_addr, pData, 1, 2);
    CE_Debug("iResult FlashWrite: %d\n", iResult);

    lower_addr = GetLowerAddress(rcu_fw_version);
    CE_Debug("Lower RCU FW Version: %#x\n", lower_addr);
    pData = &lower_addr;
    iResult = FlashWrite(++info_addr, pData, 1, 2);
    CE_Debug("iResult FlashWrite: %d\n", iResult);
    
    //5*4 Bytes for pointers and FWV#
    offset = 20;
  }
  
  if(mode == RCU_WRITE_SCRUBBING){
    //4*4 Bytes for pointers
    offset = 16;
  }
  
  //write the data block
  pSrc+=sizeof(uint32_t);
  data = *((uint32_t*)pSrc);
  CE_Debug("4. byte: %#x\n", data);

  CE_Debug("Payload: %d, Payload/2: %d\n", payload, payload/2);

  iResult = FlashWrite(start_addr, (uint32_t*)(pSrc), payload/2, 2); 
  CE_Debug("FlashWriteblock: %d\n", iResult);

  uint32_t nop = 0x2000;
  uint32_t *pnop = &nop;
  uint32_t blank = 0x0;
  uint32_t *pblank = &blank;
  uint32_t address = nop_addr+1;
  for(uint32_t i = 0; i<7; i++){
    CE_Debug("NOP_addr: %#x, pdata: %#x\n",address+i, nop);
    iResult = FlashWrite(address+i, pnop ,1 ,2);
    i++;
    CE_Debug("NOP_addr: %#x, pdata: %#x\n",address+i, blank);
    iResult = FlashWrite(address+i, pblank ,1 ,2);
  }
  
  ChangeState(orgState);
  //Return the payload of the raw data + the space for addresses, + the NOP commands
  return payload + offset;
}


//Write the Frame data to the Flash memory
int DevActel:: WriteFramesFlash(const char* pDataBlock, int NofFrames){
  int iResult = 0;

  //use the parameter for the number of frames transmitted
  uint32_t addr,
    info_addr,
    start_addr,
    stop_addr,
    init_addr,
    frame_offset,
    read_frame_size,
    write_frame_size,
    infoline1,
    infoline2,
    infoline3,
    data;
  uint32_t lower_addr,
    upper_addr;
  uint32_t *pData;  

  int orgState=fState;
  ChangeState(kStateDownloading);

  const char* pSrc=pDataBlock;

  addr = 0;
  info_addr = 0;
  start_addr = 0;
  stop_addr = 0;
  lower_addr = 0;
  upper_addr = 0;
  frame_offset = 0;
  init_addr = 0;
  read_frame_size = 0;
  write_frame_size = 0;
  infoline1 = 0;
  infoline2 = 0;
  infoline3 = 0;

  if(NofFrames == 0){
    NofFrames = 936;
  }
  CE_Debug("Number of Frames transmitted as paramter: %d", NofFrames);

  //The frame offset
  data = *((uint32_t*)pSrc);
  CE_Debug("Frame_offset: %#x\n", data);
  frame_offset = data;
  
  pSrc+=sizeof(uint32_t);
  data = *((uint32_t*)pSrc);
  CE_Debug("Addr: %#x\n", data);
  addr = data;

  pSrc+=sizeof(uint32_t);
  data = *((uint32_t*)pSrc);
  CE_Debug("Info_Addr: %#x\n", data);
  info_addr = data;

  pSrc+=sizeof(uint32_t);
  data = *((uint32_t*)pSrc);
  CE_Debug("Start_Addr: %#x\n", data);
  start_addr = init_addr = data;

  pSrc+=sizeof(uint32_t);
  data = *((uint32_t*)pSrc);
  CE_Debug("Stop_addr: %#x\n", data);
  stop_addr = data;

  pSrc+=sizeof(uint32_t);
  data = *((uint32_t*)pSrc);
  CE_Debug("Info line 1: %#x\n", data);
  infoline1 = data;

  pSrc+=sizeof(uint32_t);
  data = *((uint32_t*)pSrc);
  CE_Debug("Info line 2: %#x\n", data);
  infoline2 = data;

  pSrc+=sizeof(uint32_t);
  data = *((uint32_t*)pSrc);
  CE_Debug("Info line 3: %#x\n", data);
  infoline3 = data;
    
  //setting up pointer to info space
  upper_addr = GetUpperAddress(info_addr);
  CE_Debug("Upper info_addr: %#x\n", upper_addr);
  pData = &upper_addr;
  iResult = FlashWrite(addr, pData, 1, 2);
  CE_Debug("iResult FlashWrite: %d", iResult);

  lower_addr = GetLowerAddress(info_addr);
  CE_Debug("Lower info_addr: %#x\n", lower_addr);
  pData = &lower_addr;
  iResult = FlashWrite(++addr, pData, 1, 2);

  //setting up startaddress in info space
  upper_addr = GetUpperAddress(start_addr);
  CE_Debug("Upper start_addr: %#x\n", upper_addr);
  pData = &upper_addr;
  iResult = FlashWrite(info_addr, pData, 1, 2);

  lower_addr = GetLowerAddress(start_addr);
  CE_Debug("Lower start_addr: %#x\n", lower_addr);
  pData = &lower_addr;
  iResult = FlashWrite(++info_addr, pData, 1, 2);

  //setting up stopaddress in info space
  upper_addr = GetUpperAddress(stop_addr);
  CE_Debug("Upper stop_addr: %#x\n", upper_addr);
  pData = &upper_addr;
  iResult = FlashWrite(++info_addr, pData, 1, 2);

  lower_addr = GetLowerAddress(stop_addr);
  CE_Debug("Lower stop_addr: %#x\n", lower_addr);
  pData = &lower_addr;
  iResult = FlashWrite(++info_addr, pData, 1, 2);

  //write the 3 frame info registers
  //pSrc+=sizeof(uint32_t);
  //data = *((uint32_t*)pSrc);
  pData = &infoline1;
  CE_Debug("Frame_Info_addr1: %#x\n", *pData);
  iResult = FlashWrite(++info_addr, pData, 1, 2);

  //  pSrc+=sizeof(uint32_t);
  //data = *((uint32_t*)pSrc);
  pData = &infoline2;
  CE_Debug("Frame_Info_addr2: %#x\n", *pData);
  iResult = FlashWrite(++info_addr, pData, 1, 2);

  //pSrc+=sizeof(uint32_t);
  //data = *((uint32_t*)pSrc);
  pData = &infoline3;
  CE_Debug("Frame_Info_addr3: %#x\n", *pData);
  iResult = FlashWrite(++info_addr, pData, 1, 2);

  //Size of Readframes
  pSrc+=sizeof(uint32_t);
  data = *((uint32_t*)pSrc);
  CE_Debug("Readframesize: %d\n", data);
  read_frame_size = data;

  //Size of Writeframes
  pSrc+=sizeof(uint32_t);
  data = *((uint32_t*)pSrc);
  CE_Debug("Writeframesize: %d\n", data);
  write_frame_size = data;

  //write the read and writeframes
  //for number of readframes write the readframes with readframesize

  pSrc += sizeof(uint32_t);
  data = *((uint32_t*)pSrc);

  CE_Debug("1st readframe starts: %#x, start_addr:%#x\n", data, start_addr);

  
  //write the readframes to the Flash
  for (int i = 0; i < NofFrames; i++){

    FlashWrite(start_addr, (uint32_t*)pSrc, read_frame_size/2, 2);   

    start_addr += frame_offset;
    pSrc += read_frame_size;
  }

  //for number of frames write the writeframes with writeframesize  
  start_addr = init_addr + 0x100000;


  data = *((uint32_t*)pSrc);
  //Write the Writeframes to the Flash
  for (int i = 0; i < NofFrames; i++){

    data = *((uint32_t*)pSrc);
    CE_Debug("readframe %d: %#x, addr:%#x, pointer: %p\n", i, data, start_addr, pSrc);

    FlashWrite(start_addr, (uint32_t*)(pSrc), write_frame_size/2, 2);   

    start_addr += frame_offset;
    pSrc += write_frame_size;
  }

  ChangeState(orgState);
  //return the size of the datablock without header and tailer
  return ( NofFrames * (read_frame_size + write_frame_size) + (4 * 10));  
}

/*************************************************
 ***** Helper functions to program the flash *****
 ************************************************/

//returns the upper 16 bit of a 32 bit word
uint16_t DevActel:: GetUpperAddress(uint32_t u32address){
   uint16_t u16address;
   u16address = u32address >> 16;

return u16address;
}


//returns the lower 16 bit of a 32 bit word
uint16_t DevActel:: GetLowerAddress(uint32_t u32address){
   uint16_t u16address;
   u16address = u32address;

return u16address;
}


int DevActel:: CalculatkActionStopAddress(uint32_t filesize, uint32_t startaddress){
   int iResult;
   uint32_t stopaddress, u32rawdata;

   stopaddress = 0;
   iResult = 0;

   stopaddress = (startaddress + (filesize / 2) - 1);

   // check if we have the 1.2 Actel firmware,
   // which requires special treatment
   iResult = SingleRead(ACTELFwVersion,&u32rawdata);
   if(iResult < 0){
      CE_Debug("Actel Device: Could not read Actel FW Version register: %d", iResult);
   }
   
   //correct the FW Error
   if(u32rawdata == (0xffff0000 | ACTELFwVersion102))
        stopaddress += 100;
   
return stopaddress;
}

