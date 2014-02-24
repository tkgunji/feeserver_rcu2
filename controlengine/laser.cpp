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
#include "laser.hpp"
#include "rcu2_issue.h"
#include "dcb.hpp"
#include "ser.hpp"

/******************************************************************************/

void ce_exec_loop(){
  CE_Event("This is ControlEngine for Laser\n");
  if(!Ctr::fpInstance) Ctr::fpInstance=new CtrLaser();
  if(Ctr::fpInstance){
    CE_Event("ControlEngine '%s' (%i) of type '%s' created at %p\n", Ctr::fpInstance->GetName().c_str(), Ctr::fpInstance->GetDeviceId(), typeid(*Ctr::fpInstance).name(), Ctr::fpInstance);
    Ctr::fpInstance->RunCE();
  }
  else CE_Warning("ControlEngine not created at %p\n", Ctr::fpInstance);
  return;
}

/******************************************************************************/

CtrLaser::CtrLaser(): fpLaser(NULL){
  if     (!fpMsgbuffer && fHwType==1) fpMsgbuffer=new DevMsgbufferDcb();
  else if(!fpMsgbuffer && fHwType==2) fpMsgbuffer=new DevMsgbufferDummy();
  if(fpMsgbuffer) CE_Event("DevMsgbuffer of type '%s' created at %p with HwType %i\n", typeid(*fpMsgbuffer).name(), fpMsgbuffer, fHwType);
  else CE_Warning("DevMsgbuffer not created at %p with HwType %i\n", fpMsgbuffer, fHwType);
  if(!fpLaser && fpMsgbuffer) fpLaser=new DevLaser(subDevices.size(), this, fpMsgbuffer);
  if(fpLaser){
    AddSubDevice(fpLaser);
    CE_Event("DevLaser '%s' (%i) of type '%s' created at %p with DevMsgbuffer at %p\n", fpLaser->GetName().c_str(), fpLaser->GetDeviceId(), typeid(*fpLaser).name(), fpLaser, fpMsgbuffer);
  }
  else CE_Warning("DevLaser not created at %p with DevMsgbuffer %p\n", fpLaser, fpMsgbuffer);
}

CtrLaser::~CtrLaser(){
  fpMsgbuffer=NULL; // deleted in sub-device handling
  fpLaser=NULL;
}

uint32_t u32MaxEvents = 0;

/******************************************************************************/

DevLaser::DevLaser(int id, CEStateMachine* pParent, DevMsgbuffer* msgbuffer):Dev("LASER", id, pParent),fpMsgbuffer(msgbuffer){
  AddTransition(new CETransition(kActionGoStandby, kStateStandby, std::string(kActionGoStandby_name), NULL, NULL, 2, kStateError, kStateConfigured));
  AddTransition(new CETransition(kActionGoWarmUp, kStateWarmUp, std::string(kActionGoWarmUp_name), NULL, NULL, 1, kStateConfigured));
  AddTransition(new CETransition(kActionGoOnTrigger, kStateOnTrigger, std::string(kActionGoOnTrigger_name), NULL, NULL, 1, kStateWarmUp));
  AddTransition(new CETransition(kActionGoOnFreeRun, kStateOnFreeRun, std::string(kActionGoOnFreeRun_name), NULL, NULL, 1, kStateWarmUp));
  AddTransition(new CETransition(kActionGoOnLimited, kStateOnLimited, std::string(kActionGoOnLimited_name), NULL, NULL, 1, kStateWarmUp));
  AddTransition(new CETransition(kActionGoStbyConf, kStateConfigured, std::string(kActionGoStbyConf_name), NULL, NULL, 4, kStateWarmUp, kStateOnTrigger, kStateOnFreeRun, kStateOnLimited));
  AddTransition(new CETransition(kActionLoadRecipe, kStateConfigured, std::string(kActionLoadRecipe_name), NULL, NULL, 1, kStateStandby));
  AddTransition(new CETransition(kActionGoFlashStop, kStateFlashStop, std::string(kActionGoFlashStop_name), NULL, NULL, 4, kStateWarmUp, kStateOnTrigger, kStateOnFreeRun, kStateOnLimited));
  AddTransition(new CETransition(kActionRecover, kStateConfigured, std::string(kActionRecover_name), NULL, NULL, 1, kStateFlashStop));
}

int DevLaser::SynchronizeLocal(){
  /** Modification from 01.02.10, requested by Carsten **/
  /** Lasers will be continously enabled and switched on/off in the runmode register **/
  EnableGlobal();
  EnableLaser1();
  EnableLaser2();
  
  return kStateStandby;
}

int DevLaser::ReSynchronizeLocal(){
  CE_Debug("DevLaser::ReSynchronizeLocal\n");
  uint32_t u32rawData = 0;
  uint32_t u32rawLaserEvents = 0;
  int current=GetCurrentState();
  
  fpMsgbuffer->SingleRead(MonitoringRegister, &u32rawData);
  CE_Debug("DevLaser::u32rawData||MonitoringMaskFSMState: %#x, u32rawData:%#x\n", u32rawData & MonitoringMaskFSMState, u32rawData);       

  switch (u32rawData & MonitoringMaskFSMState) {
    //  case 0x0:
    // current = kStateConfigured;
    // break;
    //  case 0x8:
    // current = kStateWarmUp;
    // break;
    //  case 0x10:
    // fpMsgbuffer->SingleRead(ModeFreqVal, &u32rawModeFreqVal);
    // fpMsgbuffer->SingleRead(MaxNumberOfFlashEvents, &u32rawLaserEvents);
    // if(u32rawModeFreqVal == 0)
    //  current = kStateOnFreeRun;
    // else if(u32rawLaserEvents == 0)
    //  current = kStateOnTrigger;
    // else if(u32rawLaserEvents > 0)
    //  current = kStateOnLimited;
    // break;
  case 0x20:
    current = kStateFlashStop;
    break;
  default:
    break;
  }
  
  uint32_t u32rawNOfEventsInBurst = 0;
  fpMsgbuffer->SingleRead(NumberOfEventsInBurst, &u32rawNOfEventsInBurst);
  fpMsgbuffer->SingleRead(LaserCounter, &u32rawLaserEvents);
  
  if(u32rawLaserEvents > 0 && u32rawNOfEventsInBurst > 0 && u32rawLaserEvents == u32rawNOfEventsInBurst){
    current = kStateConfigured;
    CE_Debug("DevLaser:: ReSynchronizeLocal found Laser fw completed Nof Events, switched back to Stby_Configured");
  }

  return current;
}


int DevLaser::ArmorLocal(){
  int iResult = 0;

  std::string name=GetName();
  name=name+"_";

  //Writable services
  /*
  fServices.push_back(new SerMbAddrS(name+"MODEFREQVAL"               , ModeFreqVal       , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"1_FLASHSTARTTIME"     , Laser1_FlashStartTime        , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"1_FLASHENDTIME"       , Laser1_FlashEndTime       , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"2_FLASHSTARTTIME"     , Laser2_FlashStartTime       , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"2_FLASHENDTIME"       , Laser2_FlashEndTime       , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"L0REQUESTTIME"             , L0RequestTime       , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"L0RETURNWINDOWSTART"       , L0ReturnWindowStart       , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"L0RETURNWINDOWSTOP"        , L0ReturnWindowStop       , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"1_QSWITCHSTART"       , Laser1_QswitchStart       , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"1_QSWITCHSTOP"        , Laser1_QswitchStop       , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"2_QSWITCHSTART"       , Laser2_QswitchStart       , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"2_QSWITCHSTOP"        , Laser2_QswitchStop       , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"RUNMODE"                   , RunMode       , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"SAMPLECLOCKDIVIDER"        , SampleClockDivider       , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"TRIGGERCONFIGURATION1"     , TriggerConfiguration1       , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"TRIGGERCONFIGURATION2"     , TriggerConfiguration2       , 1, 0, fpMsgbuffer));
  */
  //Readable services
  /*
  fServices.push_back(new SerMbAddrS(name+"RCUVERSION"                , RCUVersion       , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"CTPSIGNATURE"              , CTPsignature       , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"SHUTTERCOUNTER"            , ShutterCounter       , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"1_FLASHCOUNTER"       , Laser1_FlashCounter       , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"2_FLASHCOUNTER"       , Laser2_FlashCounter       , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"1_QSWITCHCOUNTER"     , Laser1_QswitchCounter       , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"2_QSWITCHCOUNTER"     , Laser2_QswitchCounter       , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"L0REQUESTCOUNTER"          , L0RequestCounter       , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"L0RECEIVEDCOUNTER"         , L0ReceivedCounter       , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"L0RECEIVEDINWINDOWCOUNTER" , L0ReceivedInWindowCounter       , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"L0TIMEOUTCOUNTER"          , L0TimeoutCounter       , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"L0RETURNTIMER"             , L0ReturnTimer       , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"ACTUALLASERRATE"           , ActualLaserRate       , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"CYCLETIMER"                , CycleTimer       , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"HEARTBEAT"                 , Heartbeat       , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"MONITORINGREGISTER"        , MonitoringRegister       , 1, 0, fpMsgbuffer)); 
  fServices.push_back(new SerMbAddrS(name+"FLASHCOUNTER"              , FlashCounter       , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"LASER_COUNTER"              , LaserCounter       , 1, 0, fpMsgbuffer)); 
  fServices.push_back(new SerMbAddrS(name+"MAXNUMBEROFFLASHEVENTS"    , MaxNumberOfFlashEvents    , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"NUMBEROFEVENTSINBURST"     , NumberOfEventsInBurst       , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"ENABLES"                   , LaserEnables       , 1, 0, fpMsgbuffer));
  */
  //Trigger Interface services

  fServices.push_back(new SerMbAddrS(name+"TTC_CONTROL"       , TTCControl        , 1, 0, fpMsgbuffer));
  //  fServices.push_back(new SerMbAddrS(name+"TTC_ROICONFIG1"    , TTCROIConfig1     , 1, 0, fpMsgbuffer)); 
  ///  fServices.push_back(new SerMbAddrS(name+"TTC_ROICONFIG2"    , TTCROIConfig2     , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"TTC_L1LATENCY"     , TTCL1Latency      , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"TTC_L2LATENCY"     , TTCL2Latency      , 1, 0, fpMsgbuffer));
  /*
  fServices.push_back(new SerMbAddrS(name+"TTC_ROILATENCY"    , TTCRoILatency     , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"TTC_L1MSGLATENCY"  , TTCL1MsgLatency   , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"TTC_PREPULSECNT"   , TTCPrePulseCnt    , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"TTC_BCIDLOCAL"     , TTCBCIDLocal      , 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"TTC_L0COUNTER"     , TTCL0Counter      , 1, 0, fpMsgbuffer));
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
  */
  return iResult;
}

//Stby_Configured
int DevLaser::EnterStateCONFIGURED(){
  int iResult = 0;
  CE_Debug("DevLaser::EnterStateConfigured called\n");

  iResult = DisableWarmupLock();

  return iResult;
}

/**
 * WarmUp Mode
 **/
int DevLaser::EnterStateWARM_UP(){
  int iResult = 0;
  CE_Debug("DevLaser::EnterStateWARM_UP called\n");
 
  iResult = EnableWarmupLock();

  return iResult;
}

/**
 * OnFreeRun
 **/
int DevLaser::EnterStateON_FREE_RUN(){
  int iResult = 0;
  CE_Debug("DevLaser::EnterStateON_FREE_RUN called\n");    

  fpMsgbuffer->SingleWrite(ModeFreqVal, 0x0);

  //The change from Warmup to Laser needs to be done this way because of timing constraints on these bits.
  iResult = EnableLaser();
  iResult = DisableWarmupLock();

  return iResult;
}

/**
 * OnFreeRun
 **/
int DevLaser::LeaveStateON_FREE_RUN(){
  int iResult = 0;
  CE_Debug("DevLaser::LeaveStateON_FREE_RUN called\n");    
  iResult = DisableLaser();

  return iResult;
}

/**
 * OnTrigger
 **/
int DevLaser::EnterStateON_TRIGGER(){
  int iResult = 0;
  CE_Debug("DevLaser::EnterStateON_TRIGGER called\n");

  //The change from Warmup to Laser needs to be done this way because of timing constraints on these bits.
  iResult = EnableLaser();
  iResult = DisableWarmupLock();  
  fpMsgbuffer->SingleWrite(NumberOfEventsInBurst, 0x0);  
  
  return iResult;
}

/**
 * OnTrigger
 **/
int DevLaser::LeaveStateON_TRIGGER(){
  int iResult = 0;
  CE_Debug("DevLaser::LeaveStateON_TRIGGER called\n");    

  iResult = DisableLaser();
  
  return iResult;
}


/**
 * OnLimited Mode
 **/
int DevLaser::EnterStateON_LIMITED(){
  int iResult = 0;
  CE_Debug("DevLaser::EnterStateON_LIMITED called\n");

  fpMsgbuffer->SingleWrite(NumberOfEventsInBurst, u32MaxEvents);  
  //The change from Warmup to Laser needs to be done this way because of timing constraints on these bits.
  iResult = EnableLaser();
  iResult = DisableWarmupLock();  
  
  return iResult;
}

/**
 * OnLimited Mode
 **/
int DevLaser::LeaveStateON_LIMITED(){
  int iResult = 0;
  CE_Debug("DevLaser::LeaveStateON_LIMITED called\n");    

  fpMsgbuffer->SingleWrite(NumberOfEventsInBurst, 0x0); 
  iResult = DisableLaser();

  return iResult;
}

int DevLaser::GetGroupId(){
  return FEESVR_CMD_LASER;
}

int DevLaser::issue(uint32_t cmd, uint32_t parameter, const char* pData, int iDataSize, std::vector<uint32_t>& rb){

  int iResult = 0;

  CE_Debug("CELaser::TranslateLaserCommand cmd=%#x parameter=%#x datasize=%d\n", cmd, parameter, iDataSize);
  switch (cmd) {
  case LASER_WRITE_MODEFREQVAL:
    fpMsgbuffer->SingleWrite(ModeFreqVal,*(uint32_t*)pData);
    iResult=4;
    break;
  case LASER_WRITE_1_FLASHSTARTTIME:
    fpMsgbuffer->SingleWrite(Laser1_FlashStartTime,*(uint32_t*)pData);
    iResult=4;
    break;
  case LASER_WRITE_1_FLASHENDTIME:
    fpMsgbuffer->SingleWrite(Laser1_FlashEndTime,*(uint32_t*)pData);
    iResult=4;
    break;
  case LASER_WRITE_2_FLASHSTARTTIME:
    fpMsgbuffer->SingleWrite(Laser2_FlashStartTime,*(uint32_t*)pData);
    iResult=4;
    break;
  case LASER_WRITE_2_FLASHENDTIME:
    fpMsgbuffer->SingleWrite(Laser2_FlashEndTime,*(uint32_t*)pData);
    iResult=4;
    break;
  case LASER_WRITE_L0REQUESTTIME:
    fpMsgbuffer->SingleWrite(L0RequestTime,*(uint32_t*)pData);
    iResult=4;
    break;
  case LASER_WRITE_L0RETURNWINDOWSTART:
    fpMsgbuffer->SingleWrite(L0ReturnWindowStart,*(uint32_t*)pData);
    iResult=4;
    break;
  case LASER_WRITE_L0RETURNWINDOWSTOP:
    fpMsgbuffer->SingleWrite(L0ReturnWindowStop,*(uint32_t*)pData);
    iResult=4;
    break;
  case LASER_WRITE_1_QSWITCHSTART:
    fpMsgbuffer->SingleWrite(Laser1_QswitchStart,*(uint32_t*)pData);
    iResult=4;
    break;
  case LASER_WRITE_1_QSWITCHSTOP:
    fpMsgbuffer->SingleWrite(Laser1_QswitchStop,*(uint32_t*)pData);
    iResult=4;
    break;
  case LASER_WRITE_2_QSWITCHSTART:
    fpMsgbuffer->SingleWrite(Laser2_QswitchStart,*(uint32_t*)pData);
    iResult=4;
    break;
  case LASER_WRITE_2_QSWITCHSTOP:
    fpMsgbuffer->SingleWrite(Laser2_QswitchStop,*(uint32_t*)pData);
    iResult=4;
    break;
  case LASER_WRITE_RUNMODE:
    fpMsgbuffer->SingleWrite(RunMode,*(uint32_t*)pData);
    iResult=4;
    break;
  case LASER_WRITE_SAMPLECLOCKDIVIDER:
    fpMsgbuffer->SingleWrite(SampleClockDivider,*(uint32_t*)pData);
    iResult=4;
    break;
  case LASER_WRITE_TRIGGERCONFIGURATION1:
    fpMsgbuffer->SingleWrite(TriggerConfiguration1,*(uint32_t*)pData);
    iResult=4;
    break;
  case LASER_WRITE_TRIGGERCONFIGURATION2:
    fpMsgbuffer->SingleWrite(TriggerConfiguration2,*(uint32_t*)pData);
    iResult=4;
    break;

  case LASER_WRITE_MAXNUMBEROFFLASHEVENTS:
    fpMsgbuffer->SingleWrite(MaxNumberOfFlashEvents,*(uint32_t*)pData);
    iResult=4;
    break;
  case LASER_WRITE_NUMBEROFEVENTSINBURST:
    //fpMsgbuffer->SingleWrite(NumberOfEventsInBurst, *(uint32_t*)pData);
    u32MaxEvents=*(uint32_t*)pData;
    iResult=4;
    break;

  case LASER_WRITE_TTC_CONTROL:
    fpMsgbuffer->SingleWrite(TTCControl,*(uint32_t*)pData);
    iResult=4;
    break;
  case LASER_TOGGLE_TTC_RESET:
    fpMsgbuffer->SingleWrite(TTCModulkActionReset, 0x0);
    iResult=0;
    break;
  case LASER_WRITE_TTC_ROICONFIG1:
    fpMsgbuffer->SingleWrite(TTCROIConfig1,*(uint32_t*)pData);
    iResult=4;
    break;
  case LASER_WRITE_TTC_ROICONFIG2:
    fpMsgbuffer->SingleWrite(TTCROIConfig2,*(uint32_t*)pData);
    iResult=4;
    break;
  case LASER_TOGGLE_TTC_RESETCOUNTER:
    fpMsgbuffer->SingleWrite(TTCResetCounters, 0x0);
    iResult=0;
    break;
  case LASER_TOGGLE_TTC_ISSUETESTMODE:
    fpMsgbuffer->SingleWrite(TTCIssueTestmode, 0x0);
    iResult=0;
    break;
  case LASER_WRITE_TTC_L1LATENCY:
    fpMsgbuffer->SingleWrite(TTCL1Latency,*(uint32_t*)pData);
    iResult=4;
    break;
  case LASER_WRITE_TTC_L2LATENCY:
    fpMsgbuffer->SingleWrite(TTCL2Latency,*(uint32_t*)pData);
    iResult=4;
    break;
  case LASER_WRITE_TTC_ROILATENCY:
    fpMsgbuffer->SingleWrite(TTCRoILatency,*(uint32_t*)pData);
    iResult=4;
    break;
  case LASER_WRITE_TTC_L1MSGLATENCY:
    fpMsgbuffer->SingleWrite(TTCL1MsgLatency,*(uint32_t*)pData);
    iResult=4;
    break;
  case LASER_CLEAR_SOD:
    fpMsgbuffer->SingleWrite(ClearSOD,0x0);
    iResult = 0;
    break;
  case LASER_ARM_SYNC:
    fpMsgbuffer->SingleWrite(ArmSync,0x0);
    iResult = 0;
    break;
  case LASER_WRITE_ENABLES:
    fpMsgbuffer->SingleWrite(LaserEnables,*(uint32_t*)pData);
    iResult = 4;
    break;
    //DCS commandset
  case LASER_SET_1_FLASHSTARTTIME:
    iResult=SetLaserFlashStartTime(pData, Laser1_FlashStartTime);
    break;
  case LASER_SET_1_FLASHENDTIME:
    iResult=SetLaserFlashEndTime(pData, Laser1_FlashEndTime);
    break;
  case LASER_SET_2_FLASHSTARTTIME:
    iResult=SetLaserFlashStartTime(pData, Laser2_FlashStartTime);
    break;
  case LASER_SET_2_FLASHENDTIME:
    iResult=SetLaserFlashEndTime(pData, Laser2_FlashEndTime);
    break;
  case LASER_SET_L0REQUESTTIME:
    iResult=SetL0RequestTime(pData);
    iResult=4;
    break;
  case LASER_SET_L0RETURNWINDOWSTART:
    iResult=SetL0ReturnWindowStart(pData);
    iResult=4;
    break;
  case LASER_SET_L0RETURNWINDOWEND:
    iResult=SetL0ReturnWindowEnd(pData);
    iResult=4;
    break;
  case LASER_SET_1_QSWITCHSTART:
    iResult=SetLaser1QSwitchStart(pData);
    iResult=4;
    break;
  case LASER_SET_1_QSWITCHEND:
    iResult=SetLaser1QSwitchEnd(pData);
    iResult=4;
    break;
  case LASER_CLEAR_COUNTERS:
    iResult = ClearCounters();
    iResult = 4;
    break;    
  case LASER_ENABLE_WARMUP:
    iResult = EnableWarmupLock();
    iResult = 4;
    break;
  case LASER_DISABLE_WARMUP:
    iResult = DisableWarmupLock();
    iResult = 4;
    break;
  case LASER_ENABLE_1:
    iResult = EnableLaser1();
    iResult = 4;
    break;
  case LASER_DISABLE_1:
    iResult = DisableLaser1();
    iResult = 4;
    break;
  case LASER_ENABLE_2:
    iResult = EnableLaser2();
    iResult = 4;
    break;
  case LASER_DISABLE_2:
    iResult = DisableLaser2();
    iResult = 4;
    break;
  case LASER_READ_REG:
    {
      uint32_t data;
      const char* pBuffer = pData;
      int address=0x0;
      if(sscanf(pBuffer, "0x%x", &address)>0){	
	fpMsgbuffer->SingleRead(address, &data, 1);
	CE_Event("DevLaser::Read data =  0x%x (address = 0x%x)\n", data, address);
	iResult=4;
      }else{
	CE_Warning("DevLaser::LASER_READ_REG format is wrong :0x%x\n", pBuffer[0]);
      }
    }
    break;
  default:
    CE_Debug("CELaser: unrecognized command id (%#x)\n", cmd);
    iResult=-ENOSYS;
  }
  return iResult;
}


int DevLaser::SetLaser1QSwitchEnd(const char* pDataOrig){
  int iResult = 0;
  uint32_t u32Cms = 40000000;
  uint32_t u32Cus = 40000;
  uint32_t u32rawData = 0;
  uint32_t u32shut = 0;
  uint32_t u32qet = 0;
  uint32_t u32qed = 0;
  uint32_t u32shift = 0;
  uint32_t *pData;
  
  const char* pSrc = pDataOrig;

  u32rawData=*((uint32_t*)pSrc);
  CE_Debug("Shut: %#x\n", u32rawData);
  u32shut = u32rawData;

  pSrc+=sizeof(uint32_t);
  u32rawData = *((uint32_t*)pSrc);
  CE_Debug("qet %#x\n", u32rawData);
  u32qet = u32rawData;

  pSrc+=sizeof(uint32_t);
  u32rawData = *((uint32_t*)pSrc);
  CE_Debug("qed %#x\n", u32rawData);
  u32qed = u32rawData;

  pSrc+=sizeof(uint32_t);
  u32rawData = *((uint32_t*)pSrc);
  CE_Debug("shift %#x\n", u32rawData);
  u32shift = u32rawData;

  u32rawData = u32shut * u32Cms + (u32qet + u32qed + u32shift) * u32Cus;
  pData = &u32rawData;
  
  fpMsgbuffer->SingleWrite(Laser1_QswitchStop,*(uint32_t*)pData);
  iResult = 4;

  return iResult;
}

int DevLaser::SetLaser1QSwitchStart(const char* pDataOrig){
  int iResult = 0;
  uint32_t u32Cms = 40000000;
  uint32_t u32Cus = 40000;
  uint32_t u32rawData = 0;
  uint32_t u32shut = 0;
  uint32_t u32qet = 0;
  uint32_t u32shift = 0;
  uint32_t *pData;
  
  const char* pSrc = pDataOrig;

  u32rawData=*((uint32_t*)pSrc);
  CE_Debug("Shut: %#x\n", u32rawData);
  u32shut = u32rawData;

  pSrc+=sizeof(uint32_t);
  u32rawData = *((uint32_t*)pSrc);
  CE_Debug("qet %#x\n", u32rawData);
  u32qet = u32rawData;

  pSrc+=sizeof(uint32_t);
  u32rawData = *((uint32_t*)pSrc);
  CE_Debug("shift %#x\n", u32rawData);
  u32shift = u32rawData;

  u32rawData = u32shut * u32Cms + (u32qet + u32shift) * u32Cus;
  pData = &u32rawData;
  
  fpMsgbuffer->SingleWrite(Laser1_QswitchStart,*(uint32_t*)pData);
  iResult = 4;

  return iResult;
}

int DevLaser::SetL0ReturnWindowEnd(const char* pDataOrig){
  int iResult = 0;
  uint32_t u32Cms = 40000000;
  uint32_t u32Cus = 40000;
  uint32_t u32rawData = 0;
  uint32_t u32shut = 0;
  uint32_t u32l0 = 0;
  uint32_t u32l01 = 0;
  uint32_t u32l02 = 0;
  uint32_t u32shift = 0;
  uint32_t *pData;
  
  const char* pSrc = pDataOrig;

  u32rawData=*((uint32_t*)pSrc);
  CE_Debug("Shut: %#x\n", u32rawData);
  u32shut = u32rawData;

  pSrc+=sizeof(uint32_t);
  u32rawData = *((uint32_t*)pSrc);
  CE_Debug("L0 %#x\n", u32rawData);
  u32l0 = u32rawData;

  pSrc+=sizeof(uint32_t);
  u32rawData = *((uint32_t*)pSrc);
  CE_Debug("L01 %#x\n", u32rawData);
  u32l01 = u32rawData;

  pSrc+=sizeof(uint32_t);
  u32rawData = *((uint32_t*)pSrc);
  CE_Debug("L02 %#x\n", u32rawData);
  u32l02 = u32rawData;

  pSrc+=sizeof(uint32_t);
  u32rawData = *((uint32_t*)pSrc);
  CE_Debug("shift %#x\n", u32rawData);
  u32shift = u32rawData;

  u32rawData = u32shut * u32Cms + (u32l0 + u32l01 + u32l02 + u32shift) * u32Cus;
  pData = &u32rawData;
  
  fpMsgbuffer->SingleWrite(L0ReturnWindowStop,*(uint32_t*)pData);
  iResult = 4;

  return iResult;
}

int DevLaser::SetL0ReturnWindowStart(const char* pDataOrig){
  int iResult = 0;
  uint32_t u32Cms = 40000000;
  uint32_t u32Cus = 40000;
  uint32_t u32rawData = 0;
  uint32_t u32shut = 0;
  uint32_t u32l0 = 0;
  uint32_t u32l01 = 0;
  uint32_t u32shift = 0;
  uint32_t *pData;
  
  const char* pSrc = pDataOrig;

  u32rawData=*((uint32_t*)pSrc);
  CE_Debug("Shut: %#x\n", u32rawData);
  u32shut = u32rawData;

  pSrc+=sizeof(uint32_t);
  u32rawData = *((uint32_t*)pSrc);
  CE_Debug("L0 %#x\n", u32rawData);
  u32l0 = u32rawData;

  pSrc+=sizeof(uint32_t);
  u32rawData = *((uint32_t*)pSrc);
  CE_Debug("L01 %#x\n", u32rawData);
  u32l01 = u32rawData;

  pSrc+=sizeof(uint32_t);
  u32rawData = *((uint32_t*)pSrc);
  CE_Debug("shift %#x\n", u32rawData);
  u32shift = u32rawData;

  u32rawData = u32shut * u32Cms + (u32l0 + u32l01+ u32shift) * u32Cus;
  pData = &u32rawData;
  
  fpMsgbuffer->SingleWrite(L0ReturnWindowStart,*(uint32_t*)pData);
  iResult = 4;

  return iResult;
}

int DevLaser::SetL0RequestTime(const char* pDataOrig){
  int iResult = 0;
  uint32_t u32Cms = 40000000;
  uint32_t u32Cus = 40000;
  uint32_t u32rawData = 0;
  uint32_t u32shut = 0;
  uint32_t u32l0 = 0;
  uint32_t u32shift = 0;
  uint32_t *pData;
  
  const char* pSrc = pDataOrig;

  u32rawData=*((uint32_t*)pSrc);
  CE_Debug("Shut: %#x\n", u32rawData);
  u32shut = u32rawData;

  pSrc+=sizeof(uint32_t);
  u32rawData = *((uint32_t*)pSrc);
  CE_Debug("L0 %#x\n", u32rawData);
  u32l0 = u32rawData;

  pSrc+=sizeof(uint32_t);
  u32rawData = *((uint32_t*)pSrc);
  CE_Debug("shift %#x\n", u32rawData);
  u32shift = u32rawData;

  u32rawData = u32shut * u32Cms + (u32l0 + u32shift) * u32Cus;
  pData = &u32rawData;
  
  fpMsgbuffer->SingleWrite(L0RequestTime,*(uint32_t*)pData);
  iResult = 4;

  return iResult;
}

int DevLaser::SetLaserFlashEndTime(const char* pDataOrig, uint32_t reg){
  int iResult = 0;
  uint32_t u32Cms = 40000000;
  uint32_t u32Cus = 40000;
  uint32_t u32rawData = 0;
  uint32_t u32shut = 0;
  uint32_t u32fet = 0;
  uint32_t u32fed = 0;
  uint32_t u32shift = 0;
  uint32_t *pData;
  
  const char* pSrc = pDataOrig;

  u32rawData=*((uint32_t*)pSrc);
  CE_Debug("Shut: %#x\n", u32rawData);
  u32shut = u32rawData;

  pSrc+=sizeof(uint32_t);
  u32rawData = *((uint32_t*)pSrc);
  CE_Debug("fet %#x\n", u32rawData);
  u32fet = u32rawData;

  pSrc+=sizeof(uint32_t);
  u32rawData = *((uint32_t*)pSrc);
  CE_Debug("fed %#x\n", u32rawData);
  u32fed = u32rawData;

  pSrc+=sizeof(uint32_t);
  u32rawData = *((uint32_t*)pSrc);
  CE_Debug("shift %#x\n", u32rawData);
  u32shift = u32rawData;

  u32rawData = u32shut * u32Cms + (u32fet + u32fed + u32shift) * u32Cus;
  pData = &u32rawData;
  
  fpMsgbuffer->SingleWrite(reg,*(uint32_t*)pData);
  iResult = 4;

  return iResult;
}

int DevLaser::SetLaserFlashStartTime(const char* pDataOrig, uint32_t reg){
  int iResult = 0;
  uint32_t u32Cms = 40000000;
  uint32_t u32Cus = 40000;
  uint32_t u32rawData = 0;
  uint32_t u32shut = 0;
  uint32_t u32fet = 0;
  uint32_t u32shift = 0;
  uint32_t *pData;
  
  const char* pSrc = pDataOrig;

  u32rawData=*((uint32_t*)pSrc);
  CE_Debug("Shut: %#x\n", u32rawData);
  u32shut = u32rawData;

  pSrc+=sizeof(uint32_t);
  u32rawData = *((uint32_t*)pSrc);
  CE_Debug("fet %#x\n", u32rawData);
  u32fet = u32rawData;

  pSrc+=sizeof(uint32_t);
  u32rawData = *((uint32_t*)pSrc);
  CE_Debug("shift %#x\n", u32rawData);
  u32shift = u32rawData;

  u32rawData = u32shut * u32Cms + (u32fet + u32shift) * u32Cus;
  pData = &u32rawData;
  
  fpMsgbuffer->SingleWrite(reg,*(uint32_t*)pData);
  iResult = 4;

  return iResult;
}


int DevLaser::ClearCounters(){
  int iResult=0;
    uint32_t u32rawData;
    int iretryCount;

    iretryCount = 0;
    u32rawData = 0;

    fpMsgbuffer->SingleRead(LaserEnables, &u32rawData);
    if ((u32rawData & EnableClearCounters) == EnableClearCounters){
      //the bit was already set
    }
    else{
      //the bit was not set, we set it and wait for the counters to clear, then set it to 0 again.
      fpMsgbuffer->SingleWrite(LaserEnables,(u32rawData | EnableClearCounters) );
      usleep(100);
      fpMsgbuffer->SingleRead(LaserEnables, &u32rawData);
      while (((u32rawData & EnableClearCounters) != EnableClearCounters) || (iretryCount > 20)) {
	usleep(100);
	fpMsgbuffer->SingleRead(LaserEnables, &u32rawData);
	iretryCount++;
      }
      //the bit was set or the timeout jumped in
      if((u32rawData & EnableClearCounters) == EnableClearCounters){
	//the bit was set, so we clear it again
	fpMsgbuffer->SingleWrite(LaserEnables,(u32rawData & ~EnableClearCounters) );
	iResult=4;
      }
      else{
	//the timeout jumped in, so we give an error
	iResult=-1;
      }
    }  
  return iResult;
}


int DevLaser::EnableLaser1(){
  int iResult = 0;
  uint32_t u32rawData;

  fpMsgbuffer->SingleRead(LaserEnables, &u32rawData);
  fpMsgbuffer->SingleWrite(LaserEnables,(u32rawData | EnableMaskLaser1) );

  return iResult;
}


int DevLaser::EnableLaser2(){
  int iResult = 0;
  uint32_t u32rawData;

  fpMsgbuffer->SingleRead(LaserEnables, &u32rawData);
  fpMsgbuffer->SingleWrite(LaserEnables,(u32rawData | EnableMaskLaser2) );
  return iResult;
}

int DevLaser::EnableWarmupLock(){
  int iResult = 0;
  uint32_t u32rawData;

  fpMsgbuffer->SingleRead(RunMode, &u32rawData);
  fpMsgbuffer->SingleWrite(RunMode,(u32rawData | RunModeMaskWarmup) );
  return iResult;
}

int DevLaser::EnableGlobal(){
  int iResult = 0;
  uint32_t u32rawData;

  fpMsgbuffer->SingleRead(LaserEnables, &u32rawData);
  fpMsgbuffer->SingleWrite(LaserEnables,(u32rawData | EnableMaskGlobal) );
  return iResult;
}

int DevLaser::DisableLaser1(){
  int iResult = 0;
  uint32_t u32rawData;

  fpMsgbuffer->SingleRead(LaserEnables, &u32rawData);
  fpMsgbuffer->SingleWrite(LaserEnables,(u32rawData & ~EnableMaskLaser1) );
  return iResult;
}

int DevLaser::DisableLaser2(){
  int iResult = 0;
  uint32_t u32rawData;

  fpMsgbuffer->SingleRead(LaserEnables, &u32rawData);
  fpMsgbuffer->SingleWrite(LaserEnables,(u32rawData & ~EnableMaskLaser2) );
  return iResult;
}

int DevLaser::DisableWarmupLock(){
  int iResult = 0;
  uint32_t u32rawData;

  fpMsgbuffer->SingleRead(RunMode, &u32rawData);
  fpMsgbuffer->SingleWrite(RunMode,(u32rawData & ~RunModeMaskWarmup) );
  return iResult;
}

int DevLaser::DisableGlobal(){
  int iResult = 0;
  uint32_t u32rawData;

  fpMsgbuffer->SingleRead(LaserEnables, &u32rawData);
  fpMsgbuffer->SingleWrite(LaserEnables,(u32rawData & ~EnableMaskGlobal) );
  return iResult;
}

int DevLaser::DisableLaser(){
  int iResult = 0;
  uint32_t u32rawData;

  fpMsgbuffer->SingleRead(RunMode, &u32rawData);
  fpMsgbuffer->SingleWrite(RunMode,(u32rawData & ~RunModeMaskLaser) );
  return iResult;
}


int DevLaser::EnableLaser(){
  int iResult = 0;
  uint32_t u32rawData;

  fpMsgbuffer->SingleRead(RunMode, &u32rawData);
  fpMsgbuffer->SingleWrite(RunMode,(u32rawData | RunModeMaskLaser) );
  return iResult;
}
int DevLaser::HighLevelHandler(const char* pCommand, std::vector<uint32_t>& rb){
  
  CE_Event("DevLaser::HighLevelHandler\n");

  int iResult=0;
  uint32_t cmd=0;
  int keySize=0;
  int len=strlen(pCommand);
  const char* pBuffer=pCommand;
  if (pCommand) {
  /**
  int keySize=0;
  uint32_t cmd=0;
  if(!pCommand) return 0;
  **/
    CE_Debug("CELaser checks command %s\n", pCommand);
    if (strncmp(pCommand, "LASER_WRITE_MODEFREQVAL", keySize=strlen("LASER_WRITE_MODEFREQVAL"))==0)
      cmd=LASER_WRITE_MODEFREQVAL;
    else if (strncmp(pCommand, "LASER_WRITE_1_FLASHSTARTTIME", keySize=strlen("LASER_WRITE_1_FLASHSTARTTIME"))==0)
      cmd=LASER_WRITE_1_FLASHSTARTTIME;
    else if (strncmp(pCommand, "LASER_WRITE_1_FLASHENDTIME", keySize=strlen("LASER_WRITE_1_FLASHENDTIME"))==0)
      cmd=LASER_WRITE_1_FLASHENDTIME;
    else if (strncmp(pCommand, "LASER_WRITE_2_FLASHSTARTTIME", keySize=strlen("LASER_WRITE_2_FLASHSTARTTIME"))==0)
      cmd=LASER_WRITE_2_FLASHSTARTTIME;
    else if (strncmp(pCommand, "LASER_WRITE_2_FLASHENDTIME", keySize=strlen("LASER_WRITE_2_FLASHENDTIME"))==0)
      cmd=LASER_WRITE_2_FLASHENDTIME;
    else if (strncmp(pCommand, "LASER_WRITE_L0REQUESTTIME", keySize=strlen("LASER_WRITE_L0REQUESTTIME"))==0)
      cmd=LASER_WRITE_L0REQUESTTIME;
    else if (strncmp(pCommand, "LASER_WRITE_L0RETURNWINDOWSTART", keySize=strlen("LASER_WRITE_L0RETURNWINDOWSTART"))==0)
      cmd=LASER_WRITE_L0RETURNWINDOWSTART;
    else if (strncmp(pCommand, "LASER_WRITE_L0RETURNWINDOWSTOP", keySize=strlen("LASER_WRITE_L0RETURNWINDOWSTOP"))==0)
      cmd=LASER_WRITE_L0RETURNWINDOWSTOP;
    else if (strncmp(pCommand, "LASER_WRITE_1_QSWITCHSTART", keySize=strlen("LASER_WRITE_1_QSWITCHSTART"))==0)
      cmd=LASER_WRITE_1_QSWITCHSTART;
    else if (strncmp(pCommand, "LASER_WRITE_1_QSWITCHSTOP", keySize=strlen("LASER_WRITE_1_QSWITCHSTOP"))==0)
      cmd=LASER_WRITE_1_QSWITCHSTOP;
    else if (strncmp(pCommand, "LASER_WRITE_2_QSWITCHSTART", keySize=strlen("LASER_WRITE_2_QSWITCHSTART"))==0)
      cmd=LASER_WRITE_2_QSWITCHSTART;
    else if (strncmp(pCommand, "LASER_WRITE_2_QSWITCHSTOP", keySize=strlen("LASER_WRITE_2_QSWITCHSTOP"))==0)
      cmd=LASER_WRITE_2_QSWITCHSTOP;
    else if (strncmp(pCommand, "LASER_WRITE_RUNMODE", keySize=strlen("LASER_WRITE_RUNMODE"))==0)
      cmd=LASER_WRITE_RUNMODE;
    else if (strncmp(pCommand, "LASER_WRITE_SAMPLECLOCKDIVIDER", keySize=strlen("LASER_WRITE_SAMPLECLOCKDIVIDER"))==0)
      cmd=LASER_WRITE_SAMPLECLOCKDIVIDER;
    else if (strncmp(pCommand, "LASER_WRITE_TRIGGERCONFIGURATION1", keySize=strlen("LASER_WRITE_TRIGGERCONFIGURATION1"))==0)
      cmd=LASER_WRITE_TRIGGERCONFIGURATION1;
    else if (strncmp(pCommand, "LASER_WRITE_TRIGGERCONFIGURATION2", keySize=strlen("LASER_WRITE_TRIGGERCONFIGURATION2"))==0)
      cmd=LASER_WRITE_TRIGGERCONFIGURATION2;
    else if (strncmp(pCommand, "LASER_WRITE_TTC_CONTROL", keySize=strlen("LASER_WRITE_TTC_CONTROL"))==0)
      cmd=LASER_WRITE_TTC_CONTROL;
    else if (strncmp(pCommand, "LASER_TOGGLE_TTC_RESET", keySize=strlen("LASER_TOGGLE_TTC_RESET"))==0)
      cmd=LASER_TOGGLE_TTC_RESET;
    else if (strncmp(pCommand, "LASER_WRITE_TTC_ROICONFIG1", keySize=strlen("LASER_WRITE_TTC_ROICONFIG1"))==0)
      cmd=LASER_WRITE_TTC_ROICONFIG1;
    else if (strncmp(pCommand, "LASER_WRITE_TTC_ROICONFIG2", keySize=strlen("LASER_WRITE_TTC_ROICONFIG2"))==0)
      cmd=LASER_WRITE_TTC_ROICONFIG2;
    else if (strncmp(pCommand, "LASER_TOGGLE_TTC_RESETCOUNTER", keySize=strlen("LASER_TOGGLE_TTC_RESETCOUNTER"))==0)
      cmd=LASER_TOGGLE_TTC_RESETCOUNTER;
    else if (strncmp(pCommand, "LASER_TOGGLE_TTC_ISSUETESTMODE", keySize=strlen("LASER_TOGGLE_TTC_ISSUETESTMODE"))==0)
      cmd=LASER_TOGGLE_TTC_ISSUETESTMODE;
    else if (strncmp(pCommand, "LASER_WRITE_TTC_L2LATENCY", keySize=strlen("LASER_WRITE_TTC_L2LATENCY"))==0)
      cmd=LASER_WRITE_TTC_L2LATENCY;
    else if (strncmp(pCommand, "LASER_WRITE_TTC_L1LATENCY", keySize=strlen("LASER_WRITE_TTC_L1LATENCY"))==0)
      cmd=LASER_WRITE_TTC_L1LATENCY;
    else if (strncmp(pCommand, "LASER_WRITE_TTC_ROILATENCY", keySize=strlen("LASER_WRITE_TTC_ROILATENCY"))==0)
      cmd=LASER_WRITE_TTC_ROILATENCY;
    else if (strncmp(pCommand, "LASER_WRITE_TTC_L1MSGLATENCY", keySize=strlen("LASER_WRITE_TTC_L1MSGLATENCY"))==0)
      cmd=LASER_WRITE_TTC_L1MSGLATENCY;
    else if (strncmp(pCommand, "LASER_SET_1_FLASHSTARTTIME", keySize=strlen("LASER_SET_1_FLASHSTARTTIME"))==0)
      cmd=LASER_SET_1_FLASHSTARTTIME;
    else if (strncmp(pCommand, "LASER_SET_1_FLASHENDTIME", keySize=strlen("LASER_SET_1_FLASHENDTIME"))==0)
      cmd=LASER_SET_1_FLASHENDTIME;
    else if (strncmp(pCommand, "LASER_SET_2_FLASHSTARTTIME", keySize=strlen("LASER_SET_2_FLASHSTARTTIME"))==0)
      cmd=LASER_SET_2_FLASHSTARTTIME;
    else if (strncmp(pCommand, "LASER_SET_2_FLASHENDTIME", keySize=strlen("LASER_SET_2_FLASHENDTIME"))==0)
      cmd=LASER_SET_2_FLASHENDTIME;
    else if (strncmp(pCommand, "LASER_SET_L0REQUESTTIME", keySize=strlen("LASER_SET_L0REQUESTTIME"))==0)
      cmd=LASER_SET_L0REQUESTTIME;
    else if (strncmp(pCommand, "LASER_SET_L0RETURNWINDOWSTART", keySize=strlen("LASER_SET_L0RETURNWINDOWSTART"))==0)
      cmd=LASER_SET_L0RETURNWINDOWSTART;
    else if (strncmp(pCommand, "LASER_SET_L0RETURNWINDOWEND", keySize=strlen("LASER_SET_L0RETURNWINDOWEND"))==0)
      cmd=LASER_SET_L0RETURNWINDOWEND;
    else if (strncmp(pCommand, "LASER_SET_LASER_1_QSWITCHSTART", keySize=strlen("LASER_SET_1_QSWITCHSTART"))==0)
      cmd=LASER_SET_1_QSWITCHSTART;
    else if (strncmp(pCommand, "LASER_SET_LASER_1_QSWITCHEND", keySize=strlen("LASER_SET_1_QSWITCHEND"))==0)
      cmd=LASER_SET_1_QSWITCHEND;
    else if (strncmp(pCommand, "LASER_CLEAR_COUNTERS", keySize=strlen("LASER_CLEAR_COUNTERS"))==0)
      cmd=LASER_CLEAR_COUNTERS;
    else if (strncmp(pCommand, "LASER_ENABLE_WARMUP", keySize=strlen("LASER_ENABLE_WARMUP"))==0)
      cmd=LASER_ENABLE_WARMUP;
    else if (strncmp(pCommand, "LASER_ENABLE_2", keySize=strlen("LASER_ENABLE_2"))==0)
      cmd=LASER_ENABLE_2;
    else if (strncmp(pCommand, "LASER_ENABLE_1", keySize=strlen("LASER_ENABLE_1"))==0)
      cmd=LASER_ENABLE_1;
    else if (strncmp(pCommand, "LASER_DISABLE_WARMUP", keySize=strlen("LASER_DISABLE_WARMUP"))==0)
      cmd=LASER_DISABLE_WARMUP;
    else if (strncmp(pCommand, "LASER_DISABLE_2", keySize=strlen("LASER_DISABLE_2"))==0)
      cmd=LASER_DISABLE_2;
    else if (strncmp(pCommand, "LASER_DISABLE_1", keySize=strlen("LASER_DISABLE_1"))==0)
      cmd=LASER_DISABLE_1;
    else if(strncmp(pCommand, "LASER_WRITE_NUMBEROFEVENTSINBURST", keySize=strlen("LASER_WRITE_NUMBEROFEVENTSINBURST"))==0)
      cmd=LASER_WRITE_NUMBEROFEVENTSINBURST;
    else if(strncmp(pCommand, "LASER_WRITE_MAXNUMBEROFFLASHEVENTS", keySize=strlen("LASER_WRITE_MAXNUMBEROFFLASHEVENTS"))==0)
      cmd=LASER_WRITE_MAXNUMBEROFFLASHEVENTS;
    else if(strncmp(pCommand, "LASER_CLEAR_SOD", keySize=strlen("LASER_CLEAR_SOD"))==0)
      cmd=LASER_CLEAR_SOD;
    else if(strncmp(pCommand, "LASER_ARM_SYNC", keySize=strlen("LASER_ARM_SYNC"))==0)
      cmd=LASER_ARM_SYNC;
    else if(strncmp(pCommand, "LASER_WRITE_ENABLES", keySize=strlen("LASER_WRITE_ENABLES"))==0)
      cmd=LASER_WRITE_ENABLES;
    else if(strncmp(pCommand, "LASER_READ_REG", keySize=strlen("LASER_READ_REG"))==0)
      cmd=LASER_READ_REG;

   if (cmd>0 && keySize>0) {
      if(cmd!=LASER_READ_REG){
	pBuffer+=keySize;
	len-=keySize;
	while (*pBuffer==' ' && *pBuffer!=0 && len>0) {pBuffer++; len--;} // skip all blanks
	uint32_t parameter=0;
	uint32_t data=0;
	const char* pPayload=NULL;
	int iPayloadSize=0;
	if(sscanf(pBuffer, "0x%x", &data)>0) {
	  pPayload=(const char*)&data;
	  iPayloadSize=sizeof(data);
	}else {
	  CE_Error("error scanning high-level command %s\n", pCommand);
	  iResult=-EPROTO;
	}
	if (iResult>=0) {
	  if ((iResult=issue(cmd, parameter, pPayload, iPayloadSize, rb))>=0) {
	    iResult=1;
	  }
	}
      }else{
	std::string parameter="";
	std::string command=pCommand;
	if(keySize<command.size()){
	  if(command[keySize]!=' '){
	    CE_Warning("High level fee command '%s' not found in parameter group %i\n", command.c_str(), GetGroupId());
	    return -2;
	  }
	  parameter=command.substr(keySize+1,command.size());
	}
	if(iResult=issue(cmd, atoi(parameter.c_str()), parameter.c_str(), parameter.size(), rb)>=0){
	  iResult=1;
	}
      }
   }
  } else {
    iResult=-EINVAL;
  }
/**
  if(cmd<=0 && keySize<=0) return -EINVAL;
  const char* pBuffer=pCommand;
  pBuffer+=keySize;
  int len=strlen(pCommand);
  len-=keySize;
  while(*pBuffer==' ' && *pBuffer!=0 && len>0) make cleanmake
  {pBuffer++; len--;} // skip all blanks
  uint32_t data=0;
  int iResult=0;
  switch(cmd){
  default:
    sscanf(pBuffer, "%d", &data);
    iResult=issue(cmd, 0, (const char*)&data, 4, rb);
    break;
    }**/
  return iResult;
}

