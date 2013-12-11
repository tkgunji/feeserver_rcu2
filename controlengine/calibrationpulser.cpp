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

#include <cstdlib>
#include <typeinfo>
#include <cstdio>
#include "calibrationpulser.hpp"
#include "dcb.hpp"
#include "ser.hpp"

/******************************************************************************/

void ce_exec_loop(){
  CE_Event("This is ControlEngine for CalibrationPulser\n");
  if(!Ctr::fpInstance) Ctr::fpInstance=new CtrCpulser();
  if(Ctr::fpInstance){
    CE_Event("ControlEngine '%s' (%i) of type '%s' created at %p\n", Ctr::fpInstance->GetName().c_str(), Ctr::fpInstance->GetDeviceId(), typeid(*Ctr::fpInstance).name(), Ctr::fpInstance);
    Ctr::fpInstance->RunCE();
  }
  else CE_Warning("ControlEngine not created at %p\n", Ctr::fpInstance);
  return;
}

/******************************************************************************/

CtrCpulser::CtrCpulser(): fpCpulser(NULL){
  if     (!fpMsgbuffer && fHwType==1) fpMsgbuffer=new DevMsgbufferDcb();
  else if(!fpMsgbuffer && fHwType==2) fpMsgbuffer=new DevMsgbufferDummy();
  if(fpMsgbuffer) CE_Event("DevMsgbuffer of type '%s' created at %p with HwType %i\n", typeid(*fpMsgbuffer).name(), fpMsgbuffer, fHwType);
  else CE_Warning("DevMsgbuffer not created at %p with HwType %i\n", fpMsgbuffer, fHwType);
  if(!fpCpulser && fpMsgbuffer) fpCpulser=new DevCpulser(subDevices.size(), this, fpMsgbuffer);
  if(fpCpulser){
    AddSubDevice(fpCpulser);
    CE_Event("DevCpulser '%s' (%i) of type '%s' created at %p with DevMsgbuffer at %p\n", fpCpulser->GetName().c_str(), fpCpulser->GetDeviceId(), typeid(*fpCpulser).name(), fpCpulser, fpMsgbuffer);
  }
  else CE_Warning("DevCpulser not created at %p with DevMsgbuffer %p\n", fpCpulser, fpMsgbuffer);
}

CtrCpulser::~CtrCpulser(){
  fpMsgbuffer=NULL; // deleted in sub-device handling
  fpCpulser=NULL;
}

/******************************************************************************/

DevCpulser::DevCpulser(int id,  CEStateMachine* pParent, DevMsgbuffer* msgbuffer):Dev("CPULSER", id, pParent),fpMsgbuffer(msgbuffer){
  AddTransition(new CETransition(kActionGoStandby, kStateStandby, std::string(kActionGoStandby_name), NULL, NULL, 2, kStateError, kStateRunning));
  AddTransition(new CETransition(kActionGoConfigure, kStateConfigured, std::string(kActionGoConfigure_name), NULL, NULL, 2, kStateStandby, kStateRunning));
  AddTransition(new CETransition(kActionStart, kStateRunning, std::string(kActionStart_name), NULL, NULL, 2, kStateConfigured, kStateStandby));
  AddTransition(new CETransition(kActionStop, kStateConfigured, std::string(kActionStop_name), NULL, NULL, 1, kStateRunning));
}

int DevCpulser::SynchronizeLocal(){
  if (!fpMsgbuffer) return kStateError;
  return kStateRunning;
}

int DevCpulser::ReSynchronizeLocal(){
  return GetCurrentState();
}

int DevCpulser::ArmorLocal(){
  int iResult = 0;

  std::string name=GetName();
  name+="_";

  fServices.push_back(new SerMbAddrS(name+"CONTROL"           , Control             , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"FWVERSION"         , PulserFW            , 1, 0, fpMsgbuffer));
  //  fServices.push_back(new SerMbAddrS(name+"FINEDELAY"         , FineDelay           , 1, 0, fpMsgbuffer));
  // fServices.push_back(new SerMbAddrS(name+"CHANNEL0"          , Channel0            , 1, 0, fpMsgbuffer));
  // fServices.push_back(new SerMbAddrS(name+"CHANNEL1"          , Channel1            , 1, 0, fpMsgbuffer));
    fServices.push_back(new SerMbAddrS(name+"COARSEDELAY"       , CoarseDelay         , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"AMPLITUDE"         , Amplitude           , 1, 0, fpMsgbuffer));
  // fServices.push_back(new SerMbAddrS(name+"PULSESHAPE"        , PulseShape          , 1, 0, fpMsgbuffer));
  //  fServices.push_back(new SerMbAddrS(name+"INSTRUCTIONREG"    , InstructionRegister , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"ISSUEDTRIGGERS"    , IssuedTriggers      , 1, 0, fpMsgbuffer));
//  fServices.push_back(new SerMbAddrS(name+"PULSERCONF"        , PulserConf          , 1, 0, fpMsgbuffer));

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

int DevCpulser::issue(uint32_t cmd, uint32_t parameter, const char* pData, int iDataSize, std::vector<uint32_t>& rb){
  int iResult = 0;
  CE_Debug("DevCpulser cmd=%#x parameter=%#x datasize=%d\n", cmd, parameter, iDataSize);
  switch (cmd) {
  case CPULSER_WRITE_CONTROL:
    fpMsgbuffer->SingleWrite(Control,*(uint32_t*)pData);
    iResult=4;
    break;
  case CPULSER_WRITE_FINEDELAY:
    fpMsgbuffer->SingleWrite(FineDelay,*(uint32_t*)pData);
    iResult=4;
    break;
  case CPULSER_WRITE_COARSEDELAY:
    fpMsgbuffer->SingleWrite(CoarseDelay,*(uint32_t*)pData);
    iResult=4;
    break;
  case CPULSER_WRITE_CHANNEL0:
    fpMsgbuffer->SingleWrite(Channel0,*(uint32_t*)pData);
    iResult=4;
    break;
  case CPULSER_WRITE_CHANNEL1:
    fpMsgbuffer->SingleWrite(Channel1,*(uint32_t*)pData);
    iResult=4;
    break;
  case CPULSER_WRITE_AMPLITUDE:
    fpMsgbuffer->SingleWrite(Amplitude,*(uint32_t*)pData);
    iResult=4;
    break;
  case CPULSER_WRITE_PULSESHAPE:
    fpMsgbuffer->SingleWrite(PulseShape,*(uint32_t*)pData);
    iResult=4;
    break;
  case CPULSER_WRITE_PULSERCONF:
    fpMsgbuffer->SingleWrite(PulserConf,*(uint32_t*)pData);
    iResult=4;
    break;
  case CPULSER_WRITE_INSTRUCTIONREG:
    fpMsgbuffer->SingleWrite(InstructionRegister,*(uint32_t*)pData);
    iResult=4;
    break;
  case CPULSER_TOGGLE_REMOTERESET:
    iResult=fpMsgbuffer->SingleWrite(RemotkActionReset,0xfffff);
    // When a read command is performed (service update) short after the remote reset command
    // the firmware will switch to "local" control. This timeout is there to avoid this.
    sleep(10);
    iResult=0;
    break;
  case CPULSER_TOGGLE_ALLCHANNELSON:
    iResult=fpMsgbuffer->SingleWrite(AllChannelsOn,0xfffff);
    iResult=0;
    break;
  case CPULSER_TOGGLE_ALLCHANNELSOFF:
    iResult=fpMsgbuffer->SingleWrite(AllChannelsOff,0xfffff);
    iResult=0;
    break;
  case CPULSER_TOGGLE_REMOTE:
    iResult=fpMsgbuffer->SingleWrite(SwitchToRemote,0xfffff);
    iResult=0;
    break;
  case CPULSER_TOGGLE_LOCAL:
    iResult=fpMsgbuffer->SingleWrite(SwitchToLocal,0xfffff);
    iResult=0;
    break;
  case CPULSER_TOGGLE_GLOBALRESET:
    iResult=fpMsgbuffer->SingleWrite(GlobalReset,0x0);
    iResult=0;
    break;
  case CPULSER_TOGGLE_TESTTRIGGER:
    iResult=fpMsgbuffer->SingleWrite(TestTrigger,0x0);
    iResult=0;
    break;
  case CPULSER_WRITE_TTC_CONTROL:
    fpMsgbuffer->SingleWrite(TTCControl,*(uint32_t*)pData);
    iResult=4;
    break;
  case CPULSER_TOGGLE_TTC_RESET:
    iResult=fpMsgbuffer->SingleWrite(TTCModulkActionReset,0x0);
    iResult=0;
    break;
  case CPULSER_WRITE_TTC_ROICONFIG1:
    fpMsgbuffer->SingleWrite(TTCROIConfig1,*(uint32_t*)pData);
    iResult=4;
    break;
  case CPULSER_WRITE_TTC_ROICONFIG2:
    fpMsgbuffer->SingleWrite(TTCROIConfig2,*(uint32_t*)pData);
    iResult=4;
    break;
  case CPULSER_TOGGLE_TTC_RESETCOUNTER:
    iResult=fpMsgbuffer->SingleWrite(TTCResetCounters,0x0);
    iResult=0;
    break;
  case CPULSER_TOGGLE_TTC_ISSUETESTMODE:
    iResult=fpMsgbuffer->SingleWrite(TTCIssueTestmode,0x0);
    iResult=0;
    break;
  case CPULSER_WRITE_TTC_L1LATENCY:
    fpMsgbuffer->SingleWrite(TTCL1Latency,*(uint32_t*)pData);
    iResult=4;
    break;
  case CPULSER_WRITE_TTC_L2LATENCY:
    fpMsgbuffer->SingleWrite(TTCL2Latency,*(uint32_t*)pData);
    iResult=4;
    break;
  case CPULSER_WRITE_TTC_ROILATENCY:
    fpMsgbuffer->SingleWrite(TTCRoILatency,*(uint32_t*)pData);
    iResult=4;
    break;
  case CPULSER_WRITE_TTC_L1MSGLATENCY:
    fpMsgbuffer->SingleWrite(TTCL1MsgLatency,*(uint32_t*)pData);
    iResult=4;
    break;
  case CPULSER_ARM_SYNC:
    fpMsgbuffer->SingleWrite(ArmSync,0xffffffff);
    iResult=4;
    break;
  default:
    CE_Debug("CECpulser: unrecognized command id (%#x)\n", cmd);
    iResult=-ENOSYS;
  }
  return iResult;
}


int DevCpulser::HighLevelHandler(const char* pCommand, std::vector<uint32_t>& rb){
  int iResult=0;
  uint32_t cmd=0;
  int keySize=0;
  int len=strlen(pCommand);
  const char* pBuffer=pCommand;
  if (pCommand) {
    //CE_Debug("CECpulser checks command %s\n", pCommand);
    if (strncmp(pCommand, "CPULSER_WRITE_CONTROL", keySize=strlen("CPULSER_WRITE_CONTROL"))==0)
      cmd=CPULSER_WRITE_CONTROL;
    else if (strncmp(pCommand, "CPULSER_WRITE_FINEDELAY", keySize=strlen("CPULSER_WRITE_FINEDELAY"))==0)
      cmd=CPULSER_WRITE_FINEDELAY;
    else if (strncmp(pCommand, "CPULSER_WRITE_CHANNEL0", keySize=strlen("CPULSER_WRITE_CHANNEL0"))==0)
      cmd=CPULSER_WRITE_CHANNEL0;
    else if (strncmp(pCommand, "CPULSER_WRITE_CHANNEL1", keySize=strlen("CPULSER_WRITE_CHANNEL1"))==0)
      cmd=CPULSER_WRITE_CHANNEL1;
    else if (strncmp(pCommand, "CPULSER_WRITE_COARSEDELAY", keySize=strlen("CPULSER_WRITE_COARSEDELAY"))==0)
      cmd=CPULSER_WRITE_COARSEDELAY;
    else if (strncmp(pCommand, "CPULSER_WRITE_AMPLITUDE", keySize=strlen("CPULSER_WRITE_AMPLITUDE"))==0)
      cmd=CPULSER_WRITE_AMPLITUDE;
    else if (strncmp(pCommand, "CPULSER_WRITE_PULSESHAPE", keySize=strlen("CPULSER_WRITE_PULSESHAPE"))==0)
      cmd=CPULSER_WRITE_PULSESHAPE;
    else if (strncmp(pCommand, "CPULSER_WRITE_INSTRUCTIONREG", keySize=strlen("CPULSER_WRITE_INSTRUCTIONREG"))==0)
      cmd=CPULSER_WRITE_INSTRUCTIONREG;
    else if (strncmp(pCommand, "CPULSER_WRITE_PULSERCONF", keySize=strlen("CPULSER_WRITE_PULSERCONF"))==0)
      cmd=CPULSER_WRITE_PULSERCONF;
    else if (strncmp(pCommand, "CPULSER_TOGGLE_LOCAL", keySize=strlen("CPULSER_TOGGLE_LOCAL"))==0)
      cmd=CPULSER_TOGGLE_LOCAL;
    else if (strncmp(pCommand, "CPULSER_TOGGLE_ALLCHANNELSOFF", keySize=strlen("CPULSER_TOGGLE_ALLCHANNELSOFF"))==0)
      cmd=CPULSER_TOGGLE_ALLCHANNELSOFF;
    else if (strncmp(pCommand, "CPULSER_TOGGLE_ALLCHANNELSON", keySize=strlen("CPULSER_TOGGLE_ALLCHANNELSON"))==0)
      cmd=CPULSER_TOGGLE_ALLCHANNELSON;
    else if (strncmp(pCommand, "CPULSER_TOGGLE_REMOTERESET", keySize=strlen("CPULSER_TOGGLE_REMOTERESET"))==0)
      cmd=CPULSER_TOGGLE_REMOTERESET;
    else if (strncmp(pCommand, "CPULSER_TOGGLE_REMOTE", keySize=strlen("CPULSER_TOGGLE_REMOTE"))==0)
      cmd=CPULSER_TOGGLE_REMOTE;
    else if (strncmp(pCommand, "CPULSER_TOGGLE_TESTTRIGGER", keySize=strlen("CPULSER_TOGGLE_TESTTRIGGER"))==0)
      cmd=CPULSER_TOGGLE_TESTTRIGGER;
    else if (strncmp(pCommand, "CPULSER_TOGGLE_GLOBALRESET", keySize=strlen("CPULSER_TOGGLE_GLOBALRESET"))==0)
      cmd=CPULSER_TOGGLE_GLOBALRESET;
    else if (strncmp(pCommand, "CPULSER_WRITE_TTC_CONTROL", keySize=strlen("CPULSER_WRITE_TTC_CONTROL"))==0)
      cmd=CPULSER_WRITE_TTC_CONTROL;
    else if (strncmp(pCommand, "CPULSER_TOGGLE_TTC_RESET", keySize=strlen("CPULSER_TOGGLE_TTC_RESET"))==0)
      cmd=CPULSER_TOGGLE_TTC_RESET;
    else if (strncmp(pCommand, "CPULSER_WRITE_TTC_ROICONFIG1", keySize=strlen("CPULSER_WRITE_TTC_ROICONFIG1"))==0)
      cmd=CPULSER_WRITE_TTC_ROICONFIG1;
    else if (strncmp(pCommand, "CPULSER_WRITE_TTC_ROICONFIG2", keySize=strlen("CPULSER_WRITE_TTC_ROICONFIG2"))==0)
      cmd=CPULSER_WRITE_TTC_ROICONFIG2;
    else if (strncmp(pCommand, "CPULSER_TOGGLE_TTC_RESETCOUNTER", keySize=strlen("CPULSER_TOGGLE_TTC_RESETCOUNTER"))==0)
      cmd=CPULSER_TOGGLE_TTC_RESETCOUNTER;
    else if (strncmp(pCommand, "CPULSER_TOGGLE_TTC_ISSUETESTMODE", keySize=strlen("CPULSER_TOGGLE_TTC_ISSUETESTMODE"))==0)
      cmd=CPULSER_TOGGLE_TTC_ISSUETESTMODE;
    else if (strncmp(pCommand, "CPULSER_WRITE_TTC_L2LATENCY", keySize=strlen("CPULSER_WRITE_TTC_L2LATENCY"))==0)
      cmd=CPULSER_WRITE_TTC_L2LATENCY;
    else if (strncmp(pCommand, "CPULSER_WRITE_TTC_L1LATENCY", keySize=strlen("CPULSER_WRITE_TTC_L1LATENCY"))==0)
      cmd=CPULSER_WRITE_TTC_L1LATENCY;
    else if (strncmp(pCommand, "CPULSER_WRITE_TTC_ROILATENCY", keySize=strlen("CPULSER_WRITE_TTC_ROILATENCY"))==0)
      cmd=CPULSER_WRITE_TTC_ROILATENCY;
    else if (strncmp(pCommand, "CPULSER_WRITE_TTC_L1MSGLATENCY", keySize=strlen("CPULSER_WRITE_TTC_L1MSGLATENCY"))==0)
      cmd=CPULSER_WRITE_TTC_L1MSGLATENCY;
    else if (strncmp(pCommand, "CPULSER_ARM_SYNC", keySize=strlen("CPULSER_ARM_SYNC"))==0)
      cmd=CPULSER_ARM_SYNC;
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

int DevCpulser::GetGroupId(){
  return FEESVR_CMD_CPULSER;
}

