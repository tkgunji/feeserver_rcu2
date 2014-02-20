#include <cerrno>
#include <cstdlib>
#include <typeinfo>
#include <cstdio>
#include "busybox.hpp"
#include "rcu2_issue.h"
#include "dcb.hpp"
#include "ser.hpp"
#include "dcscMsgBufferInterface.h"

/******************************************************************************/

void ce_exec_loop(){
  CE_Event("This is ControlEngine for BusyBox\n");
  if(!Ctr::fpInstance) Ctr::fpInstance=new CtrBb();
  if(Ctr::fpInstance){
    CE_Event("ControlEngine '%s' (%i) of type '%s' created at %p\n", Ctr::fpInstance->GetName().c_str(), Ctr::fpInstance->GetDeviceId(), typeid(*Ctr::fpInstance).name(), Ctr::fpInstance);
    Ctr::fpInstance->RunCE();
  }
  else CE_Warning("ControlEngine not created at %p\n", Ctr::fpInstance);
  return;
}

/******************************************************************************/

CtrBb::CtrBb(): fpBb(NULL){
  AddTransition(new CETransition(kActionGoStandby, kStateStandby, std::string(kActionGoStandby_name), NULL, NULL, 4, kStateError, kStateFailure, kStateRunning, kStateDownloading));
  AddTransition(new CETransition(kActionConf, kStateDownloading, std::string(kActionConf_name), NULL, NULL, 2, kStateStandby, kStateRunning));
  AddTransition(new CETransition(kActionConfigureDone, kStateRunning, std::string(kActionConfigureDone_name), NULL, NULL, 1, kStateDownloading));
  if     (!fpMsgbuffer && fHwType==1) fpMsgbuffer=new DevMsgbufferDcb();
  else if(!fpMsgbuffer && fHwType==2) fpMsgbuffer=new DevMsgbufferDummy();
  if(fpMsgbuffer) CE_Event("DevMsgbuffer of type '%s' created at %p with HwType %i\n", typeid(*fpMsgbuffer).name(), fpMsgbuffer, fHwType);
  else CE_Warning("DevMsgbuffer not created at %p with HwType %i\n", fpMsgbuffer, fHwType);
  if(!fpBb && fpMsgbuffer) fpBb=new DevBb(subDevices.size(), this, fpMsgbuffer);
  if(fpBb){
    AddSubDevice(fpBb);
    CE_Event("DevBb '%s' (%i) of type '%s' created at %p with DevMsgbuffer at %p\n", fpBb->GetName().c_str(), fpBb->GetDeviceId(), typeid(*fpBb).name(), fpBb, fpMsgbuffer);
  }
  else CE_Warning("DevBb not created at %p with DevMsgbuffer %p\n", fpBb, fpMsgbuffer);
}

CtrBb::~CtrBb(){
  fpMsgbuffer=NULL; // deleted in sub-device handling
  fpBb=NULL;
}

/******************************************************************************/

DevBb::DevBb(int id,  CEStateMachine* pParent, DevMsgbuffer* msgbuffer): Dev("BB", id, pParent),fpMsgbuffer(msgbuffer){
  char* FEE_SERVER_BB_FPGAS = NULL;
  FEE_SERVER_BB_FPGAS = getenv("FEE_SERVER_BB_FPGAS");
  if(FEE_SERVER_BB_FPGAS){
    if      (strncmp(FEE_SERVER_BB_FPGAS, "0", strlen("0"))==0) fFeeServerBbFpgas=0x0;
    else if (strncmp(FEE_SERVER_BB_FPGAS, "1", strlen("1"))==0) fFeeServerBbFpgas=0x1;
    else if (strncmp(FEE_SERVER_BB_FPGAS, "2", strlen("2"))==0) fFeeServerBbFpgas=0x2;
    //TODO: Determine number of FPGAS from register probing
    else{
     fFeeServerBbFpgas=0x2;
     CE_Warning("Environment variable FEE_SERVER_BB_FPGAS set to invalid value %s, defaulting to 2\n", FEE_SERVER_BB_FPGAS);
    }
  }
  else{
    CE_Warning("Environment variable FEE_SERVER_BB_FPGAS not set, defaulting to 2\n");
    fFeeServerBbFpgas=0x2;
  }
}
int DevBb::ReSynchronizeLocal(){
  if(!fpMsgbuffer) return kStateError;
  return kStateStandby;
}
int DevBb::ArmorLocal(){
  if(fFeeServerBbFpgas>2){
    CE_Error("Invalid number of FPGAS %d\n", fFeeServerBbFpgas);
    return -EINVAL;
  }
  for(uint32_t i=0;i<fFeeServerBbFpgas;i++){
    char fpgaNumber[]=" ";
    snprintf(fpgaNumber,2,"%.1u",i);
    std::string name=GetName();
    name+="_";
    name+=fpgaNumber;
    name+="_";
    
    fServices.push_back(new SerMbAddrS(name+"TXR"         , BbTXmodule     + FPGAOffset *i, 1, 0, fpMsgbuffer));
    fServices.push_back(new SerMbAddrS(name+"RXMP"        , BbRXmemp       + FPGAOffset *i, 1, 0, fpMsgbuffer));
    fServices.push_back(new SerMbAddrS(name+"EIDFIFOC"    , BbEventFIFOCNT + FPGAOffset *i, 1, 0, fpMsgbuffer));
    fServices.push_back(new SerMbAddrS(name+"CEID0"       , BbCurEventID1  + FPGAOffset *i, 1, 0, fpMsgbuffer));
    fServices.push_back(new SerMbAddrS(name+"CEID1"       , BbCurEventID2  + FPGAOffset *i, 1, 0, fpMsgbuffer));
    fServices.push_back(new SerMbAddrS(name+"CEID2"       , BbCurEventID3  + FPGAOffset *i, 1, 0, fpMsgbuffer));
    fServices.push_back(new SerMbAddrS(name+"MREID0"      , BbNewEventID1  + FPGAOffset *i, 1, 0, fpMsgbuffer));
    fServices.push_back(new SerMbAddrS(name+"MREID1"      , BbNewEventID2  + FPGAOffset *i, 1, 0, fpMsgbuffer));
    fServices.push_back(new SerMbAddrS(name+"MREID2"      , BbNewEventID3  + FPGAOffset *i, 1, 0, fpMsgbuffer));
    fServices.push_back(new SerMbAddrS(name+"L0TTO"       , BbL0TTO        + FPGAOffset *i, 1, 0, fpMsgbuffer));
    fServices.push_back(new SerMbAddrS(name+"FEEBA"       , BbBusyCond     + FPGAOffset *i, 1, 0, fpMsgbuffer));
    fServices.push_back(new SerMbAddrS(name+"FSMH"        , BbHaltFSMmatch + FPGAOffset *i, 1, 0, fpMsgbuffer));
//  fServices.push_back(new SerMbAddrS(name+"FM"          , BbForematch    + FPGAOffset *i, 1, 0, fpMsgbuffer));
    fServices.push_back(new SerMbAddrS(name+"RRTO"        , BbReReqTimeout + FPGAOffset *i, 1, 0, fpMsgbuffer));
    fServices.push_back(new SerMbAddrS(name+"CRID"        , BbCurRequestID+ FPGAOffset *i, 1, 0, fpMsgbuffer));
    fServices.push_back(new SerMbAddrS(name+"RC"          , BbRetryCount   + FPGAOffset *i, 1, 0, fpMsgbuffer));
    fServices.push_back(new SerMbAddrS(name+"BT0"         , BbBusytimeH    + FPGAOffset *i, 1, 0, fpMsgbuffer));
    fServices.push_back(new SerMbAddrS(name+"BT1"         , BbBusytimeL    + FPGAOffset *i, 1, 0, fpMsgbuffer));
    fServices.push_back(new SerMbAddrS(name+"RXMF"        , BbRXmemfilter  + FPGAOffset *i, 1, 0, fpMsgbuffer));
    fServices.push_back(new SerMbAddrS(name+"FWVERSION"   , BbFwVersion    + FPGAOffset *i, 1, 0, fpMsgbuffer));
    fServices.push_back(new SerMbAddrS(name+"TRIGMODE"    , BbTrigMode     + FPGAOffset *i, 1, 0, fpMsgbuffer));
    fServices.push_back(new SerMbAddrS(name+"TTC_CONTROL0", BbTrmCTR1      + FPGAOffset *i, 1, 0, fpMsgbuffer));
    fServices.push_back(new SerMbAddrS(name+"TTC_CONTROL1", BbTrmCTR2      + FPGAOffset *i, 1, 0, fpMsgbuffer));
    fServices.push_back(new SerMbAddrS(name+"TTC_L1L"     , BbTrmL1LAT     + FPGAOffset *i, 1, 0, fpMsgbuffer));
    fServices.push_back(new SerMbAddrS(name+"TTC_L2LMAX"  , BbTrmL2LATMAX  + FPGAOffset *i, 1, 0, fpMsgbuffer));
    fServices.push_back(new SerMbAddrS(name+"TTC_L2LMIN"  , BbTrmL2LATMIN  + FPGAOffset *i, 1, 0, fpMsgbuffer));
    fServices.push_back(new SerMbAddrS(name+"TTC_BCIDO"   , BbTrmBCIDL     + FPGAOffset *i, 1, 0, fpMsgbuffer));
    fServices.push_back(new SerMbAddrS(name+"TTC_L0C"     , BbTrmL0CNT     + FPGAOffset *i, 1, 0, fpMsgbuffer));
    fServices.push_back(new SerMbAddrS(name+"TTC_L1C"     , BbTrmL1CNT     + FPGAOffset *i, 1, 0, fpMsgbuffer));
    fServices.push_back(new SerMbAddrS(name+"TTC_L1MC"    , BbTrmL1MCNT    + FPGAOffset *i, 1, 0, fpMsgbuffer));
    fServices.push_back(new SerMbAddrS(name+"TTC_L2AC"    , BbTrmL2ACNT    + FPGAOffset *i, 1, 0, fpMsgbuffer));
    fServices.push_back(new SerMbAddrS(name+"TTC_L2RC"    , BbTrmL2RCNT    + FPGAOffset *i, 1, 0, fpMsgbuffer));
    Ser::RegisterService(Ser::eDataTypeString, (name+"STATUS").c_str(), 0, DevBb::updateServiceStatus, 0, i, 0, fpMsgbuffer);
  }
  return 0;
}

int DevBb::updateServiceStatus(Ser::TceServiceData* pData, int major, int minor, void* parameter){
  int nRet=0;
  char values[129];
  uint32_t rawValue=0;
  uint32_t address=0x0;
  for(uint32_t j=0;j<128;j++){
    address=0x2100+0x8000*major+j;
    nRet=((DevMsgbuffer*)parameter)->SingleRead(address, &rawValue);
    if     (nRet<0)        values[j]='F';
    else if(rawValue==0x0) values[j]='0';
    else if(rawValue==0x1) values[j]='1';
    else if(rawValue==0x2) values[j]='2';
    else if(rawValue==0x3) values[j]='3';
    else                   values[j]='U';
  }
  values[128]='\0';
  *(std::string*)(pData->strVal)=values;
  return 0;
}

int DevBb::WriteReg(uint32_t cmd, uint32_t address, uint32_t value, int iDataSize){
  int size=1*sizeof(uint32_t);
  if(iDataSize<size){
    CE_Warning("data size missmatch, %i byte available in buffer but %i requested by command %p\n", iDataSize, size, cmd);
    return -EINVAL;
  }
  for(uint32_t i=0;i<fFeeServerBbFpgas;i++) fpMsgbuffer->SingleWrite(address+ FPGAOffset *i, value);
  return size;
}
int DevBb::issue(uint32_t cmd, uint32_t parameter, const char* pData, int iDataSize, std::vector<uint32_t>& rb){
  int size=0;
  CE_Debug("DevBb::issue cmd=%p parameter=%p datasize=%i\n", cmd, parameter, iDataSize);
  switch (cmd) {
  //Write single registers
  case BB_WRITE_TX:
    size=WriteReg(cmd, BbTXmodule, *(uint32_t*)pData, iDataSize);
    break;
  case BB_WRITE_L0TTO:
    size=WriteReg(cmd, BbL0TTO, *(uint32_t*)pData, iDataSize);
    break;
  case BB_WRITE_FEEBA:
    size=WriteReg(cmd, BbBusyCond, *(uint32_t*)pData, iDataSize);
    break;
  case BB_WRITE_FSMH:
    size=WriteReg(cmd, BbHaltFSMmatch, *(uint32_t*)pData, iDataSize);
    break;
  case BB_WRITE_FM:
    size=WriteReg(cmd, BbForcematch, *(uint32_t*)pData, iDataSize);
    break;
  case BB_WRITE_RRTO:
    size=WriteReg(cmd, BbReReqTimeout, *(uint32_t*)pData, iDataSize);
    break;
  case BB_WRITE_RXMF:
    size=WriteReg(cmd, BbRXmemfilter, *(uint32_t*)pData, iDataSize);
    break;
  case BB_WRITE_TRIGMODE:
    size=WriteReg(cmd, BbTrigMode, *(uint32_t*)pData, iDataSize);
    break;
  case BB_WRITE_TTC_CONTROL0:
    size=WriteReg(cmd, BbTrmCTR1, *(uint32_t*)pData, iDataSize);
    break;
  case BB_WRITE_TTC_CONTROL1:
    size=WriteReg(cmd, BbTrmCTR2, *(uint32_t*)pData, iDataSize);
    break;
  case BB_WRITE_TTC_TMR:
    size=WriteReg(cmd, BbTrmMRST, *(uint32_t*)pData, iDataSize);
    break;
  case BB_WRITE_TTC_L0L:
    size=WriteReg(cmd, 0x300a, *(uint32_t*)pData, iDataSize);
    break;
  case BB_WRITE_TTC_L1L:
    size=WriteReg(cmd, BbTrmL1LAT, *(uint32_t*)pData, iDataSize);
    break;
  case BB_WRITE_TTC_BCIDO:
    size=WriteReg(cmd, 0x3016, *(uint32_t*)pData, iDataSize);
    break;
  //Write status registers:
  case BB_WRITE_STATUS:
    size=256*sizeof(uint32_t);
    if(iDataSize<size){
      CE_Warning("data size missmatch, %d byte available in buffer but %d requested by command (%#x)\n", iDataSize, size, cmd);
      return -EINVAL;
    }
    {
      for(uint32_t i=0;i<fFeeServerBbFpgas;i++) fpMsgbuffer->MultipleWrite(0x2100+ FPGAOffset *i, (uint32_t*)pData, size);
    }
    break;
  //Reset FW
  case BB_FIRMWARE_RESET:
    {
      rcuBusControlCmd(eResetFirmware);
    }
    size=0;
    break;
  //Complete BB init
  case BB_INIT:
    size=256*sizeof(uint32_t);
    //check if we got the right amount of data
    if(iDataSize<=size){
      CE_Warning("data size missmatch, %d byte available in buffer but %d requested by command (%#x)\n", iDataSize, size, cmd);
      return -EINVAL;
    }
    uint32_t data[128];
    for(uint32_t j=0;j<128;j++) data[j]=0x0;
    {
      //Firmware reset
      rcuBusControlCmd(eResetFirmware);
      for(uint32_t i=0;i<fFeeServerBbFpgas;i++){
        //Setup trigger module
          //Reset
        fpMsgbuffer->SingleWrite(BbTrmMRST + FPGAOffset *i, 0x0000);
          //enable serialB, disable L0 support and set L2A mask
        fpMsgbuffer->SingleWrite(BbTrmCTR1 + FPGAOffset *i, 0x0109);
//        //BCID offset
//      fpMsgbuffer->SingleWrite(0x3016+ FPGAOffset *i, 0x085f);
//        //L0 Latency
//      fpMsgbuffer->SingleWrite(0x300a+ FPGAOffset *i, 0x0018);
          //L1 Latency
        fpMsgbuffer->SingleWrite( BbTrmL1LAT + FPGAOffset *i, 0x2104);
        //Setup Busy Box
          //Number of multi-event buffers = 1
        //fpMsgbuffer->SingleWrite(0x2009+ FPGAOffset *i, 0x0001);
          //Disable all inputs
        fpMsgbuffer->MultipleWrite(0x2100+ FPGAOffset *i, data, 128);
          //Write channels to enable
        fpMsgbuffer->MultipleWrite(0x2100+ FPGAOffset *i, ((uint32_t*)pData)+128*i, 128);
          //halt the state-machine of the BB
        fpMsgbuffer->SingleWrite(0x200a+ FPGAOffset *i, 0x0001);
          //pop event ID from the D-RORC FIFO
        for(uint32_t j=0;j<8;j++) fpMsgbuffer->SingleWrite(0x0001+ FPGAOffset *i, 0xff60);
          //set request ID to 0
        for(uint32_t j=0;j<3;j++) fpMsgbuffer->SingleWrite(0x0001+ FPGAOffset *i, 0xff70);
          //resume the state-machine operation
        fpMsgbuffer->SingleWrite(0x200a+ FPGAOffset *i, 0x0000);
      }
    }
    break;
  default:
    CE_Warning("unrecognized command id (%#x)\n", cmd);
    return -ENOSYS;
  }
  return size;
}
int DevBb::HighLevelHandler(const char* pCommand, std::vector<uint32_t>& rb){
  int keySize=0;
  uint32_t cmd=0;
  if(!pCommand) return 0;
  if      (strncmp(pCommand, "BB_WRITE_TX"   , keySize=strlen("BB_WRITE_TX"   ))==0) cmd=BB_WRITE_TX;
  else if (strncmp(pCommand, "BB_WRITE_L0TTO", keySize=strlen("BB_WRITE_L0TTO"))==0) cmd=BB_WRITE_L0TTO;
  else if (strncmp(pCommand, "BB_WRITE_FEEBA", keySize=strlen("BB_WRITE_FEEBA"))==0) cmd=BB_WRITE_FEEBA;
  else if (strncmp(pCommand, "BB_WRITE_FSMH" , keySize=strlen("BB_WRITE_FSMH" ))==0) cmd=BB_WRITE_FSMH;
  else if (strncmp(pCommand, "BB_WRITE_FM"   , keySize=strlen("BB_WRITE_FM"   ))==0) cmd=BB_WRITE_FM;
  else if (strncmp(pCommand, "BB_WRITE_RRTO" , keySize=strlen("BB_WRITE_RRTO" ))==0) cmd=BB_WRITE_RRTO;
  else if (strncmp(pCommand, "BB_WRITE_RXMF" , keySize=strlen("BB_WRITE_RXMF" ))==0) cmd=BB_WRITE_RXMF;
  else if (strncmp(pCommand, "BB_WRITE_TRIGMODE" , keySize=strlen("BB_WRITE_TRIGMODE" ))==0) cmd=BB_WRITE_TRIGMODE;
  else if (strncmp(pCommand, "BB_WRITE_TTC_CONTROL0" , keySize=strlen("BB_WRITE_TTC_CONTROL0" ))==0) cmd=BB_WRITE_TTC_CONTROL0;
  else if (strncmp(pCommand, "BB_WRITE_TTC_CONTROL1" , keySize=strlen("BB_WRITE_TTC_CONTROL1" ))==0) cmd=BB_WRITE_TTC_CONTROL1;
  else if (strncmp(pCommand, "BB_WRITE_TTC_TMR"  , keySize=strlen("BB_WRITE_TTC_TMR"  ))==0) cmd=BB_WRITE_TTC_TMR;
  else if (strncmp(pCommand, "BB_WRITE_TTC_L0L"  , keySize=strlen("BB_WRITE_TTC_L0L"  ))==0) cmd=BB_WRITE_TTC_L0L;
  else if (strncmp(pCommand, "BB_WRITE_TTC_L1L"  , keySize=strlen("BB_WRITE_TTC_L1L"  ))==0) cmd=BB_WRITE_TTC_L1L;
  else if (strncmp(pCommand, "BB_WRITE_TTC_BCIDO", keySize=strlen("BB_WRITE_TTC_BCIDO"))==0) cmd=BB_WRITE_TTC_BCIDO;
  else if (strncmp(pCommand, "BB_FIRMWARE_RESET", keySize=strlen("BB_FIRMWARE_RESET"))==0) cmd=BB_FIRMWARE_RESET;
  else if (strncmp(pCommand, "BB_INIT", keySize=strlen("BB_INIT"))==0) cmd=BB_INIT;
  else if (strncmp(pCommand, "BB_WRITE_STATUS", keySize=strlen("BB_WRITE_STATUS"))==0) cmd=BB_WRITE_STATUS;

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
  switch(cmd){
  default:
    sscanf(pBuffer, "%d", &data);
    pPayload=(const char*)&data;
    iPayloadSize=sizeof(data);
    parameter=data;
    break;
  }
  return issue(cmd, parameter, pPayload, iPayloadSize, rb)<=0;
}
int DevBb::GetGroupId(){
  return FEESVR_CMD_BB;
}

