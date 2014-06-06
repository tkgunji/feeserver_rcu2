#include <cerrno>
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <typeinfo>
#include "rcu2.hpp"
#include "rcu_issue.h"
#include "ser.hpp"
#include "dcscMsgBufferInterface.h"
#include "i2c-dev.h"

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

  if(!fpRcu2 && fpMsgbuffer) fpRcu2=new DevRcu2(subDevices.size(), this,  fpMsgbuffer);
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
  return FEESVR_CMD_RCU;
}

int DevRcu2::HighLevelHandler(const char* pCommand, std::vector<uint32_t>& rb){
  //Handling of high-level commands (text strings)
  //High-level commands are typically converted into coresponding low-level (binary) commands and passed to the issue function
  CE_Event("DevRcu2::HighLevelHandler\n");


  if(!pCommand){
    CE_Error("Hight level fee command buffer does not exist\n");
    return -1;
  }
  uint32_t cmd=0;
  size_t keySize=0;
  std::string command=pCommand;
  std::string test="";
  if(command.find(test="EXAMPLE_COMMAND_1",0)==0){ keySize=test.size(); cmd=EXAMPLE_COMMAND_1; }
  if(command.find(test="RCU2_READ_REG",0)==0){ keySize=test.size(); cmd=RCU_READ_STATUS_REG; }
  else if(command.find(test="RCU2_WRITE_TO_REG",0)==0){ keySize=test.size(); cmd=RCU_WRITE_TO_REG; }

  /// following commands are added from rcu
  else if(command.find(test="RCU2_WRITE_AFL", 0)==0){keySize=test.size(); cmd=RCU_WRITE_AFL;}
  else if(command.find(test="RCU2_READ_AFL",0)==0){keySize=test.size(); cmd=RCU_READ_AFL;}
  else if(command.find(test="RCU2_WRITE_MEMORY",0)==0){keySize=test.size(); cmd=RCU_WRITE_TO_REG;}
  else if(command.find(test="RCU2_READ_FW_VERSION",0)==0){keySize=test.size();cmd=RCU_READ_FW_VERSION;}
  else if(command.find(test="RCU2_RESET",0)==0){keySize=test.size();cmd=RCU_RESET;}
  else if(command.find(test="RCU2_SYNC_SCLK_L1TTC",0)==0){keySize=test.size(); cmd=RCU_SYNC_SCLK_L1TTC;}
  else if(command.find(test="RCU2_ALTRO_INSTRUCTION_EXECUTE",0)==0){keySize=test.size(); cmd=RCU_ALTRO_INSTRUCTION_EXECUTE;}
  else if(command.find(test="RCU2_ALTRO_INSTRUCTION_CLEAR",0)==0){keySize=test.size(); cmd=RCU_ALTRO_INSTRUCTION_CLEAR;}
  else if(command.find(test="RCU2_ALTRO_INSTRUCTION_WRITE_FILE",0)==0){keySize=test.size(); cmd=RCU_ALTRO_INSTRUCTION_WRITE_FILE;}
  else if(command.find(test="RCU2_ALTRO_INSTRUCTION_READ_FILE",0)==0){keySize=test.size(); cmd=RCU_ALTRO_INSTRUCTION_READ_FILE;}
  else if(command.find(test="RCU2_CONFGFEC",0)==0){keySize=test.size(); cmd=RCU_CONFGFEC;}
  else if(command.find(test="RCU2_WRITE_EN_INT_BA",0)==0){keySize=test.size(); cmd=RCU_WRITE_EN_INT_BA;}
  else if(command.find(test="RCU2_READ_EN_INT_BA",0)==0){keySize=test.size(); cmd=RCU_READ_EN_INT_BA;}
  else if(command.find(test="REG_CFG_EXECUTE",0)==0){keySize=test.size(); cmd=REG_CFG_EXECUTE;}
  else if(command.find(test="REG_CFG_EXECUTE_SINGLE",0)==0){keySize=test.size(); cmd=REG_CFG_EXECUTE_SINGLE;}
  else if(command.find(test="REG_CFG_CLEAR",0)==0){keySize=test.size(); cmd=REG_CFG_CLEAR;}
  else if(command.find(test="REG_CFG_WRITE_FILE",0)==0){keySize=test.size(); cmd=REG_CFG_WRITE_FILE;}
  else if(command.find(test="REG_CFG_READ_FILE",0)==0){keySize=test.size();  cmd=REG_CFG_READ_FILE;}
  else if(command.find(test="RCU2_DRIVER_RELOAD",0)==0){keySize=test.size(); cmd=RCU_DRIVER_RELOAD;}
  else if(command.find(test="RCU2_DRIVER_UNLOAD",0)==0){keySize=test.size(); cmd=RCU_DRIVER_UNLOAD;}
  else if(command.find(test="RCU2_DRIVER_LOAD",0)==0){keySize=test.size(); cmd=RCU_DRIVER_LOAD;}
  else if(command.find(test="RCU2_TEST_ERROR",0)==0){keySize=test.size(); cmd=RCU_TEST_ERROR;}
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


int DevRcu2::Rcu2I2CTempRead(Ser::TceServiceData* pData, int32_t adc, int32_t unused, void* parameter){
  if(!parameter) return -1;
  DevRcu2* rcu2=(DevRcu2*) parameter;
  int nRet=0;
  uint32_t rawValue=1;
  float newValue=0;
  uint32_t tmp=1;


  //nRet |= rcu2->fpMsgbuffer->SingleI2CRead(0x40, &rawValue, 0);
  ///// routine 
  ///// start sampling 
  nRet |= rcu2->fpMsgbuffer->SingleI2CWrite(0x40, 0x40, 0x3e,1);
  //rcu2->fpMsgbuffer->SingleI2CWrite(0x40, 0x40, 0x3e,1);
  ////  getting data 
  sleep(1);
  rawValue = rcu2->fpMsgbuffer->SingleI2CRead(0x40, 0x03, &tmp,1);
  //printf("Rcu2I2CTempRead:: 0x%x %d\n", rawValue, nRet);
  newValue = 1.0*(((rawValue>>8) + (rawValue<<8)&0x00ffff));
  //newValue = rawValue;
  newValue = -46.85 + 175.72*newValue/65536.0;


  //rawValue = rcu2->fpMsgbuffer->SingleI2CRead(0x40, 0x03, &tmp,1);
  //printf("aaaaaa 0x%x\n", rawValue);
  //newValue = rawValue;
  //newValue = -46.85 + 175.72*newValue/65536.0;

  printf("Rcu2I2CTemperarture:: %f\n", newValue);  

  if(nRet>=-1) pData->fVal=newValue;
  else pData->fVal=nRet;
  return nRet;
}

int DevRcu2::Rcu2I2CRHRead(Ser::TceServiceData* pData, int32_t adc, int32_t unused, void* parameter){
  if(!parameter) return -1;
  DevRcu2* rcu2=(DevRcu2*) parameter;
  int nRet=0;
  uint32_t rawValue=1;
  float newValue=0;
  uint32_t tmp=1;


  //nRet |= rcu2->fpMsgbuffer->SingleI2CRead(0x40, &rawValue, 0);

  ///// routine 
  ///// start sampling 
  nRet |= rcu2->fpMsgbuffer->SingleI2CWrite(0x40, 0x40, 0xe5,1);
  ////  getting data 
  sleep(1);
  rawValue = rcu2->fpMsgbuffer->SingleI2CRead(0x40, 0x05, &tmp,1);
  //printf("Rcu2I2CRHRead:: 0x%x\n", rawValue);
  newValue = 1.0*(((rawValue>>8) + (rawValue<<8)&0x00ffff));
  //newValue = rawValue;  
  newValue = -6 + 125.0*newValue/65536.0;

  printf("Rcu2I2CRHumidity:: %f\n", newValue);  

  /*
  rawValue = rcu2->fpMsgbuffer->SingleI2CRead(0x40, 0x05, &tmp,2);
  printf("aaaaaa 0x%x\n", rawValue);
  newValue = rawValue;
  newValue = -6 + 125.0*newValue/65536.0;
  */
  if(nRet>=-1) pData->fVal=newValue;
  else pData->fVal=nRet;
  return nRet;
}

///pressure sensor 
int DevRcu2::Rcu2I2CPreRead(Ser::TceServiceData* pData, int32_t adc, int32_t unused, void* parameter){
  if(!parameter) return -1;
  DevRcu2* rcu2=(DevRcu2*) parameter;
  int nRet=0;
  uint32_t rawValue=1;
  float newValue=0;
  uint32_t tmp=1;
  float padc = 0;
  float tadc = 0;
  float a0 = 0.0;
  float b1 = 0.0;
  float b2 = 0.0;
  float c12 = 0.0;
  float pcomp=0.0;
  float pressure = 0.0;
  uint32_t res = 0; 
  uint32_t new_res = 0;
  int sign = 0;

  /// routine 
  /// start sampling for pressure 
  nRet |= rcu2->fpMsgbuffer->SingleI2CWrite(0x60, 0x12, 0x1,1);
  sleep(1);
  /// read padc
  res =  rcu2->fpMsgbuffer->SingleI2CRead(0x60, 0x00, &tmp, 1);
  padc = 1.0*(((res>>8) + (res<<8)&0x00ffff)) / 64;
  CE_Debug("padc = %f\n",padc);

  /// read tadc
  sleep(1);
  res =  rcu2->fpMsgbuffer->SingleI2CRead(0x60, 0x02, &tmp, 1);
  tadc = 1.0*(((res>>8) + (res<<8)&0x00ffff)) / 64;
  CE_Debug("tadc = %f\n",tadc);

  /// read a0
  sleep(1);
  res =  rcu2->fpMsgbuffer->SingleI2CRead(0x60, 0x04, &tmp, 1);
  new_res = (((res>>8) + (res<<8)&0x00ffff));
  sign = (new_res & 0x8000 ) >> 15;
  a0 = (new_res & 0x7ff8 ) >> 3;
  a0 += ( ((new_res & 0x4) >>2) / 2.0 + 
	  ((new_res & 0x2) >>1) / 4.0 + 
	  ((new_res & 0x1)) / 8.0 );
  a0 = (1-2*sign)*a0;
  CE_Debug("a0 = %f\n",a0);

  //// read a1
  sleep(1);
  res =  rcu2->fpMsgbuffer->SingleI2CRead(0x60, 0x06, &tmp, 1);
  new_res = (((res>>8) + (res<<8)&0x00ffff));
  /////////////
  sign = (new_res & 0x8000 ) >> 15;
  new_res = ~new_res;
  new_res += 1;
  b1 = (new_res & 0x6000 ) >> 13;
  b1 += ( ((new_res & 0x1000) >>12) / 2.0 + 
	  ((new_res & 0x0800) >>11) / 4.0 + 
	  ((new_res & 0x0400) >>10) / 8.0 + 
	  ((new_res & 0x0200) >> 9) / 16.0 + 
	  ((new_res & 0x0100) >> 8) / 32.0 + 
	  ((new_res & 0x0080) >> 7) / 64.0 + 
	  ((new_res & 0x0040) >> 6) / 128.0 + 
	  ((new_res & 0x0020) >> 5) / 256.0 + 
	  ((new_res & 0x0010) >> 4) / 512.0 + 
	  ((new_res & 0x0008) >> 3) / 1024.0 + 
	  ((new_res & 0x0004) >> 2) / 2048.0 + 
	  ((new_res & 0x0002) >> 1) / 4096.0 + 
	  ((new_res & 0x0001)  ) / 8192.0); 
  b1 = (1-2*sign)*b1;
  CE_Debug("b1 = %f\n",b1);

  ///// read b2
  sleep(1);
  res =  rcu2->fpMsgbuffer->SingleI2CRead(0x60, 0x08, &tmp, 1);
  new_res = (((res>>8) + (res<<8)&0x00ffff));
  sign = (new_res & 0x8000 ) >> 15;
  new_res = ~new_res;
  new_res += 1;
  b2 = (new_res & 0x4000 ) >> 14;
  b2 += (
	 ((new_res & 0x2000) >>13) / 2.0 + 
	 ((new_res & 0x1000) >>12) / 4.0 + 
	 ((new_res & 0x0800) >>11) / 8.0 + 
	 ((new_res & 0x0400) >>10) / 16.0 + 
	 ((new_res & 0x0200) >> 9) / 32.0 + 
	 ((new_res & 0x0100) >> 8) / 64.0 + 
	 ((new_res & 0x0080) >> 7) / 128.0 + 
	 ((new_res & 0x0040) >> 6) / 256.0 + 
	 ((new_res & 0x0020) >> 5) / 512.0 + 
	 ((new_res & 0x0010) >> 4) / 1024.0 + 
	 ((new_res & 0x0008) >> 3) / 2048.0 + 
	 ((new_res & 0x0004) >> 2) / 4096.0 + 
	 ((new_res & 0x0002) >> 1) / 8192.0 + 
	 ((new_res & 0x0001)  ) / 16384.0); 
  b2 = (1-2*sign)*b2;
  CE_Debug("b2 = %f\n",b2);

  ////// read c12
  sleep(1);
  res =  rcu2->fpMsgbuffer->SingleI2CRead(0x60, 0x0a, &tmp, 1);
  new_res = (((res>>8) + (res<<8)&0x00ffff));
  /////////////
  sign = (new_res & 0x8000 ) >> 15;
  new_res = new_res >> 2;
  /////////////
  c12 = (
	 ((new_res & 0x1000) >>12) / 1024.0 + 
	 ((new_res & 0x0800) >>11) / 2048.0 + 
	 ((new_res & 0x0400) >>10) / 4096.0 + 
	 ((new_res & 0x0200) >> 9) / 8192.0 + 
	 ((new_res & 0x0100) >> 8) / 16384.0 + 
	 ((new_res & 0x0080) >> 7) / 32768.0 + 
	 ((new_res & 0x0040) >> 6) / 65536.0 +  
	 ((new_res & 0x0020) >> 5) / 131072.0 + 
	 ((new_res & 0x0010) >> 4) / 262144.0 +  
	 ((new_res & 0x0008) >> 3) / 524288.0 + 
	 ((new_res & 0x0004) >> 2) / 1048576.0 + 
	 ((new_res & 0x0002) >> 1) / 2097152.0 + 
	 ((new_res & 0x0001)  ) /  4194304);
  c12 = (1-2*sign)*c12;
  CE_Debug("c12 = %f\n",c12);

  pcomp = a0 + (b1+c12*tadc)*padc+b2*tadc;
  pressure = pcomp*(115.0-50.0)/1023.0+50;

  printf("Rcu2I2CPressure:: %f\n", pressure);  

  if(nRet>=0) pData->fVal=pressure;
  else pData->fVal=nRet;
  return nRet;
}

int DevRcu2::ArmorLocal(){
  //Will be executed at start of FeeServer

  std::string name=GetName();
  //these are the test purposes
  fServices.push_back(new SerMbAddrS(name+"_CTRLREG", TTCControl, 1, 0, fpMsgbuffer));  
  fServices.push_back(new SerMbAddrS(name+"_L1_LATENCY_REG", TTCL1Latency, 1, 0, fpMsgbuffer));  
  fServices.push_back(new SerMbAddrS(name+"_L2_LATENCY_REG", TTCL2Latency, 1, 0, fpMsgbuffer));  
  fServices.push_back(new SerMbAddrS(name+"_VERSION", V2_RCU_Version, 1, 0, fpMsgbuffer));  
  //list of services inherited from rcu  
  fServices.push_back(new SerMbAddrS(name+"_AFL"          , V2_FECActiveList_RO  , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"_FECERR_AI"     , V2_FECErrAI_RO     , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"_FECERR_AO"     , V2_FECErrAO_RO     , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"_FECERR_BI"     , V2_FECErrBI_RO     , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"_FECERR_BO"     , V2_FECErrBO_RO     , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"_FWVERSION"    , RCUFwVersion      , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"_SLC_STATUS_V2"   , FECResultREG         , 1, 0, fpMsgbuffer));
  

  name=GetName()+"_TEMP";
  Ser::RegisterService(Ser::eDataTypeFloat, name.c_str(), 0.01, DevRcu2::Rcu2I2CTempRead, 0, 0, 0, this);
  name=GetName()+"_RH";
  Ser::RegisterService(Ser::eDataTypeFloat, name.c_str(), 0.01, DevRcu2::Rcu2I2CRHRead, 0, 0, 0, this);
  name=GetName()+"_Pressure";
  Ser::RegisterService(Ser::eDataTypeFloat, name.c_str(), 0.01, DevRcu2::Rcu2I2CPreRead, 0, 0, 0, this);



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
    CE_Debug("Command:: No operation\n");
    iResult=0;
    break;
  case RCU_READ_STATUS_REG:
    CE_Debug("Command::RCU2_READ_REG\n");
    ////////////////////////////////////////////////
    //////// parameter.c_str() = pData = [addr] [data] /////
    if(sscanf(pBuffer, "0x%x", &address)>0){
      uint32_t rbsize=rb.size();
      rb.resize(rbsize+1);
      fpMsgbuffer->SingleRead(address,&rb[rbsize]);
      fpMsgbuffer->SingleRead(address, &data); 
      CE_Event("DevRcu2 read data =  0x%x (address = 0x%x)\n", data, address);
      iResult=0;
    }else{
      CE_Warning("DevRcu2 read data format is wrong!!\n");
      iResult=0;
    }
    break;
  case RCU_WRITE_TO_REG:
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
  case RCU_WRITE_AFL:
    SetAfl(*(uint32_t*)pData);
    iResult=4;
    break;
  case RCU_READ_AFL:
    {
      uint32_t rbsize=rb.size();
      rb.resize(rbsize+parameter);
      fpMsgbuffer->MultipleRead(V2_ALTROACL, parameter, &rb[rbsize]);
      iResult=0;
    }
    break;
  case RCU_READ_FW_VERSION:
    {
      uint32_t rbsize=rb.size();
      rb.resize(rbsize+1);
      fpMsgbuffer->SingleRead(V2_RCU_Version,&rb[rbsize]);
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
  case RCU_SYNC_SCLK_L1TTC:
    fpMsgbuffer->SingleWrite(V2_CMDSCLKsync,0);
    iResult=0;
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
	    uint32_t errorAI=0;
	    uint32_t errorBI=0;
	    uint32_t errorAO=0;
	    uint32_t errorBO=0;
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
	      fpMsgbuffer->SingleRead(V2_FECErrAI_RO,&errorAI);
	      fpMsgbuffer->SingleRead(V2_FECErrBI_RO,&errorBI);
	      fpMsgbuffer->SingleRead(V2_FECErrAO_RO,&errorAO);
	      fpMsgbuffer->SingleRead(V2_FECErrBO_RO,&errorBO);
	      fpMsgbuffer->SingleRead(V2_BUSBSY,&errorBusy);
	      i++;
	    }
	    while(i<4 && (errorAI!=0 || errorBI!=0 || errorAO!=0 || errorBO!=0 || (errorRes&0x100000)!=0 || (errorBusy&0x1)!=0));
	    if(i>=4 && (errorRes&100000)!=0) {someError=1; CE_Error("Execution of instruction memory failed; result memory line %#x: %#x (stickyErrorRepeat=%#x)\n", j, errorRes, stickyErrorRepeat);}
	    if(i>=4 && errorAI!=0) {someError=1; CE_Error("Execution of instruction memory failed; FEC AI error register: %#x (stickyErrorRepeat=%#x)\n", errorAI, stickyErrorRepeat);}
	    if(i>=4 && errorBI!=0) {someError=1; CE_Error("Execution of instruction memory failed; FEC BI error register: %#x (stickyErrorRepeat=%#x)\n", errorBI, stickyErrorRepeat);}
	    if(i>=4 && errorAO!=0) {someError=1; CE_Error("Execution of instruction memory failed; FEC AO error register: %#x (stickyErrorRepeat=%#x)\n", errorAO, stickyErrorRepeat);}
	    if(i>=4 && errorBO!=0) {someError=1; CE_Error("Execution of instruction memory failed; FEC BO error register: %#x (stickyErrorRepeat=%#x)\n", errorBO, stickyErrorRepeat);}
	    if(i>=4 && (errorBusy&0x2)!=0){someError=1; CE_Error("Execution of instruction memory failed; BusBusy register: %#x (stickyErrorRepeat=%#x)\n", errorBusy, stickyErrorRepeat);}
	    index=0;
	  }
	}
	if(imem){delete[] imem; imem=0;}
	if(rmem){delete[] rmem; rmem=0;}
	uint32_t errorAI=0;
	uint32_t errorBI=0;
	uint32_t errorAO=0;
	uint32_t errorBO=0;
	fpMsgbuffer->SingleRead(V2_FECErrAI_RO,&errorAI);
	fpMsgbuffer->SingleRead(V2_FECErrBI_RO,&errorBI);
	fpMsgbuffer->SingleRead(V2_FECErrAO_RO,&errorAO);
	fpMsgbuffer->SingleRead(V2_FECErrBO_RO,&errorBO);
	if((errorAI&0x1) || (errorBI&0x1) || (errorAO&0x1) || (errorBO&0x1)){
	  stickyErrorRepeat--;
	  if(!stickyErrorRepeat) {someError=1; CE_Error("Execution of instruction memory failed; unrecoverable sticky bit found, FEC AI/AO error register: %#x/%#x, FEC BI/BO error register: %#x/%#x\n", errorAI, errorAO, errorBI, errorBO);}
	  else {someError=0; fpMsgbuffer->SingleWrite(V2_CMDRESET,0);}
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
  case RCU_CONFGFEC:
    fpMsgbuffer->SingleWrite(CONFGFEC,1);
    sleep(1);
    iResult=0;
    break;
  case RCU_WRITE_EN_INT_BA: //this address needs to be checked!
    fpMsgbuffer->SingleWrite(FECINTmode, *(uint32_t*)pData);
    iResult=4;
    break;
  case RCU_READ_EN_INT_BA: //this address needs to be checked! 
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
  if(initRcuAccess(NULL, NULL)<0){
    CE_Error("initRcuAccess finished with error code\n");
  }
  return 1;
}
int DevMsgbufferRcu2::SingleWrite(uint32_t address, uint32_t data, uint32_t mode){
  CE_Debug("access to the register 0x%x for writing\n", address);
 return Rcu2SingleWrite(address, 1, data);
}
int DevMsgbufferRcu2::SingleRead(uint32_t address, uint32_t* pData, uint32_t mode){
  CE_Debug("access to the register 0x%x for reading\n", address);
  return Rcu2SingleRead(address, 1, pData);
  //return rcuSingleRead(address, pData);
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

int DevMsgbufferRcu2::SingleI2CRead(uint32_t base, uint32_t address, uint32_t* pData, uint32_t mode){
  CE_Debug("access to the i2c register 0x%x for reading\n", address);
  int data = 0;
  data = Rcu2SingleI2CRead(base, address, mode, pData);
  CE_Debug("reading out from the i2c register 0x%x : data = 0x%x (0x%x)\n", address, pData, data);
  return data;
}

int DevMsgbufferRcu2::SingleI2CWrite(uint32_t base, uint32_t address, uint32_t pData, uint32_t mode){
  CE_Debug("access to the i2c register 0x%x for writing 0x%x\n", address, pData);
  return Rcu2SingleI2CWrite(base, address, 1, pData);
}

