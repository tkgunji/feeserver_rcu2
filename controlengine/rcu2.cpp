#include <cerrno>
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <typeinfo>
#include "rcu2.hpp"
#include "rcu2_issue.h"
#include "ser.hpp"
#include "dcscMsgBufferInterface.h"

/******************************************************************************/

CtrRcu2::CtrRcu2():fpRcu2(NULL){
  AddTransition(new CETransition(kActionGoStandby, kStateStandby, std::string(kActionGoStandby_name), NULL, NULL, 5, kStateError, kStateFailure, kStateRunning, kStateDownloading, kStateIdle));
  AddTransition(new CETransition(kActionGoIdle, kStateIdle, std::string(kActionGoIdle_name), CtrRcu2GO_IDLE, this, 5, kStateError, kStateFailure, kStateRunning, kStateDownloading, kStateStandby));
  AddTransition(new CETransition(kActionConf, kStateDownloading, std::string(kActionConf_name), NULL, NULL, 3, kStateStandby, kStateIdle, kStateRunning));
  AddTransition(new CETransition(kActionConfigureDone, kStateRunning, std::string(kActionConfigureDone_name), NULL, NULL, 1, kStateDownloading));
  AddTransition(new CETransition(kActionStart, kStateConfigured, std::string(kActionStart_name), NULL, NULL, 1, kStateRunning));
  AddTransition(new CETransition(kActionStop, kStateRunning, std::string(kActionStop_name), NULL, NULL, 1, kStateConfigured));
  AddTransition(new CETransition(kActionStartDaqConf, kStateConfDdl, std::string(kActionStartDaqConf_name), NULL, NULL, 4, kStateDownloading, kStateStandby, kStateIdle, kStateRunning));
  AddTransition(new CETransition(kActionEndDaqConf, kStateRunning, std::string(kActionEndDaqConf_name), NULL, NULL, 1, kStateConfDdl));

  if     (!fpMsgbuffer && fHwType==1) fpMsgbuffer=new DevMsgbufferRcu2();
  else if(!fpMsgbuffer && fHwType==2) fpMsgbuffer=new DevMsgbufferDummy();
  if(fpMsgbuffer) CE_Event("DevMsgbuffer of type '%s' created at %p with HwType %i\n", typeid(*fpMsgbuffer).name(), fpMsgbuffer, fHwType);
  else CE_Warning("DevMsgbuffer not created at %p with HwType %i\n", fpMsgbuffer, fHwType);

  if(!fpRcu2 && fpMsgbuffer) fpRcu2=new DevRcu2(subDevices.size(), this, fpMsgbuffer);
  if(fpRcu2){
    AddSubDevice(fpRcu2);
    CE_Event("DevRcu2 '%s' (%i) of type '%s' created at %p with DevMsgbuffer at %p\n", fpRcu2->GetName().c_str(), fpRcu2->GetDeviceId(), typeid(*fpRcu2).name(), fpRcu2, fpMsgbuffer);
  }
  else CE_Warning("DevRcu2 not created at %p with DevMsgbuffer %p\n", fpRcu2, fpMsgbuffer);

  if     (!fpFecaccess && fpMsgbuffer && fHwType==1) fpFecaccess=new DevFecaccessRcu2 (fpMsgbuffer);
  else if(!fpFecaccess && fpMsgbuffer && fHwType==2) fpFecaccess=new DevFecaccessDummy(fpMsgbuffer);
  if(fpFecaccess) CE_Event("DevFecaccess of type '%s' created at %p with DevMsgbuffer at %p and HwType %i\n", typeid(*fpFecaccess).name(), fpFecaccess, fpMsgbuffer, fHwType);
  else CE_Warning("DevFecaccess not created at %p with DevMsgbuffer at %p and fHwType=%i\n", fpFecaccess, fpMsgbuffer, fHwType);
}

CtrRcu2::~CtrRcu2(){
  fpMsgbuffer=NULL; // deleted in sub-device handling
  fpRcu2=NULL;
  fpFecaccess=NULL;
  for(uint32_t i=0;i<32;i++){
    fpFecs[i]=NULL;
  }
}

int CtrRcu2::CtrRcu2GO_IDLE(CEStateMachine* callbackObject){
  CE_Debug("CtrRcuGO_IDLE called\n");
  if(!callbackObject){
    CE_Error("No state machine\n");
    return -1;
  }
  (dynamic_cast<CtrRcu2*>(callbackObject))->fpRcu2->SetAfl(0);
  return 0;
}

/******************************************************************************/

std::string DevRcu2::imemFileName;
std::string DevRcu2::regCfgFileName;

DevRcu2::DevRcu2(int id,  CEStateMachine* pParent, DevMsgbuffer* msgbuffer): Dev("RCU2", id, pParent),fpMsgbuffer(msgbuffer){
  AddTransition(new CETransition(kActionGoStandby, kStateStandby, std::string(kActionGoStandby_name), NULL, NULL, 2, kStateError, kStateFailure));

  imemFileName=Ctr::tmpDir+"/FEESERVER_IMEM";
  regCfgFileName=Ctr::tmpDir+"/FEESERVER_REG_CFG";
}


int DevRcu2::ReSynchronizeLocal(){
  //Used to determine state of device, more complex tests can be implemented
  //return kStateStandby;
  if (!fpMsgbuffer) return kStateError;
  int current = GetCurrentState();
  if (current == kStateElse) return kStateStandby;
  //routine to check the status of the global registers
  return current;
}

int DevRcu2::GetGroupId(){
  //Defining the command group of the RCU2.
  return FEESVR_CMD_RCU2;
}

int DevRcu2::HighLevelHandler(const char* pCommand, std::vector<uint32_t>& rb){
  //Handling of high-level commands (text strings)
  //High-level commands are typically converted into coresponding low-level (binary) commands and passed to the issue function
  if(!pCommand){
    CE_Error("Hight level fee command buffer does not exist\n");
    return -1;
  }
  uint32_t cmd=0;
  size_t keySize=0;
  std::string command=pCommand;
  std::string test="";
  if(command.find(test="EXAMPLE_COMMAND_1",0)==0){ keySize=test.size(); cmd=EXAMPLE_COMMAND_1; }
  else if(command.find(test="EXAMPLE_COMMAND_2",0)==0){ keySize=test.size(); cmd=EXAMPLE_COMMAND_2; }
  else if(command.find(test="RCU2_READ_STATUS_REG",0)==0){ keySize=test.size(); cmd=RCU2_READ_STATUS_REG; }
  else if(command.find(test="RCU2_WRITE_TO_REG",0)==0){ keySize=test.size(); cmd=RCU2_WRITE_TO_REG; }

  /// following commands are added from rcu
  else if(command.find(test="RCU2_WRITE_AFL", 0)==0){keySize=test.size(); cmd=RCU2_WRITE_AFL;}
  else if(command.find(test="RCU2_READ_AFL",0)==0){keySize=test.size(); cmd=RCU2_READ_AFL;}
  else if(command.find(test="RCU2_WRITE_MEMORY",0)==0){keySize=test.size(); cmd=RCU2_WRITE_TO_REG;}
  else if(command.find(test="RCU2_READ_FW_VERSION",0)==0){keySize=test.size();cmd=RCU2_READ_FW_VERSION;}
  else if(command.find(test="RCU2_RESET",0)==0){keySize=test.size();cmd=RCU2_RESET;}
  else if(command.find(test="RCU2_SYNC_SCLK_L1TTC",0)==0){keySize=test.size(); cmd=RCU2_SYNC_SCLK_L1TTC;}
  else if(command.find(test="RCU2_ALTRO_INSTRUCTION_EXECUTE",0)==0){keySize=test.size(); cmd=RCU2_ALTRO_INSTRUCTION_EXECUTE;}
  else if(command.find(test="RCU2_ALTRO_INSTRUCTION_CLEAR",0)==0){keySize=test.size(); cmd=RCU2_ALTRO_INSTRUCTION_CLEAR;}
  else if(command.find(test="RCU2_ALTRO_INSTRUCTION_WRITE_FILE",0)==0){keySize=test.size(); cmd=RCU2_ALTRO_INSTRUCTION_WRITE_FILE;}
  else if(command.find(test="RCU2_ALTRO_INSTRUCTION_READ_FILE",0)==0){keySize=test.size(); cmd=RCU2_ALTRO_INSTRUCTION_READ_FILE;}
  else if(command.find(test="RCU2_CONFGFEC",0)==0){keySize=test.size(); cmd=RCU2_CONFGFEC;}
  else if(command.find(test="RCU2_WRITE_EN_INT_BA",0)==0){keySize=test.size(); cmd=RCU2_WRITE_EN_INT_BA;}
  else if(command.find(test="RCU2_READ_EN_INT_BA",0)==0){keySize=test.size(); cmd=RCU2_READ_EN_INT_BA;}
  else if(command.find(test="REG_CFG_EXECUTE",0)==0){keySize=test.size(); cmd=REG_CFG_EXECUTE;}
  else if(command.find(test="REG_CFG_EXECUTE_SINGLE",0)==0){keySize=test.size(); cmd=REG_CFG_EXECUTE_SINGLE;}
  else if(command.find(test="REG_CFG_CLEAR",0)==0){keySize=test.size(); cmd=REG_CFG_CLEAR;}
  else if(command.find(test="REG_CFG_WRITE_FILE",0)==0){keySize=test.size(); cmd=REG_CFG_WRITE_FILE;}
  else if(command.find(test="REG_CFG_READ_FILE",0)==0){keySize=test.size();  cmd=REG_CFG_READ_FILE;}
  else if(command.find(test="RCU2_DRIVER_RELOAD",0)==0){keySize=test.size(); cmd=RCU2_DRIVER_RELOAD;}
  else if(command.find(test="RCU2_DRIVER_UNLOAD",0)==0){keySize=test.size(); cmd=RCU2_DRIVER_UNLOAD;}
  else if(command.find(test="RCU2_DRIVER_LOAD",0)==0){keySize=test.size(); cmd=RCU2_DRIVER_LOAD;}
  else if(command.find(test="RCU2_TEST_ERROR",0)==0){keySize=test.size(); cmd=RCU2_TEST_ERROR;}
  else return 0;
  
  std::string parameter="";
  if(keySize<command.size()){
    if(command[keySize]!=' '){
      CE_Warning("High level fee command '%s' not found in parameter group %i\n", command.c_str(), GetGroupId());
      return -2;
    }
    parameter=command.substr(keySize+1,command.size());
  }
  if(int iResult=issue(cmd, atoi(parameter.c_str()), parameter.c_str(), parameter.size(), rb)<0){
    CE_Warning("High level fee command '%s' issued binary command %p with parameter %p, failed with error %i\n", command.c_str(), cmd, atoi(parameter.c_str()), iResult);
    return -3;
  }
  return 1;
}

int DevRcu2::SetAfl(uint32_t value){
  CE_Debug("Seting AFL to value %x\n", value);
  int nRet=0;
  uint32_t afl=0x0;
  nRet=fpMsgbuffer->SingleRead(RCU2_FECActiveList, &afl);
  if (nRet<0){
    CE_Warning("can not read RCU AFL %#x: SingleRead failed (%d)\n", 0x8000, nRet);
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
      nRet=fpMsgbuffer->SingleWrite(0x8000, afl);
      if (nRet<0){
        CE_Error("SingleWrite failed to write %p to RCU AFL with error %i\n", 0x8000, nRet);
        return -2;
      }
      else CE_Debug("Set AFL to %#x\n", afl);
      usleep(300000);
    }
  }
  pParent->ChangeState(orgState);
  return 1;
}

///some internal routine to access RadMon/ADC through SPI/I2C 

int DevRcu2::Rcu2AdcConf(uint32_t adc){
   return 1;
}
int DevRcu2::Rcu2RadMonConf(uint32_t radmon){
   return 1;
}

int DevRcu2::Rcu2AdcRead(Ser::TceServiceData* pData, int32_t adc, int32_t unused, void* parameter){
  if(!parameter) return -1;
  DevRcu2* rcu=(DevRcu2*) parameter;
  int nRet=0;
  uint32_t rawValue=0;
  // nRet|=rcu->fpMsgbuffer->SingleWrite(0x801a,adc+0xff000000);
  //  nRet|=rcu->fpMsgbuffer->SingleRead(0x800d,&rawValue);
  rawValue=rawValue>>6;
  //factor=0.00390625=1/256=0.015625*0.25=1/64*1/4    
  //rawValue&=0xffc0;                                                                                          
  if(nRet>=0) pData->fVal=rawValue*0.25;
  else pData->fVal=nRet;
  return nRet;
}

int DevRcu2::Rcu2RadMonRead(Ser::TceServiceData* pData, int32_t adc, int32_t unused, void* parameter){
  if(!parameter) return -1;
  DevRcu2* rcu=(DevRcu2*) parameter;
  int nRet=0;
  uint32_t rawValue=0;
  //nRet|=rcu->fpMsgbuffer->SingleWrite(0x801a,adc+0xff000000);
  //nRet|=rcu->fpMsgbuffer->SingleRead(0x800d,&rawValue);
  rawValue=rawValue>>6;
  //factor=0.00390625=1/256=0.015625*0.25=1/64*1/4                     
  //rawValue&=0xffc0;                                                                                 
  if(nRet>=0) pData->fVal=rawValue*0.25;
  else pData->fVal=nRet;
  return nRet;
}

int DevRcu2::ArmorLocal(){
  //Will be executed at start of FeeServer

  std::string name=GetName();
  //these are the test purposes
  fServices.push_back(new SerMbAddrS(name+"_CTRLREG", 0x40000, 1, 0, fpMsgbuffer));  
  fServices.push_back(new SerMbAddrS(name+"_L1_LATENCY_REG", 0x40060, 1, 0, fpMsgbuffer));  
  fServices.push_back(new SerMbAddrS(name+"_L2_LATENCY_REG", 0x40070, 1, 0, fpMsgbuffer));  
  fServices.push_back(new SerMbAddrS(name+"_VERSION", 0x51060, 1, 0, fpMsgbuffer));  
  //list of services inherited from rcu  
  //  fServices.push_back(new SerMbAddrS(name+"_AFL"          , V2_FECActiveList_RO  , 1, 0, fpMsgbuffer));
  ////////////////////////////
  // following two addresses are not in the current rcu2 register map or address is not indentical !!
  // to be confirmed 
  //fServices.push_back(new SerMbAddrS(name+"_FECERR_A"     , V2_FECErrA_RO     , 1, 0, fpMsgbuffer));
  //fServices.push_back(new SerMbAddrS(name+"_FECERR_B"     , V2_FECErrB_RO     , 1, 0, fpMsgbuffer));
  //fServices.push_back(new SerMbAddrS(name+"_FWVERSION"    , RCUFwVersion      , 1, 0, fpMsgbuffer));
  //fServices.push_back(new SerMbAddrS(name+"_DCS_FWVERSION", DCSFwVersion      , 1, 0, fpMsgbuffer));
  //fServices.push_back(new SerMbAddrS(name+"_DCS_OLDMODE"  , DCSOldMode        , 1, 0, fpMsgbuffer));
  //fServices.push_back(new SerMbAddrS(name+"_SLC_STATUS"   , FECErrReg         , 1, 0, fpMsgbuffer));
  ////////////////////////////

  //std::vector<uint32_t> temp;
  /// this is necessary???? to be confirmed...
  //issue(REG_CFG_READ_FILE, 0, 0, 0, temp);
  //issue(RCU_ALTRO_INSTRUCTION_READ_FILE, 0, 0, 0, temp);
  //Rcu2AdcConf(0);
  //Rcu2AdcConf(4);
  //name=GetName()+"_TEMP1";
  //Ser::RegisterService(Ser::eDataTypeFloat, name.c_str(), 0.5, DevRcu2::Rcu2AdcRead, 0, 0, 0, this);
  //name=GetName()+"_TEMP2";
  //Ser::RegisterService(Ser::eDataTypeFloat, name.c_str(), 0.5, DevRcu2::Rcu2AdcRead, 0, 4, 0, this);

  //Rcu2RadMonConf(0);
  //Rcu2RadMonConf(4);
  //name=GetName()+"_RADMON1";
  //Ser::RegisterService(Ser::eDataTypeFloat, name.c_str(), 0.5, DevRcu2::Rcu2AdcRead, 0, 0, 0, this);
  //name=GetName()+"_RADMON2";
  //Ser::RegisterService(Ser::eDataTypeFloat, name.c_str(), 0.5, DevRcu2::Rcu2AdcRead, 0, 4, 0, this);

  return 0;
}


int DevRcu2::issue(uint32_t cmd, uint32_t parameter, const char* pData, int iDataSize, std::vector<uint32_t>& rb){
  //Handling of low-level commands
  //The cmd paramter is defined in rcu_issue.h
  int iResult=0;
  CE_Debug("Rcu2::TranslateRcuCommand cmd=%#x parameter=%#x datasize=%d\n", cmd, parameter, iDataSize);
  uint32_t data;
  const char* pBuffer = pData;
  int address=0x0;

  switch(cmd){
  case EXAMPLE_COMMAND_1:
    //Return number of bytes of the command buffer (pData) consumed by the command
    iResult=0;
    break;
  case EXAMPLE_COMMAND_2:
    iResult=0;
    break;
  case RCU2_READ_STATUS_REG:
    CE_Debug("Command::RCU2_READ_STATUS_REG\n");
    ////////////////////////////////////////////////
    //////// parameter.c_str() = pData = [addr] [data] /////
    if(sscanf(pBuffer, "0x%x", &address)>0){
      fpMsgbuffer->SingleRead(address, &data); 
      CE_Event("DevRcu2 read data =  0x%x (address = 0x%x)\n", data, address);
      iResult=0;
    }else{
      CE_Warning("DevRcu2 read data format is wrong!!\n");
      iResult=0;
    }
    break;
  case RCU2_WRITE_TO_REG:
    {
      CE_Debug("Command::RCU2_WRITE_TO_REG\n");
      ////////////////////////////////////////////////
      //////// parameter.c_str() = pData = [addr] [data] /////
      int write_data = 0x0;
      int write_address = 0x0;
      const char* pBlank=NULL;
      if(sscanf(pBuffer, "0x%x", &write_address)>0 && (pBlank=strchr(pBuffer, ' '))!=NULL && 
	 sscanf(pBlank, " 0x%x", &write_data)>0){
	fpMsgbuffer->SingleWrite(write_address, write_data);
	CE_Debug("DevRcu2 write data =  0x%x (address = 0x%x)\n", write_data, write_address);
	iResult=0;
      }else{
	CE_Warning("DevRcu2 write data format is wrong!!\n");
	iResult=0;
      }
    }
    break;
  case RCU2_WRITE_AFL:
    SetAfl(*(uint32_t*)pData);
    iResult=4;
    break;
  case RCU2_READ_AFL:
    {
      uint32_t rbsize=rb.size();
      rb.resize(rbsize+parameter);
      fpMsgbuffer->MultipleRead(V2_ALTROACL, parameter, &rb[rbsize]);
      iResult=0;
    }
    break;
  case RCU2_READ_FW_VERSION:
    {
      uint32_t rbsize=rb.size();
      rb.resize(rbsize+1);
      fpMsgbuffer->SingleRead(RCUFwVersion, &rb[rbsize]);
      iResult=0;
    }
    break;
  case RCU2_RESET:
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
  case RCU2_SYNC_SCLK_L1TTC:
    fpMsgbuffer->SingleWrite(V2_CMDSCLKsync,0);
    iResult=0;
    break;
  case RCU2_ALTRO_INSTRUCTION_EXECUTE:
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
	    if(i>=4 && errorA!=0) {someError=1; CE_Error("Execution of instruction memory failed; FEC A error register: %#x (stickyErrorRepeat=%#x)\n", errorA, stickyErrorRepeat);}
	    if(i>=4 && errorB!=0) {someError=1; CE_Error("Execution of instruction memory failed; FEC B error register: %#x (stickyErrorRepeat=%#x)\n", errorB, stickyErrorRepeat);}
	    if(i>=4 && (errorBusy&0x2)!=0){someError=1; CE_Error("Execution of instruction memory failed; BusBusy register: %#x (stickyErrorRepeat=%#x)\n", errorBusy, stickyErrorRepeat);}
	    index=0;
	  }
	}
	if(imem){delete[] imem; imem=0;}
	if(rmem){delete[] rmem; rmem=0;}
	uint32_t errorA=0;
	uint32_t errorB=0;
	fpMsgbuffer->SingleRead(V2_FECErrA_RO,&errorA); //this address needs to be checked 
	fpMsgbuffer->SingleRead(V2_FECErrB_RO,&errorB); //this address needs to be checked
	if((errorA&0x1) || (errorB&0x1)){
	  stickyErrorRepeat--;
	  if(!stickyErrorRepeat) {someError=1; CE_Error("Execution of instruction memory failed; unrecoverable sticky bit found, FEC A error re\
gister: %#x, FEC B error register: %#x\n", errorA, errorB);}
	  else {someError=0; fpMsgbuffer->SingleWrite(V2_CMDRESET,0);}
	}
	else stickyErrorRepeat=0;
	if(someError) {SetErrorState(); CE_Error("Entering error state\n");}
      }
      while(stickyErrorRepeat>0);
      iResult=0;
    }
    break;
  case RCU2_ALTRO_INSTRUCTION_CLEAR:
    fAltroInstr.clear();
    iResult=0;
    break;
  case RCU2_ALTRO_INSTRUCTION_WRITE_FILE:
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
  case RCU2_ALTRO_INSTRUCTION_READ_FILE:
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
  case RCU2_CONFGFEC:
    fpMsgbuffer->SingleWrite(CONFGFEC,1);
    sleep(1);
    iResult=0;
    break;
  case RCU2_WRITE_EN_INT_BA: //this address needs to be checked!
    fpMsgbuffer->SingleWrite(FECINTmode, *(uint32_t*)pData);
    iResult=4;
    break;
  case RCU2_READ_EN_INT_BA: //this address needs to be checked! 
    {
      uint32_t rbsize=rb.size();
      rb.resize(rbsize+1);
      fpMsgbuffer->SingleRead(FECINTmode, &rb[rbsize]);
      iResult=0;
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
  case RCU2_DRIVER_RELOAD:
    if(fpMsgbuffer->DriverReload()<0){
      CE_Error("RCU driver reload failed with error\n");
    }
    iResult=0;
    break;
  case RCU2_DRIVER_UNLOAD:
    if(fpMsgbuffer->DriverUnload()<0){
      CE_Error("RCU driver unload failed with error\n");
    }
    iResult=0;
    break;
  case RCU2_DRIVER_LOAD:
    if(fpMsgbuffer->DriverLoad()<0){
      CE_Error("RCU driver load failed with error\n");
    }
    iResult=0;
    break;
  case RCU2_TEST_ERROR:
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

DevFecaccessRcu2::DevFecaccessRcu2(DevMsgbuffer* msgbuffer): DevFecaccess(msgbuffer){
}
int DevFecaccessRcu2::AccessFecReg(uint32_t &data, uint32_t fec, uint32_t reg, uint32_t rnw){
  //To be implemented...
  return -2160;
}

/******************************************************************************/

DevMsgbufferRcu2::DevMsgbufferRcu2(){
  DriverLoad();
}
DevMsgbufferRcu2::~DevMsgbufferRcu2(){
  DriverUnload();
}
int DevMsgbufferRcu2::DriverReload(){
  DriverUnload();
  DriverLoad();
  return 1;
}
int DevMsgbufferRcu2::DriverUnload(){
  if(releaseRcuAccess()<0){
    CE_Error("initRcuAccess finished with error code\n");
  }
  return 1;
}
int DevMsgbufferRcu2::DriverLoad(){
  if(initRcuAccess(NULL)<0){
    CE_Error("initRcuAccess finished with error code\n");
  }
  return 1;
}
int DevMsgbufferRcu2::SingleWrite(uint32_t address, uint32_t data, uint32_t mode){
 return Rcu2SingleWrite(address, 1, data);
}
int DevMsgbufferRcu2::SingleRead(uint32_t address, uint32_t* pData, uint32_t mode){
  CE_Debug("access to the register 0x%x\n", address);
  return Rcu2SingleRead(address, 1, pData);
}
int DevMsgbufferRcu2::MultipleWrite(uint32_t address, uint32_t* pData, int iSize, int iDataSize, uint32_t mode){
  return Rcu2MultipleWrite(address, iSize, pData);
}
int DevMsgbufferRcu2::MultipleRead(uint32_t address, int iSize,uint32_t* pData, uint32_t mode){
  return Rcu2MultipleRead(address, iSize, pData);
}
int DevMsgbufferRcu2::FlashErase(int startSec, int stopSec){ 
  return stopSec-startSec; 
}

