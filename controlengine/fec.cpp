#include <unistd.h>
#include "fec.hpp"
#include "messagebuffer.hpp"
#include "rcu.hpp"
#include "ctr.hpp"

/******************************************************************************/

DevFec::DevFec(std::string name, int id,  CEStateMachine* pParent, DevFecaccess* fecaccess, uint32_t fec): Dev(name, id, pParent),fpFecaccess(fecaccess),fFec(fec){
  AddTransition(new CETransition(kActionGoOff, kStateOff, std::string(kActionGoOff_name), DevFec::CtrRcuGO_OFF, this, 4, kStateError, kStateFailure, kStateStandby, kStateOn));
  AddTransition(new CETransition(kActionGoOn, kStateOn, std::string(kActionGoOn_name), DevFec::CtrRcuGO_ON, this, 4, kStateError, kStateFailure, kStateStandby, kStateOff));
}
int DevFec::CtrRcuGO_OFF(CEStateMachine* callbackObject){
  int nRet=(dynamic_cast<DevFec*>(callbackObject))->fpFecaccess->SetFecAflStatus(0, (dynamic_cast<DevFec*>(callbackObject))->fFec);
  if(nRet<0){
    CE_Warning("Writing to AFL returned error code %i while turning off FEC %d\n", nRet, (dynamic_cast<DevFec*>(callbackObject))->fFec);
    return -1;
  }
  return 1;
}
int DevFec::CtrRcuGO_ON(CEStateMachine* callbackObject){
  int nRet=(dynamic_cast<DevFec*>(callbackObject))->fpFecaccess->SetFecAflStatus(1, (dynamic_cast<DevFec*>(callbackObject))->fFec);
  if(nRet<0){
    CE_Warning("Writing to AFL returned error code %i while turning on FEC %d\n", nRet, (dynamic_cast<DevFec*>(callbackObject))->fFec);
    return -1;
  }
  return 1;
}
int DevFec::ReSynchronizeLocal(){
  //CE_Debug("DevFec::ReSynchronizeLocal\n");
  if (fpFecaccess==NULL) return kStateError;
  uint32_t status=0;
  int nRet=0;
  nRet=fpFecaccess->GetFecAflStatus(status, fFec);
  if (nRet<0){
    CE_Warning("checking AFL for FEC %d failed, %d\n", fFec, nRet);
    return kStateFailure;
  }
  if(status==0) return kStateOff;
  nRet=fpFecaccess->ReadFecReg(status, fFec, FECCSR2);
  if (nRet<0){
    CE_Warning("checking CSR2 for FEC %d failed, %d\n", fFec, nRet);
    return kStateFailure;
  }
  if((status&0x3)==0x3) return kStateOn;
  return kStateStandby;
}
int DevFec::EnterStateON(){
  if(!fpFecaccess){
    CE_Warning("fpFecaccess did not exist when entering state ON for FEC %d\n", fFec);
    return -ENOSYS;
  }
  /**
  * TPC test June 2007
  * According to Roberto and some specifications of the I2C bus, the bus has to
  * be set into a correct state. This is done with a few dummy readout operations
  */
  int nRet=0;
  uint32_t data=0;
  for(int i=0;i<5;i++){
    nRet=fpFecaccess->ReadFecReg(data, fFec, FECCSR0);
    if(nRet<0) CE_Warning("ReadFecReg returned error code %d while setting state ON for FEC %d\n", nRet, fFec);
  }
  PreUpdateLocal();
  return 0;
}

int DevFec::PreUpdateLocal(){
  //CE_Debug("DevFec::PreUpdateLocal\n");
  uint32_t status=0;
  int nRet=0;
  status=0;
  fpFecaccess->GetFecAflStatus(status, fFec);
  if(status){
    uint32_t data=0;
    if(fpFecaccess->ReadFecReg(data, fFec, FECCSR0)<0) CE_Warning("ReadFecReg returned error code %d while reading from FEC %d\n", fFec, nRet);
    if(!(data&1024)){
      data|=1024;
      if(fpFecaccess->WriteFecReg(data, fFec, FECCSR0)<0) CE_Warning("ReadFecReg returned error code %d while writing to FEC %d\n", fFec, nRet);
      CE_Debug("Turned on continous ADC for FEC %d (hardware address).\n", fFec);
    }
  }
  return 0;
}

/******************************************************************************/

DevFecaccess::DevFecaccess(DevMsgbuffer* msgbuffer): fpMsgbuffer(msgbuffer){
}
int DevFecaccess::ReadFecReg(uint32_t &data, uint32_t fec, uint32_t reg){
  return AccessFecReg(data, fec, reg, 1);
}
int DevFecaccess::WriteFecReg(uint32_t data, uint32_t fec, uint32_t reg){
  return AccessFecReg(data, fec, reg, 0);
}

int DevFecaccess::DetectFecs(uint32_t &detectedFecs){
  CE_Debug("DevFecaccess::DetectFecs\n");
  int nRet=0;
  uint32_t orgAfl=0;
  nRet=GetAfl(orgAfl);
  if(nRet<0) return nRet;
  nRet=SetAfl(0xffffffff);
  if(nRet<0) return nRet;
  uint32_t rawData=0;
  detectedFecs=0;
  for(uint32_t i=0;i<32;i++){
    nRet=AccessFecReg(rawData, i, 0, 1);
    if(nRet<0) detectedFecs=detectedFecs | (0x0<<i);
    else       detectedFecs=detectedFecs | (0x1<<i);
  }
  nRet=SetAfl(orgAfl);
  if (nRet<0) return nRet;
  return 0;
}
int DevFecaccess::GetFecAflStatus(uint32_t &status, uint32_t fec){

  int nRet=0;
  uint32_t afl=0;
  nRet=GetAfl(afl);
  if (nRet<0) return nRet;
  status=(afl&(0x1<<fec))>>fec;
  return 0;
}
int DevFecaccess::SetFecAflStatus(uint32_t status, uint32_t fec){
  CE_Debug("DevFecaccess::SetFecAflStatus\n");

  int nRet=0;
  uint32_t afl=0;
  nRet=GetAfl(afl);
  if (nRet<0) return nRet;
  CE_Debug("n=%x, fec=%x, status=%x, afl=%x\n", 2, fec, status, afl);
  if(status^(afl&(0x1<<fec))){
    if(status==0) afl=afl&~(0x1<<fec);
    else afl=afl|(0x1<<fec);
    CE_Debug("n=%x, fec=%x, status=%x, afl=%x\n", 3, fec, status, afl);
    nRet=fpMsgbuffer->SingleWrite(V2_FECActiveList, afl);
    if (nRet<0){
      CE_Warning("can not write RCU AFL %#x: SingleWrite failed (%d)\n", V2_FECActiveList, nRet);
      return -2100;
    }
    //Wait for board controllers to come up
    usleep(300000);
    nRet=fpMsgbuffer->SingleWrite(CMDRESETFEC, 0);
    if (nRet<0){
      CE_Warning("can not write CMDRESETFEC %#x: SingleWrite failed (%d)\n", CMDRESETFEC, nRet);
      return -2101;
    }
    nRet=WriteFecReg(0x7FF, fec, FECCSR0);
    if(nRet<0){
      CE_Warning("can not set continous ADC for FEC %d, WriteFecReg returned error code %d\n", fec, nRet);
      return nRet;
    }
  }
  return 0;
}
int DevFecaccess::GetAfl(uint32_t &value){
  int nRet=0;
  nRet=fpMsgbuffer->SingleRead(V2_FECActiveList_RO, &value);
  if (nRet<0){
    CE_Warning("can not read RCU AFL %#x: SingleRead failed (%d)\n", V2_FECActiveList_RO, nRet);
    return -2102;
  }
  return 0;
}
int DevFecaccess::SetAfl(uint32_t value){
  int nRet=0;
  for(uint32_t i=0; i<32; i++){
    nRet=SetFecAflStatus(value&(0x1<<i), i);
    if(nRet<0) return nRet;
  }
  return 0;
}

DevFecaccessDummy::DevFecaccessDummy(DevMsgbuffer* msgbuffer): DevFecaccess(msgbuffer){
}
DevFecaccessDummy::~DevFecaccessDummy(){
  fMemory.clear();
}
int DevFecaccessDummy::AccessFecReg(uint32_t &data, uint32_t fec, uint32_t reg, uint32_t rnw){
  if(rnw==0) fMemory[fec*256+reg]=data;
  else data=fMemory[fec*256+reg];
  return 0;
}

