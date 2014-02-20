/************************************************************************
** This file is property of and copyright by the Experimental Nuclear 
** Physics Group, Dep. of Physics and Technology
** University of Bergen, Norway, 2009
** This file has been written by Dominik Fehlker
** Please report bugs to Dominik.Fehlker@uib.no
**
** Permission to use, copy, modify and distribute this software and its  
** documentation strictly for non-commercial purposes is hereby granted  
** without fee, provided that the above copyright notice appears in all  
** copies and that both the copyright notice and this permission notice  
** appear in the supporting documentation. The authors make no claims    
** about the suitability of this software for any purpose. It is         
** provided "as is" without express or implied warranty.                 
**  
*************************************************************************/

#include <cerrno>
#include <cstdlib>
#include <typeinfo>
#include <cstdio>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "gatepulser.hpp"
#include "rcu2_issue.h"
#include "dcb.hpp"
#include "ser.hpp"

/******************************************************************************/

void ce_exec_loop(){
  CE_Event("This is ControlEngine for GatePulser\n");
  if(!Ctr::fpInstance) Ctr::fpInstance=new CtrGpulser();
  if(Ctr::fpInstance){
    CE_Event("ControlEngine '%s' (%i) of type '%s' created at %p\n", Ctr::fpInstance->GetName().c_str(), Ctr::fpInstance->GetDeviceId(), typeid(*Ctr::fpInstance).name(), Ctr::fpInstance);
    Ctr::fpInstance->RunCE();
  }
  else CE_Warning("ControlEngine not created at %p\n", Ctr::fpInstance);
  return;
}

/******************************************************************************/

CtrGpulser::CtrGpulser(): fpGpulser(NULL){
  if     (!fpMsgbuffer && fHwType==1) fpMsgbuffer=new DevMsgbufferDcb();
  else if(!fpMsgbuffer && fHwType==2) fpMsgbuffer=new DevMsgbufferDummy();
  if(fpMsgbuffer) CE_Event("DevMsgbuffer of type '%s' created at %p with HwType %i\n", typeid(*fpMsgbuffer).name(), fpMsgbuffer, fHwType);
  else CE_Warning("DevMsgbuffer not created at %p with HwType %i\n", fpMsgbuffer, fHwType);
  if(!fpGpulser && fpMsgbuffer) fpGpulser=new DevGpulser(subDevices.size(), this, fpMsgbuffer);
  if(fpGpulser){
    AddSubDevice(fpGpulser);
    CE_Event("DevGpulser '%s' (%i) of type '%s' created at %p with DevMsgbuffer at %p\n", fpGpulser->GetName().c_str(), fpGpulser->GetDeviceId(), typeid(*fpGpulser).name(), fpGpulser, fpMsgbuffer);
  }
  else CE_Warning("DevGpulser not created at %p with DevMsgbuffer %p\n", fpGpulser, fpMsgbuffer);
  signal(SIGUSR1, InterruptHandler); // signal handler registered    
  CE_Debug("Interrupt handler registered for interrupt %i\n", SIGUSR1);
  fp=open("/dev/irq/irq", O_RDWR); // open IRQ driver
  if(fp==-1) CE_Debug("Error on opening %d  .. exiting\n", errno);
}

CtrGpulser::~CtrGpulser(){
  fpMsgbuffer=NULL; // deleted in sub-device handling
  fpGpulser=NULL;
  if(fp) close(fp);
  fp=0;
}

void CtrGpulser::InterruptHandler(int sig){
  CE_Debug("Caught interrupt %i\n", sig);
}

/******************************************************************************/

DevGpulser::DevGpulser(int id,  CEStateMachine* pParent, DevMsgbuffer* msgbuffer):Dev("GPULSER", id, pParent),fpMsgbuffer(msgbuffer){
  AddTransition(new CETransition(kActionGoStandby, kStateStandby, std::string(kActionGoStandby_name), NULL, NULL, 5, kStateError, kStateConfigured, kStatePulserL0, kStatePulserL1, kStatePulserOff));
  AddTransition(new CETransition(kActionGoPulserL0, kStatePulserL0, std::string(kActionGoPulserL0_name), NULL, NULL, 3, kStateConfigured, kStatePulserL1, kStatePulserOff));
  AddTransition(new CETransition(kActionGoPulserL1, kStatePulserL1, std::string(kActionGoPulserL1_name), NULL, NULL, 3, kStateConfigured, kStatePulserL0, kStatePulserOff));
  AddTransition(new CETransition(kActionGoPulserOff, kStatePulserOff, std::string(kActionGoPulserOff_name), NULL, NULL, 3, kStateConfigured, kStatePulserL0, kStatePulserL1));
  AddTransition(new CETransition(kActionGoStbyConf, kStateConfigured, std::string(kActionGoStbyConf_name), NULL, NULL, 3, kStatePulserL0, kStatePulserL1, kStatePulserOff));
  AddTransition(new CETransition(kActionLoadRecipe, kStateConfigured, std::string(kActionLoadRecipe_name), NULL, NULL, 1, kStateStandby));
}

int DevGpulser::SynchronizeLocal(){
  if(!fpMsgbuffer) return kStateError;
  return kStateConfigured;
}

int DevGpulser::ReSynchronizeLocal(){
  return GetCurrentState();
}

int DevGpulser::ArmorLocal(){
  int iResult = 0;

  std::string name=GetName();
  name+="_";

  fServices.push_back(new SerMbAddrS(name+"CONFREG"           , ConfReg           , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"FWVERSION"         , RCUVersion        , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"PULSESTATUS"       , PulseStatus       , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"PULSECOUNTER"      , PulseCounter      , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"MASKCRATE1"        , MaskCrate1          , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"MASKCRATE2"        , MaskCrate2          , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"MASKCRATE3"        , MaskCrate3          , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"MASKCRATE4"        , MaskCrate4          , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"MASKCRATE5"        , MaskCrate5          , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"MASKCRATE6"        , MaskCrate6          , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"OPENFRAMECRATE1"   , OpenFrameCrate1          , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"OPENFRAMECRATE2"   , OpenFrameCrate2          , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"OPENFRAMECRATE3"   , OpenFrameCrate3          , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"OPENFRAMECRATE4"   , OpenFrameCrate4          , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"OPENFRAMECRATE5"   , OpenFrameCrate5          , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"OPENFRAMECRATE6"   , OpenFrameCrate6          , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"CLOSEFRAMECRATE1"  , CloseFrameCrate1          , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"CLOSEFRAMECRATE2"  , CloseFrameCrate2          , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"CLOSEFRAMECRATE3"  , CloseFrameCrate3          , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"CLOSEFRAMECRATE4"  , CloseFrameCrate4          , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"CLOSEFRAMECRATE5"  , CloseFrameCrate5          , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"CLOSEFRAMECRATE6"  , CloseFrameCrate6          , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"ERRORMONITOR"      , ErrorMonitor          , 1, 0, fpMsgbuffer));


  //Trigger Interface services
  
  fServices.push_back(new SerMbAddrS(name+"TTC_CONTROL"       , TTCControl        , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"TTC_ROICONFIG1"    , TTCROIConfig1     , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"TTC_ROICONFIG2"    , TTCROIConfig2     , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"TTC_L1LATENCY"     , TTCL1Latency      , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"TTC_L2LATENCY"     , TTCL2Latency      , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"TTC_ROILATENCY"    , TTCRoILatency     , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"TTC_L1MSGLATENCY"  , TTCL1MsgLatency   , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"TTC_PREPULSECNT"   , TTCPrePulseCnt    , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"TTC_BCIDLOCAL"     , TTCBCIDLocal      , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"TTC_L0COUNTER"     , TTCL0Counter      , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"TTC_L1COUNTER"     , TTCL1Counter      , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"TTC_L1MSGCOUNTER"  , TTCL1MsgCounter   , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"TTC_L2ACOUNTER"    , TTCL2aCounter     , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"TTC_L2RCOUNTER"    , TTCL2rCounter     , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"TTC_ROICOUNTER"    , TTCRoICounter     , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"TTC_HAMMINGERRCNT" , TTCHammingErrorCnt, 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"TTC_ERRORCNT"      , TTCErrorCnt       , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"TTC_BUFFEREDEVENTS", TTCBufferedEvents , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"TTC_DAQHEADER1"    , TTCDAQHeader1     , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"TTC_DAQHEADER2"    , TTCDAQHeader2     , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"TTC_DAQHEADER3"    , TTCDAQHeader3     , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"TTC_DAQHEADER4"    , TTCDAQHeader4     , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"TTC_DAQHEADER5"    , TTCDAQHeader5     , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"TTC_DAQHEADER6"    , TTCDAQHeader6     , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"TTC_DAQHEADER7"    , TTCDAQHeader7     , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"TTC_EVENTINFO"     , TTCEventInfo      , 1, 0, fpMsgbuffer));
  return iResult;
}

int DevGpulser::EnterStatePULSER_L0(){
  int iResult = 0;

  iResult = WriteMode(0x0);

  return iResult;
}

int DevGpulser::EnterStatePULSER_L1(){
  int iResult = 0;

  iResult = WriteMode(0x1);

  return iResult;
}

int DevGpulser::LeaveStatePULSER_L1(){
  int iResult = 0;

  iResult = WriteMode(0x0);

  return iResult;
}

int DevGpulser::EnterStatePULSER_OFF(){
  int iResult = 0;
  uint32_t u32rawData = 0;
  uint32_t u32newData = 0;

  fpMsgbuffer->SingleRead(ConfReg, &u32rawData);
  u32newData = u32rawData & ~ConfRegMaskEnable;
  fpMsgbuffer->SingleWrite(ConfReg,u32newData);

  return iResult;
}

int DevGpulser::LeaveStatePULSER_OFF(){
  int iResult = 0;
  uint32_t u32rawData = 0;
  uint32_t u32newData = 0;

  fpMsgbuffer->SingleRead(ConfReg, &u32rawData);
  u32newData = u32rawData | ConfRegMaskEnable;
  fpMsgbuffer->SingleWrite(ConfReg,u32newData);

  return iResult;
}

int DevGpulser::issue(uint32_t cmd, uint32_t parameter, const char* pData, int iDataSize, std::vector<uint32_t>& rb){
  int iResult = 0;
  CE_Debug("DevGpulser::TranslateGpulserCommand cmd=%#x parameter=%#x datasize=%d\n", cmd, parameter, iDataSize);
  switch (cmd) {
  case GPULSER_WRITE_CONFREG:
    fpMsgbuffer->SingleWrite(ConfReg,*(uint32_t*)pData);
    iResult=4;
    break;
  case GPULSER_TOGGLE_RESET:
    iResult=fpMsgbuffer->SingleWrite(RestTestReg,0x0);
    iResult=0;
    break;
  case GPULSER_TOGGLE_GLOBAL_RESET:
    iResult=fpMsgbuffer->SingleWrite(GlobalRest,0x0);
    iResult=0;
    break;
  case GPULSER_WRITE_TTC_CONTROL:
    fpMsgbuffer->SingleWrite(TTCControl,*(uint32_t*)pData);
    iResult=4;
    break;
  case GPULSER_TOGGLE_TTC_RESET:
    iResult=fpMsgbuffer->SingleWrite(TTCModulkActionReset,0x0);
    iResult=0;
    break;
  case GPULSER_WRITE_TTC_ROICONFIG1:
    fpMsgbuffer->SingleWrite(TTCROIConfig1,*(uint32_t*)pData);
    iResult=4;
    break;
  case GPULSER_WRITE_TTC_ROICONFIG2:
    fpMsgbuffer->SingleWrite(TTCROIConfig2,*(uint32_t*)pData);
    iResult=4;
    break;
  case GPULSER_TOGGLE_TTC_RESETCOUNTER:
    iResult=fpMsgbuffer->SingleWrite(TTCResetCounters,0x0);
    iResult=0;
    break;
  case GPULSER_TOGGLE_TTC_ISSUETESTMODE:
    iResult=fpMsgbuffer->SingleWrite(TTCIssueTestmode,0x0);
    iResult=0;
    break;
  case GPULSER_WRITE_TTC_L1LATENCY:
    fpMsgbuffer->SingleWrite(TTCL1Latency,*(uint32_t*)pData);
    iResult=4;
    break;
  case GPULSER_WRITE_TTC_L2LATENCY:
    fpMsgbuffer->SingleWrite(TTCL2Latency,*(uint32_t*)pData);
    iResult=4;
    break;
  case GPULSER_WRITE_TTC_ROILATENCY:
    fpMsgbuffer->SingleWrite(TTCRoILatency,*(uint32_t*)pData);
    iResult=4;
    break;
  case GPULSER_WRITE_TTC_L1MSGLATENCY:
    fpMsgbuffer->SingleWrite(TTCL1MsgLatency,*(uint32_t*)pData);
    iResult=4;
    break;
  case GPULSER_ARM_SYNC:
    fpMsgbuffer->SingleWrite(ARM_SYNCH,0x0);
    iResult=0;
    break;
  case GPULSER_WRITE_DIVRATIO:
    WriteDivRatio(*(uint32_t*)pData);
    iResult = 4;
    break;
  case GPULSER_WRITE_MODE:
    WriteMode(*(uint32_t*)pData);
    iResult = 4;
    break;
  case GPULSER_WRITE_PULSEWIDTH:
    WritePulseWidth(*(uint32_t*)pData);
    iResult = 4;
    break;
  case GPULSER_WRITE_MASKCRATE1:
    fpMsgbuffer->SingleWrite(MaskCrate1,*(uint32_t*)pData);
    iResult=4;
    break;
  case GPULSER_WRITE_MASKCRATE2:
    fpMsgbuffer->SingleWrite(MaskCrate2,*(uint32_t*)pData);
    iResult=4;
    break;
  case GPULSER_WRITE_MASKCRATE3:
    fpMsgbuffer->SingleWrite(MaskCrate3,*(uint32_t*)pData);
    iResult=4;
    break;
  case GPULSER_WRITE_MASKCRATE4:
    fpMsgbuffer->SingleWrite(MaskCrate4,*(uint32_t*)pData);
    iResult=4;
    break;
  case GPULSER_WRITE_MASKCRATE5:
    fpMsgbuffer->SingleWrite(MaskCrate5,*(uint32_t*)pData);
    iResult=4;
    break;
  case GPULSER_WRITE_MASKCRATE6:
    fpMsgbuffer->SingleWrite(MaskCrate6,*(uint32_t*)pData);
    iResult=4;
    break;
  default:
    CE_Debug("CEgpulser: unrecognized command id (%#x)\n", cmd);
    iResult=-ENOSYS;
  }
  return iResult;
}

int DevGpulser::HighLevelHandler(const char* pCommand, std::vector<uint32_t>& rb){
  int iResult=0;
  uint32_t cmd=0;
  int keySize=0;
  int len=strlen(pCommand);
  const char* pBuffer=pCommand;
  if (pCommand) {
    //CE_Debug("CEgpulser checks command %s\n", pCommand);
    if (strncmp(pCommand, "GPULSER_WRITE_CONFREG", keySize=strlen("GPULSER_WRITE_CONFREG"))==0)
      cmd=GPULSER_WRITE_CONFREG;
    else if (strncmp(pCommand, "GPULSER_TOGGLE_RESET", keySize=strlen("GPULSER_TOGGLE_RESET"))==0)
      cmd=GPULSER_TOGGLE_RESET;
    else if (strncmp(pCommand, "GPULSER_TOGGLE_GLOBAL_RESET", keySize=strlen("GPULSER_TOGGLE_GLOBAL_RESET"))==0)
      cmd=GPULSER_TOGGLE_GLOBAL_RESET;
    else if (strncmp(pCommand, "GPULSER_WRITE_TTC_CONTROL", keySize=strlen("GPULSER_WRITE_TTC_CONTROL"))==0)
      cmd=GPULSER_WRITE_TTC_CONTROL;
    else if (strncmp(pCommand, "GPULSER_TOGGLE_TTC_RESET", keySize=strlen("GPULSER_TOGGLE_TTC_RESET"))==0)
      cmd=GPULSER_TOGGLE_TTC_RESET;
    else if (strncmp(pCommand, "GPULSER_WRITE_TTC_ROICONFIG1", keySize=strlen("GPULSER_WRITE_TTC_ROICONFIG1"))==0)
      cmd=GPULSER_WRITE_TTC_ROICONFIG1;
    else if (strncmp(pCommand, "GPULSER_WRITE_TTC_ROICONFIG2", keySize=strlen("GPULSER_WRITE_TTC_ROICONFIG2"))==0)
      cmd=GPULSER_WRITE_TTC_ROICONFIG2;
    else if (strncmp(pCommand, "GPULSER_TOGGLE_TTC_RESETCOUNTER", keySize=strlen("GPULSER_TOGGLE_TTC_RESETCOUNTER"))==0)
      cmd=GPULSER_TOGGLE_TTC_RESETCOUNTER;
    else if (strncmp(pCommand, "GPULSER_TOGGLE_TTC_ISSUETESTMODE", keySize=strlen("GPULSER_TOGGLE_TTC_ISSUETESTMODE"))==0)
      cmd=GPULSER_TOGGLE_TTC_ISSUETESTMODE;
    else if (strncmp(pCommand, "GPULSER_WRITE_TTC_L2LATENCY", keySize=strlen("GPULSER_WRITE_TTC_L2LATENCY"))==0)
      cmd=GPULSER_WRITE_TTC_L2LATENCY;
    else if (strncmp(pCommand, "GPULSER_WRITE_TTC_L1LATENCY", keySize=strlen("GPULSER_WRITE_TTC_L1LATENCY"))==0)
      cmd=GPULSER_WRITE_TTC_L1LATENCY;
    else if (strncmp(pCommand, "GPULSER_WRITE_TTC_ROILATENCY", keySize=strlen("GPULSER_WRITE_TTC_ROILATENCY"))==0)
      cmd=GPULSER_WRITE_TTC_ROILATENCY;
    else if (strncmp(pCommand, "GPULSER_WRITE_TTC_L1MSGLATENCY", keySize=strlen("GPULSER_WRITE_TTC_L1MSGLATENCY"))==0)
      cmd=GPULSER_WRITE_TTC_L1MSGLATENCY;
    else if (strncmp(pCommand, "GPULSER_ARM_SYNC", keySize=strlen("GPULSER_ARM_SYNC"))==0)
      cmd=GPULSER_ARM_SYNC;
    else if (strncmp(pCommand, "GPULSER_WRITE_DIVRATIO", keySize=strlen("GPULSER_WRITE_DIVRATIO"))==0)
      cmd=GPULSER_WRITE_DIVRATIO;
    else if (strncmp(pCommand, "GPULSER_WRITE_MODE", keySize=strlen("GPULSER_WRITE_MODE"))==0)
      cmd=GPULSER_WRITE_MODE;
    else if (strncmp(pCommand, "GPULSER_WRITE_PULSEWIDTH", keySize=strlen("GPULSER_WRITE_PULSEWIDTH"))==0)
      cmd=GPULSER_WRITE_PULSEWIDTH;
    else if (strncmp(pCommand, "GPULSER_WRITE_MASKCRATE1", keySize=strlen("GPULSER_WRITE_MASKCRATE1"))==0)
      cmd=GPULSER_WRITE_MASKCRATE1;
    else if (strncmp(pCommand, "GPULSER_WRITE_MASKCRATE2", keySize=strlen("GPULSER_WRITE_MASKCRATE2"))==0)
      cmd=GPULSER_WRITE_MASKCRATE2;
    else if (strncmp(pCommand, "GPULSER_WRITE_MASKCRATE3", keySize=strlen("GPULSER_WRITE_MASKCRATE3"))==0)
      cmd=GPULSER_WRITE_MASKCRATE3;
    else if (strncmp(pCommand, "GPULSER_WRITE_MASKCRATE4", keySize=strlen("GPULSER_WRITE_MASKCRATE4"))==0)
      cmd=GPULSER_WRITE_MASKCRATE4;
    else if (strncmp(pCommand, "GPULSER_WRITE_MASKCRATE5", keySize=strlen("GPULSER_WRITE_MASKCRATE5"))==0)
      cmd=GPULSER_WRITE_MASKCRATE5;
    else if (strncmp(pCommand, "GPULSER_WRITE_MASKCRATE6", keySize=strlen("GPULSER_WRITE_MASKCRATE6"))==0)
      cmd=GPULSER_WRITE_MASKCRATE6;
    if (cmd>0 && keySize>0) {
      pBuffer+=keySize;
      len-=keySize;
      while (*pBuffer==' ' && *pBuffer!=0 && len>0) {pBuffer++; len--;} // skip all blanks
      uint32_t parameter=0;
      uint32_t data=0;
      const char* pPayload=NULL;
      int iPayloadSize=0;
     
    
      if (sscanf(pBuffer, "0x%x", &data)>0) {
	pPayload=(const char*)&data;
	iPayloadSize=sizeof(data);
      }
      /**else {
	CE_Error("error scanning high-level command %s\n", pCommand);
	iResult=-EPROTO;
	}**/
      
      if (iResult>=0) {
	if ((iResult=issue(cmd, parameter, pPayload, iPayloadSize, rb))>=0) {
	  iResult=1;
	}
      }
    }
  } else {
    iResult=-EINVAL;
  }
  return iResult;  
}

int DevGpulser::WriteDivRatio(uint32_t parameter){
  int iResult = 0;
  uint32_t u32rawData = 0;
  uint32_t u32newData = 0;

  CE_Debug("parameter: %#x\n", parameter);

  if(3 >= parameter){
    parameter = parameter << 14;
    fpMsgbuffer->SingleRead(ConfReg, &u32rawData);
    u32newData = ((u32rawData &~ConfRegMaskClkDivRatio)|parameter);
    fpMsgbuffer->SingleWrite(ConfReg,u32newData);  
  }
  else{
    CE_Debug("CE_Gpulser::Parameter too big: %#x\n", parameter);
  }

  return iResult;
}

int DevGpulser::WriteMode(uint32_t parameter){
  int iResult = 0;
  uint32_t u32rawData = 0;
  uint32_t u32newData = 0;

  CE_Debug("parameter: %#x\n", parameter);

  if(1 >= parameter){
    parameter = parameter << 13;
    fpMsgbuffer->SingleRead(ConfReg, &u32rawData);
    u32newData = ((u32rawData & ~ConfRegMaskMode)|parameter);
    fpMsgbuffer->SingleWrite(ConfReg, u32newData);  
  }
  else{
    CE_Debug("CE_Gpulser::Parameter too big: %#x\n", parameter);
  }

  return iResult;
}

int DevGpulser::WritePulseWidth(uint32_t parameter){
  int iResult = 0;
  uint32_t u32rawData = 0;
  uint32_t u32newData = 0;

  if(ConfRegMaskPulseWidth >= parameter){
    fpMsgbuffer->SingleRead(ConfReg, &u32rawData);
    u32newData = ((u32rawData & ~ConfRegMaskPulseWidth)| parameter);
    fpMsgbuffer->SingleWrite(ConfReg,u32newData);
  }
  else{
    CE_Debug("CE_Gpulser::Parameter too big: %#x\n", parameter);
  }

  return iResult;
}


int DevGpulser::GetGroupId(){
  return FEESVR_CMD_GPULSER;
}

