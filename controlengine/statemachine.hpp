#ifndef __STATEMACHINE_HPP
#define __STATEMACHINE_HPP

#include <stdint.h>
#include <vector>
#include <cstdarg>
#include "issuehandler.hpp"
#include "lockguard.hpp"
#include "ser.hpp"

//States
#define kStateSequence5 88
#define kStateSequence4 87
#define kStateSequence3 86
#define kStateSequence2 85
#define kStateSequence1 84
#define kStateSequence0 83
#define kStateSequence9 82
#define kStateSequence8 81
#define kStateSequence7 80
#define kStateSequence6 79
#define kStateErrorRecover 78
#define kStateSuperSafe 77
#define kStatePulserL1 76
#define kStatePulserL0 75
#define kStatePulserOff 74
#define kStateFlashStop 73
#define kStateOnOnce 72
#define kStateOnEmbedded 71
#define kStateStbyEmbedded 70
#define kStateOnLimited 69
#define kStateWarmingUp 68
#define kStateWarmUp 67
#define kStateColdConfigured 66
#define kStateOnTrigger 65
#define kStateOnFreeRun 64
#define kStateRunInhibit 63
#define kStateNoState 62 //Do not use
#define kStateOnShutterClosed 61
#define kStateOnShutterOpen 60
#define kStateEorProg 59
#define kStateSorProg 58
#define kStateEorFailure 57
#define kStateSorFailure 56
#define kStateRunOk 55
#define kStateScrubbing 54
#define kStateReadback 53
#define kStatkActionStopped 52
#define kStateExpert 51
#define kStateOpen 50
#define kStateClosed 49
#define kStateLocked 48
#define kStateReadyDet 47
#define kStateOnDet 46
#define kStateBeamTunDet 45
#define kStateInterTunDet 44
#define kStateStbyConfFero 43
#define kStateStbyConfDet 42
#define kStateFailure 41
#define kStateReadyLocked 40
#define kStateBeamTun 39
#define kStateMovReady 38
#define kStateMovStbyConf 37
#define kStateMovBeamTun 36
#define kStateCalibrating 35
#define kStateRunning 34
#define kStateIdle 33
#define kStateConfDdl 32
#define kStateOk 31
#define kStateNotOk 30
#define kStateSoftInterlock 29
#define kStateProgressingDown 28
#define kStateProgressingUp 27
#define kStateTripped 26
#define kStateErrorLocal 25
#define kStateInterlock 24
#define kStateInterlockWent 23
#define kStateInterlockWait 22
#define kStateDownloading 21
#define kStateMixedLo 20
#define kStateMixed 19
#define kStateRampDownPause 18
#define kStateRampUpPause 17
#define kStateProgressing 16
#define kStateSaving 15
#define kStateInter 14
#define kStateError 13
#define kStateNotReady 12
#define kStateOn 11
#define kStateRampUpLo 10
#define kStateRampDownLo 9
#define kStateRampUp 8
#define kStateRampDown 7
#define kStateReady 6
#define kStateStandby 5
#define kStateOff 4
#define kStateConfigured 3
#define kStateNoControl 2
#define kStateDead 1 //Do not use
#define kStateElse 0

//State names
#define kStateSequence0_name "SEQUENCE_0"
#define kStateSequence1_name "SEQUENCE_1"
#define kStateSequence2_name "SEQUENCE_2"
#define kStateSequence3_name "SEQUENCE_3"
#define kStateSequence4_name "SEQUENCE_4"
#define kStateSequence5_name "SEQUENCE_5"
#define kStateSequence6_name "SEQUENCE_6"
#define kStateSequence7_name "SEQUENCE_7"
#define kStateSequence8_name "SEQUENCE_8"
#define kStateSequence9_name "SEQUENCE_9"
#define kStateErrorRecover_name "ERROR_RECOVER"
#define kStateSuperSafe_name "SUPER_SAFE"
#define kStatePulserL1_name "PULSER_L1"
#define kStatePulserL0_name "PULSER_L0"
#define kStatePulserOff_name "PULSER_OFF"
#define kStateFlashStop_name "FLASH_STOP"
#define kStateOnOnce_name "ON_ONCE"
#define kStateOnEmbedded_name "ON_EMBEDDED"
#define kStateStbyEmbedded_name "STBY_EMBEDDED"
#define kStateOnLimited_name "ON_LIMITED"
#define kStateWarmingUp_name "WARMING_UP"
#define kStateWarmUp_name "WARM_UP"
#define kStateColdConfigured_name "COLD_CONFIGURED"
#define kStateOnTrigger_name "ON_TRIGGER"
#define kStateOnFreeRun_name "ON_FREE_RUN"
#define kStateRunInhibit_name "RUN_INHIBIT"
#define kStateNoState_name "NO_STATE"
#define kStateOnShutterClosed_name "ON_SHUTTER_CLOSED"
#define kStateOnShutterOpen_name "ON_SHUTTER_OPEN"
#define kStateEorProg_name "EOR_PROGRESSING"
#define kStateSorProg_name "SOR_PROGRESSING"
#define kStateEorFailure_name "EOR_FAILURE"
#define kStateSorFailure_name "SOR_FAILURE"
#define kStateRunOk_name "RUN_OK"
#define kStateScrubbing_name "SCRUBBING"
#define kStateReadback_name "READBACK"
#define kStatkActionStopped_name "STOPPED"
#define kStateExpert_name "EXPERT"
#define kStateOpen_name "OPEN"
#define kStateClosed_name "CLOSED"
#define kStateLocked_name "LOCKED"
#define kStateReadyDet_name "READY_DET"
#define kStateOnDet_name "ON_DET"
#define kStateBeamTunDet_name "BEAM_TUN_DET"
#define kStateInterTunDet_name "INTERMEDIATE_DET"
#define kStateStbyConfFero_name "STBY_CONF_FERO"
#define kStateStbyConfDet_name "STBY_CONF_DET"
#define kStateFailure_name "FAILURE"
#define kStateReadyLocked_name "READY_LOCKED"
#define kStateBeamTun_name "BEAM_TUNING"
#define kStateMovReady_name "MOVING_READY"
#define kStateMovStbyConf_name "MOVING_STBY_CONF"
#define kStateMovBeamTun_name "MOVING_BEAM_TUN"
#define kStateCalibrating_name "CALIBRATING"
#define kStateRunning_name "RUNNING"
#define kStateConfDdl_name "CONF_DDL"
#define kStateIdle_name "IDLE"
#define kStateNotOk_name "NOT_OK"
#define kStateOk_name "OK"
#define kStateTripped_name "TRIPPED"
#define kStateRampUpPause_name "RAMP_UP_PAUSE"
#define kStateRampDownPause_name "RAMP_DOWN_PAUSE"
#define kStateInterlockWait_name "INTERLOCK_WAIT"
#define kStateErrorLocal_name "ERROR_LOCAL"
#define kStateInterlock_name "INTERLOCK"
#define kStateInterlockWent_name "INTERLOCK_WENT"
#define kStateSoftInterlock_name "SOFT_INTERLOCK"
#define kStateDownloading_name "DOWNLOADING"
#define kStateMixedLo_name "MIXED_LOW"
#define kStateMixed_name "MIXED"
#define kStateProgressingDown_name "PROGRESSING_DOWN"
#define kStateProgressingUp_name "PROGRESSING_UP"
#define kStateProgressing_name "PROGRESSING"
#define kStateSaving_name "SAVING"
#define kStateReady_name "READY"
#define kStateStandby_name "STANDBY"
#define kStateOff_name "OFF"
#define kStateConfigured_name "STBY_CONFIGURED"
#define kStateInter_name "INTERMEDIATE"
#define kStateError_name "ERROR"
#define kStateNotReady_name "NOT_READY"
#define kStateOn_name "ON"
#define kStateRampUpLo_name "RAMPING_UP_LOW"
#define kStateRampDownLo_name "RAMPING_DOWN_LOW"
#define kStateRampUp_name "RAMPING_UP"
#define kStateRampDown_name "RAMPING_DOWN"
#define kStateNoControl_name "NO_CONTROL"
#define kStateDead_name "DEAD"
#define kStateElse_name "ELSE"

//State colours
#define kStateSequence0_colour "kobaltblau"
#define kStateSequence1_colour "kobaltblau"
#define kStateSequence2_colour "kobaltblau"
#define kStateSequence3_colour "kobaltblau"
#define kStateSequence4_colour "kobaltblau"
#define kStateSequence5_colour "kobaltblau"
#define kStateSequence6_colour "kobaltblau"
#define kStateSequence7_colour "kobaltblau"
#define kStateSequence8_colour "kobaltblau"
#define kStateSequence9_colour "kobaltblau"
#define kStateErrorRecover_colour "FwStateAttention3"
#define kStateSuperSafe_colour "FwStateOKNotPhysics"
#define kStatePulserL1_colour "FwStateOKPhysics"
#define kStatePulserL0_colour "FwStateOKPhysics"
#define kStatePulserOff_colour "FwStateOKNotPhysics"
#define kStateFlashStop_colour "FwStateAttention3"
#define kStateOnOnce_colour "FwStateOKPhysics"
#define kStateOnEmbedded_colour "FwStateOKPhysics"
#define kStateStbyEmbedded_colour "kobaltblau"
#define kStateOnLimited_colour "FwStateOKPhysics"
#define kStateWarmingUp_colour "kobaltblau"
#define kStateWarmUp_colour "FwStateOKNotPhysics"
#define kStateColdConfigured_colour "FwStateOKNotPhysics"
#define kStateOnTrigger_colour "FwStateOKPhysics"
#define kStateOnFreeRun_colour "FwStateOKPhysics"
#define kStateRunInhibit_colour "FwStateOKNotPhysics"
#define kStateNoState_colour "FwDead"
#define kStateOnShutterClosed_colour "FwStateOKNotPhysics"
#define kStateOnShutterOpen_colour "FwStateOKPhysics"
#define kStateEorProg_colour "FwStateAttention1"
#define kStateSorProg_colour "FwStateAttention1"
#define kStateEorFailure_colour "FwStateAttention3"
#define kStateSorFailure_colour "FwStateAttention3"
#define kStateRunOk_colour "FwStateOKPhysics"
#define kStateScrubbing_colour "FwStateOKPhysics"
#define kStateReadback_colour "FwStateOKPhysics"
#define kStatkActionStopped_colour "FwStateOKNotPhysics"
#define kStateExpert_colour "FwStateOKPhysics"
#define kStateOpen_colour "FwStateOKPhysics"
#define kStateClosed_colour "FwStateOKNotPhysics"
#define kStateLocked_colour "kobaltblau"
#define kStateReadyDet_colour "FwStateOKNotPhysics"
#define kStateOnDet_colour "FwStateOKNotPhysics"
#define kStateBeamTunDet_colour "FwStateOKNotPhysics"
#define kStateInterTunDet_colour "FwStateOKNotPhysics"
#define kStateStbyConfFero_colour "FwStateOKNotPhysics"
#define kStateStbyConfDet_colour "FwStateOKNotPhysics"
#define kStateFailure_colour "FwStateAttention3"
#define kStateReadyLocked_colour "FwStateOKPhysics"
#define kStateBeamTun_colour "FwStateOKNotPhysics"
#define kStateMovReady_colour "kobaltblau"
#define kStateMovStbyConf_colour "kobaltblau"
#define kStateMovBeamTun_colour "kobaltblau"
#define kStateCalibrating_colour "kobaltblau"
#define kStateRunning_colour "FwStateOKPhysics"
#define kStateConfDdl_colour "kobaltblau"
#define kStateIdle_colour "FwStateOKNotPhysics"
#define kStateNotOk_colour "FwStateAttention2"
#define kStateOk_colour "FwStateOKPhysics"
#define kStateRampUpPause_colour "kobaltblau"
#define kStateRampDownPause_colour "kobaltblau"
#define kStateInterlockWait_colour "FwStateAttention2"
#define kStateTripped_colour "FwStateAttention3"
#define kStateErrorLocal_colour "FwStateAttention3"
#define kStateInterlock_colour "FwStateAttention3"
#define kStateInterlockWent_colour "FwStateAttention2"
#define kStateSoftInterlock_colour "FwStateAttention2"
#define kStateDownloading_colour "kobaltblau"
#define kStateMixedLo_colour "FwStateAttention1"
#define kStateMixed_colour "FwStateAttention1"
#define kStateProgressingDown_colour "kobaltblau"
#define kStateProgressingUp_colour "kobaltblau"
#define kStateProgressing_colour "kobaltblau"
#define kStateSaving_colour "kobaltblau"
#define kStateReady_colour "FwStateOKPhysics"
#define kStateStandby_colour "FwStateOKNotPhysics"
#define kStateOff_colour "FwStateOKNotPhysics"
#define kStateConfigured_colour "FwStateOKNotPhysics"
#define kStateInter_colour "FwStateOKNotPhysics"
#define kStateError_colour "FwStateAttention3"
#define kStateNotReady_colour "FwStateOKNotPhysics"
#define kStateOn_colour "FwStateOKPhysics"
#define kStateRampUpLo_colour "kobaltblau"
#define kStateRampDownLo_colour "kobaltblau"
#define kStateRampUp_colour "kobaltblau"
#define kStateRampDown_colour "kobaltblau"
#define kStateNoControl_colour "FwStateAttention2"
#define kStateDead_colour "FwDead"

#define kActionGoFlashStop 105
#define kActionQuitScrubbing 104
#define kActionGoScrubbing 103
#define kActionQuitReadback 102
#define kActionGoReadback 101
#define kActionConfigureDone 100
#define kActionRampTo500 60
#define kActionRampTo1000 59
#define kActionGoSuperSafe 58
#define kActionGoPulserL1 57
#define kActionGoPulserL0 56
#define kActionGoPulserOff 55
#define kActionArmSync 54
#define kActionClearSOD 53
#define kActionGoOnOnce 52
#define kActionGoOnEmbedded 51
#define kActionGoStbyEmbedded 50
#define kActionGoOnLimited 49
#define kActionFail 48
#define kActionGoWarmUp 47
#define kActionGoOnFreeRun 46
#define kActionGoOnTrigger 45
#define kActionGoOnSlow 44
#define kActionAllowRun 43
#define kActionInhibitRun 42
#define kActionPrepareRun 41
#define kActionEndEorProg 40
#define kActionEndSorProg 39
#define kActionGoEorFailure 38
#define kActionGoSorFailure 37
#define kActionCloseShutter 36
#define kActionOpenShutter 35
#define kActionAckRunFailure 34
#define kActionReset 33
#define kActionEor 32
#define kActionSor 31
#define kActionSetLow 30
#define kActionSetHigh 29
#define kActionBreak 28
#define kActionStopCalib 27
#define kActionEndDaqConf 26
#define kActionStartDaqConf 25
#define kActionUnLock 24
#define kActionLock 23
#define kActionGoStbyConf 22
#define kActionGoBeamTun 21
#define kActionCalibrate 20
#define kActionSaveRecipe 19
#define kActionLoadRecipe 18
#define kActionCloseDdl 17
#define kActionGoIdle 16
#define kActionStart 15
#define kActionGoConfigure 14
#define kActionRecover 13
#define kActionAbandon 12
#define kActionStartSequence 11
#define kActionNext 10
#define kActionGoOn 9
#define kActionGoReady 8
#define kActionGoInter 7
#define kActionConf 6
#define kActionSetConf 5
#define kActionRecoverInterlock 4
#define kActionStop 3
#define kActionGoStandby 2
#define kActionGoOff 1
#define kActionElse 0

//Action names
#define kActionGoFlashStop_name "GO_STOP"
#define kActionQuitScrubbing_name "QUIT_SCRUBBING"
#define kActionGoScrubbing_name "SCRUBBING"
#define kActionQuitReadback_name "QUIT_READBACK"
#define kActionGoReadback_name "READBACK"
#define kActionConfigureDone_name "RUN"
#define kActionRampTo1000_name "RAMP_TO_1000"
#define kActionRampTo500_name "RAMP_TO_500"
#define kActionGoSuperSafe_name "GO_SUPER_SAFE"
#define kActionGoPulserL1_name "GO_PULSER_L1"
#define kActionGoPulserL0_name "GO_PULSER_L0"
#define kActionGoPulserOff_name "GO_PULSER_OFF"
#define kActionArmSync_name "ARM_SYNC"
#define kActionClearSOD_name "CLEAR_SOD"
#define kActionGoOnOnce_name "GO_ON_ONCE"
#define kActionGoOnEmbedded_name "GO_ON_EMBEDDED"
#define kActionGoStbyEmbedded_name "GO_STBY_EMBEDDED"
#define kActionGoOnLimited_name "GO_ON_LIMITED"
#define kActionFail_name "FAIL"
#define kActionGoWarmUp_name "GO_WARM_UP"
#define kActionGoOnFreeRun_name "GO_ON_FREE_RUN"
#define kActionGoOnTrigger_name "GO_ON_TRIGGER"
#define kActionGoOnSlow_name "GO_ON_SLOW"
#define kActionAllowRun_name "ALLOW_RUN"
#define kActionInhibitRun_name "INHIBIT_RUN"
#define kActionPrepareRun_name "PREPARE_FOR_RUN"
#define kActionEndEorProg_name "END_EOR_PROGRESSING"
#define kActionEndSorProg_name "END_SOR_PROGRESSING"
#define kActionGoEorFailure_name "GO_EOR_FAILURE"
#define kActionGoSorFailure_name "GO_SOR_FAILURE"
#define kActionCloseShutter_name "CLOSE_SHUTTER"
#define kActionOpenShutter_name "OPEN_SHUTTER"
#define kActionAckRunFailure_name "ACK_RUN_FAILURE"
#define kActionReset_name "RESET"
#define kActionEor_name "EOR"
#define kActionSor_name "SOR"
#define kActionSetLow_name "SET_LOW"
#define kActionSetHigh_name "SET_HIGH"
#define kActionBreak_name "BREAK"
#define kActionStopCalib_name "STOP_CALIB"
#define kActionEndDaqConf_name "END_DAQ_CONF"
#define kActionStartDaqConf_name "START_DAQ_CONF"
#define kActionUnLock_name "UNLOCK"
#define kActionLock_name "LOCK"
#define kActionGoStbyConf_name "GO_STBY_CONF"
#define kActionGoBeamTun_name "GO_BEAM_TUN"
#define kActionCalibrate_name "CALIBRATE"
#define kActionSaveRecipe_name "SAVE_RECIPE"
#define kActionLoadRecipe_name "LOAD_RECIPE"
#define kActionCloseDdl_name "CLOSE_DDL"
#define kActionGoIdle_name "GO_IDLE"
#define kActionStart_name "START"
#define kActionGoConfigure_name "GO_CONFIGURE"
#define kActionRecoverInterlock_name "RECOVER_INTERLOCK"
#define kActionAbandon_name "ABANDON"
#define kActionStartSequence_name "START_SEQUENCE"
#define kActionNext_name "NEXT"
#define kActionGoOn_name "GO_ON"
#define kActionGoStandby_name "GO_STANDBY"
#define kActionGoOff_name "GO_OFF"
#define kActionGoReady_name "GO_READY"
#define kActionGoInter_name "GO_INTERMEDIATE"
#define kActionConf_name "CONFIGURE"
#define kActionSetConf_name "SAVE_CONFIG"
#define kActionRecover_name "RECOVER"
#define kActionStop_name "STOP"
#define kActionElse_name "ELSE"

class CETransition;
class CEStateMachine;
class DeviceCommandHandler;
struct ceServiceData_t;

class CEStateMachine{
public:
  CEStateMachine(std::string name, int id, CEStateMachine* pParent);
  virtual ~CEStateMachine();
  static int updateDimDeviceState(Ser::TceServiceData* pData, int major, int minor, void* parameter);
  CETransition* FindTransition(int transitionId);
  int TriggerTransition(CETransition* pTrans);
  int ChangeState(int state);
  int GetCurrentState()               {return fState;}
  int GetDeviceId()                   {return fId;}
  std::string GetName()               {return fName;}

protected:
  int Armor();
  virtual int ArmorLocal()            {return 1;}
  int PreUpdate();
  virtual int PreUpdateLocal()        {return 1;}
  int Synchronize();
  virtual int SynchronizeLocal()      {return ReSynchronizeLocal();}
  int ReSynchronize();
  virtual int ReSynchronizeLocal()    {return kStateElse;}
  int SetFileState(int state);
  int GetFileState(int &state);
  int TriggerTransition(std::string& command);
  int TriggerTransition(std::string& name, std::string& transition);
  int StateEnterDispatcher(int state);
  int StateLeaveDispatcher(int state);
  const char* GetStateName(int state);
  int SetErrorState();
  int AddTransition(CETransition* pTransition);
  int AddSubDevice(CEStateMachine* pDevice);
  CETransition* FindTransition(std::string& transitionName);
  CEStateMachine* GetParentDevice()   {return fpParent;}
  virtual int EnterStateIDLE()        {return 0;}
  virtual int EnterStateSTANDBY()     {return 0;}
  virtual int EnterStateCONFIGURED()  {return 0;}
  virtual int EnterStateREADBACK()    {return 0;}
  virtual int EnterStateSCRUBBING()   {return 0;}
  virtual int EnterStateWARM_UP()     {return 0;}
  virtual int EnterStateON_TRIGGER()  {return 0;}
  virtual int EnterStateON_FREE_RUN() {return 0;}
  virtual int EnterStateON_LIMITED()  {return 0;}
  virtual int EnterStatePULSER_L0()   {return 0;}
  virtual int EnterStatePULSER_L1()   {return 0;}
  virtual int EnterStatePULSER_OFF()  {return 0;}
  virtual int LeaveStateIDLE()        {return 0;}
  virtual int LeaveStateRUNNING()     {return 0;}
  virtual int LeaveStateREADBACK()    {return 0;}
  virtual int LeaveStateSCRUBBING()   {return 0;}
  virtual int LeaveStatePULSER_L1()   {return 0;}
  virtual int LeaveStatePULSER_OFF()  {return 0;}
  virtual int LeaveStateON_TRIGGER()  {return 0;}
  virtual int LeaveStateON_FREE_RUN() {return 0;}
  virtual int LeaveStateON_LIMITED()  {return 0;}
  static CE_Mutex fFsmMutex;
  int fId;
  int fState;
  std::string fName;
  CEStateMachine* fpParent;
  std::vector<CETransition*> transitions;
  std::vector<CEStateMachine*> subDevices;
};

typedef int(*callbackFunction)(CEStateMachine* callbackObject);

class CETransition{
public:
  friend class CEStateMachine;
  CETransition(int id, int finalState, std::string name, callbackFunction transitionFunction, CEStateMachine* transitionObject, int numberOfInitialStates, ...);
  int IsAllowedTransition(int state);
  int GetID()                              {return fId;}
  std::string& GetName()                   {return fName;}
  int GetFinalState()                      {return fFinalState;}
  callbackFunction GetTransitionFunction() {return fTransitionFunction;}
  CEStateMachine* GetTransitionObject()    {return fTransitionObject;}
  void SetTransitionCallback(callbackFunction transitionFunction, CEStateMachine* transitionObject) {fTransitionFunction=transitionFunction; fTransitionObject=transitionObject;}

protected:
  int fId;
  int fFinalState;
  std::string fName;
  callbackFunction fTransitionFunction;
  CEStateMachine* fTransitionObject;
  std::vector<int> fInitialStates;
};

#endif //__STATEMACHINE_HPP

