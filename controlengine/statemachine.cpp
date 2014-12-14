
#include <cerrno>
#include <cmath>
#include <vector>
#include <cstdlib>
#include <fstream>
#include "statemachine.hpp"
#include "rcu_issue.h"
#include "ctr.hpp"

CE_Mutex CEStateMachine::fFsmMutex;

CEStateMachine::CEStateMachine(std::string name, int id,  CEStateMachine* pParent):fId(id),fState(kStateElse),fName(name),fpParent(pParent){
}

CEStateMachine::~CEStateMachine(){
  subDevices.clear();
  transitions.clear();
}

int CEStateMachine::Armor(){
  ArmorLocal();
  std::string name=GetName()+"_STATE";
  Ser::RegisterService(Ser::eDataTypeFloat, name.c_str(), 0.5, CEStateMachine::updateDimDeviceState, 0, 0, Ser::eDataTypeFloat, this);
  name=GetName()+"_STATENAME";
  Ser::RegisterService(Ser::eDataTypeString, name.c_str(), 0.0, CEStateMachine::updateDimDeviceState, 0, 0, Ser::eDataTypeString, this);
  std::vector<CEStateMachine*>::iterator it;
  for(it=subDevices.begin();it<subDevices.end();it++){
    CE_Event("Armoring device '%s' (%i)\n", (**it).GetName().c_str(), (**it).GetDeviceId());
    (**it).Armor();
  }
  return 1;
}

int CEStateMachine::PreUpdate(){
  PreUpdateLocal();
  std::vector<CEStateMachine*>::iterator it;
  for(it=subDevices.begin();it<subDevices.end();it++){
    (*it)->PreUpdate();
  }
  return 1;
}

int CEStateMachine::Synchronize(){
  int state=SynchronizeLocal();
  if(fState==kStateError) state=kStateError;
  std::vector<CEStateMachine*>::iterator it;
  for(it=subDevices.begin();it<subDevices.end();it++){
    if((*it)->Synchronize()==kStateError) state=kStateError;
  }
  if(state!=fState){
    CE_Debug("Device '%s' (%i) changed from state '%s' (%i) to state '%s' (%i)\n", GetName().c_str(), GetDeviceId(), GetStateName(fState), fState, GetStateName(state), state);
    ChangeState(state);
  }
  return state;
}

int CEStateMachine::ReSynchronize(){
  int state=ReSynchronizeLocal();
  if(fState==kStateError) state=kStateError;
  std::vector<CEStateMachine*>::iterator it;
  for(it=subDevices.begin();it<subDevices.end();it++){


    CE_Debug("CEStateMachine::ReSynchronize() main : %s-%s : %s-%s\n", GetName().c_str(), GetStateName(fState), (*it)->GetName().c_str(), GetStateName((*it)->ReSynchronize()));

    if((*it)->ReSynchronize()==kStateError){
      state=kStateError;
      break;
    }
    /*
    else if((*it)->GetName()=="RCU2" && 
	     (*it)->ReSynchronize()==kStateRunning){
      state=kStateRunning;
    }else if((*it)->GetName()=="RCU2" && 
	     (*it)->ReSynchronize()==kStateStandby){
      state=kStateStandby;
    }else if((*it)->GetName()=="RCU2" && 
	     (*it)->ReSynchronize()==kStateIdle){
      state=kStateIdle;
    }
    */

    ///align with the RCU2 status 
    if((*it)->GetName()=="RCU2"){
      state = (*it)->ReSynchronize();
    }
  }


  if(state!=fState){
    CE_Debug("Device '%s' (%i) changed from state '%s' (%i) to state '%s' (%i)\n", GetName().c_str(), GetDeviceId(), GetStateName(fState), fState, GetStateName(state), state);
    ChangeState(state);
  }
  return state;
}

int CEStateMachine::updateDimDeviceState(Ser::TceServiceData* pData, int major, int minor, void* parameter){
  int iResult=0;
  ((CEStateMachine*)parameter)->ReSynchronize();
  if(pData){
    if(parameter){
      switch(minor){
      case Ser::eDataTypeFloat:
	pData->fVal=(float)((CEStateMachine*)parameter)->GetCurrentState();
	break;
      case Ser::eDataTypeInt:
	pData->iVal=(int)((CEStateMachine*)parameter)->GetCurrentState();
	break;
      case Ser::eDataTypeString:
	{
	  std::string* data=(std::string*)pData->strVal;
	  if (data) {
	    const char* name=((CEStateMachine*)parameter)->GetStateName(((CEStateMachine*)parameter)->GetCurrentState());
	    if (name)
	      *data=name;
	    else
	      *data="INVALID";
	  }
	}
	break;
      }
    }
    else{
      CE_Error("missing parameter for function updateMainState, check service registration\n");
      iResult=-EFAULT;
    }
  }
  else{
    CE_Error("invalid location\n");
    iResult=-EINVAL;
  }
  return iResult;
}

int CEStateMachine::AddSubDevice(CEStateMachine* pDevice){
  if(!pDevice){
    CE_Error("No state machine device\n");
    return -1;
  }
  std::vector<CEStateMachine*>::iterator subDevice=subDevices.begin();
  while(subDevice!=subDevices.end()){
    if(*subDevice==pDevice){
      CE_Error("State machine %p already exists\n", pDevice);
      return -2;
    }
    if((*subDevice)->GetDeviceId()==pDevice->GetDeviceId()){
      CE_Error("State machine ID %i already exists\n", pDevice->GetDeviceId());
      return -3;
    }
    subDevice++;
  }
  subDevices.push_back(pDevice); 
  return 1;
}

CETransition::CETransition(int id, int finalState, std::string name, callbackFunction transitionFunction, CEStateMachine* transitionObject, int numberOfInitialStates, ...){
  fId=id;
  fFinalState=finalState;
  fName=name.c_str();
  fTransitionFunction=transitionFunction;
  fTransitionObject=transitionObject;
  va_list ap;
  va_start(ap, numberOfInitialStates);
  for(int i=0;i<numberOfInitialStates;i++){
    fInitialStates.push_back(va_arg(ap,int));
  }
  va_end(ap);
}

CETransition* CEStateMachine::FindTransition(std::string& transitionName){
  std::vector<CETransition*>::iterator transition=transitions.begin();
  while(transition!=transitions.end()){
    if(transitionName.compare((*transition)->GetName())==0){
      return *transition;
    }
    transition++;
  }
  CE_Debug("Transition '%s' not defined for device '%s' (%i)\n", transitionName.c_str(), GetName().c_str(), GetDeviceId());
  return NULL;
}

CETransition* CEStateMachine::FindTransition(int transitionId){
  std::vector<CETransition*>::iterator transition=transitions.begin();
  while(transition!=transitions.end()){
    if((*transition)->GetID()==transitionId){
      return *transition;
    }
    transition++;
  }
  CE_Debug("Transition ID %i not defined for device '%s' (%i)\n", transitionId, GetName().c_str(), GetDeviceId());
  return NULL;
}

int CEStateMachine::TriggerTransition(std::string& command){
  std::string device;
  std::string transition;
  size_t keySize=command.find(" ");
  if(keySize==std::string::npos){
    device=GetName();
    transition=command;
  }
  else{
    device=command.substr(0,keySize);
    transition=command.substr(keySize+1, command.size());
  }
  if(TriggerTransition(device, transition)<0){
    CE_Error("No transition '%s' for device '%s' from command '%s'\n", transition.c_str(), device.c_str(), command.c_str());
    return -1;
  }
  return 1;
}
 
int CEStateMachine::TriggerTransition(std::string& device, std::string& transition){
  if(device.compare(GetName())==0){
    CETransition* trans = NULL;
    trans=FindTransition(transition);
    if(!trans) trans=FindTransition(atoi(transition.c_str()));
    if(!trans){
      CE_Error("Unknown transition '%s' for device '%s'\n", transition.c_str(), device.c_str());
      return -1;
    }
    if(int iResult=TriggerTransition(trans)<0){
      CE_Debug("Transition '%s' (%i) failed with error %i for device '%s' (%i)\n", trans->GetName().c_str(), trans->GetID(), iResult, GetName().c_str(), GetDeviceId());
      return -2;
    }
    return 1;
  }
  std::vector<CEStateMachine*>::iterator subDevice=subDevices.begin();
  while(subDevice!=subDevices.end()){
    if((*subDevice)->TriggerTransition(device, transition)>0){
      return 2;
    } 
    subDevice++;
  }
  return -3;
}

int CEStateMachine::TriggerTransition(CETransition* pTrans){
  if(!pTrans){
    CE_Error("Transition does not exists\n");
    return -1;
  }
  CE_LockGuard l(fFsmMutex); 
  int current=GetCurrentState();
  int final=pTrans->GetFinalState();
  if(pTrans->IsAllowedTransition(current)<0){
    CE_Debug("Transition '%s' (%i) for device '%s' (%i) ignored, not valid from state '%s' (%i) to state '%s' (%i)\n", pTrans->GetName().c_str(), pTrans->GetID(), GetName().c_str(), GetDeviceId(), GetStateName(current), current, GetStateName(final), final);
    return 1;
  }
  if(int iResult=StateLeaveDispatcher(current)<0){
    CE_Error("Transition '%s' (%i) for device '%s' (%i) failed with error %i when leaving state '%s' (%i)\n", pTrans->GetName().c_str(), pTrans->GetID(), GetName().c_str(), GetDeviceId(), iResult, GetStateName(current), current);
    SetErrorState();
    return -1;
  }
  if(pTrans->GetTransitionFunction()){
    if(int iResult=pTrans->GetTransitionFunction()(pTrans->GetTransitionObject())<0){
      CE_Error("Transition '%s' (%i) for device '%s' (%i) failed with error %i\n", pTrans->GetName().c_str(), pTrans->GetID(), GetName().c_str(), GetDeviceId(), iResult);
      SetErrorState();
      return -2;
    }
  }
  fState=final;
  if(int iResult=StateEnterDispatcher(final)<0){
    CE_Error("Transition '%s' (%i) for device '%s' (%i) failed with error %i when entering state '%s' (%i)\n", pTrans->GetName().c_str(), pTrans->GetID(), GetName().c_str(), GetDeviceId(), iResult, GetStateName(final), final);
    SetErrorState();
    return -3;
  }
  CE_Event("Transition '%s' (%i) for device '%s' (%i) completed from state '%s' (%i) to state '%s' (%i)\n", pTrans->GetName().c_str(), pTrans->GetID(), GetName().c_str(), GetDeviceId(), GetStateName(current), current, GetStateName(final), final);
  int transition=pTrans->GetID();
  std::vector<CEStateMachine*>::iterator subDevice=subDevices.begin();
  while(subDevice!=subDevices.end()){
    CETransition* trans=(*subDevice)->FindTransition(transition);
    if(trans){
      (*subDevice)->TriggerTransition(trans);
    }
    subDevice++;
  }
  Ser::ceUpdateServices(fName.c_str());
  return 2;
}

int CEStateMachine::ChangeState(int final){
  CE_LockGuard l(fFsmMutex); 
  int current=GetCurrentState();
  if(int iResult=StateLeaveDispatcher(current)<0){
    CE_Error("State change for device '%s' (%i) failed with error %i when leaving state '%s' (%i)\n", GetName().c_str(), GetDeviceId(), iResult, GetStateName(current), current);
    SetErrorState();
    return -1;
  }
  fState=final;
  if(int iResult=StateEnterDispatcher(final)<0){
    CE_Error("State change for device '%s' (%i) failed with error %i when entering state '%s' (%i)\n", GetName().c_str(), GetDeviceId(), iResult, GetStateName(final), final);
    SetErrorState();
    return -3;
  }
  CE_Event("State change for device '%s' (%i) completed from state '%s' (%i) to state '%s' (%i)\n", GetName().c_str(), GetDeviceId(), GetStateName(current), current, GetStateName(final), final);
  return 2;
}

int CEStateMachine::SetErrorState(){
  CE_LockGuard l(fFsmMutex); 
  CE_Event("Device '%s' (%i) set to state '%s' (%i)\n", GetName().c_str(), GetDeviceId(), GetStateName(kStateError), kStateError);
  fState=kStateError;
  return 1;
}

int CEStateMachine::SetFileState(int state){
  std::string name=fName;
  if (name.length()>0) name+="_";
  name=Ctr::tmpDir+"/FEESERVER_"+name+"STATE";
  char stateChar[]="          ";
  snprintf(stateChar,11,"%i",state);
  std::ofstream stateFile(name.c_str(), std::ofstream::binary);
  stateFile.write(stateChar, strlen(stateChar));
  stateFile.close();
  return 1;
}

int CEStateMachine::GetFileState(int &state){
  std::string name=fName;
  if (name.length()>0) name+="_";
  name=Ctr::tmpDir+"/FEESERVER_"+name+"STATE";
  std::ifstream stateFile(name.c_str(), std::ifstream::binary);
  char stateChar[]="          ";
  stateFile.read(stateChar, strlen(stateChar));
  state=atoi(stateChar);
  stateFile.close();
  return 1;
}

int CEStateMachine::AddTransition(CETransition* pTransition){
  if(!pTransition){
    CE_Error("No transition\n");
    return -1;
  }
  CE_Event("Adding transition '%s' (%i) with final state '%s' (%i) for device '%s' (%i)\n", pTransition->GetName().c_str(), pTransition->GetID(), GetStateName(pTransition->GetFinalState()), pTransition->GetFinalState(), GetName().c_str(), GetDeviceId());
  std::vector<CETransition*>::iterator transition=transitions.begin();
  while(transition!=transitions.end()){
    if((*transition)==pTransition){
      CE_Error("Transition %p already exists\n", pTransition);
      return -2;
    }
    if((*transition)->GetID()==pTransition->GetID()){
      CE_Error("Transition ID %i already exists\n", pTransition->GetID());
      return -3;
    }
    transition++;
  }
  transitions.push_back(pTransition); 
  return 1;
}

int CETransition::IsAllowedTransition(int state){
  std::vector<int>::iterator initialState=fInitialStates.begin();
  while(initialState!=fInitialStates.end()){
    if((*initialState)==state) return 1;
    initialState++;
  }
  return -1;
}

int CEStateMachine::StateEnterDispatcher(int state){
  switch(state){
    case kStateIdle:       return EnterStateIDLE();
    case kStateStandby:    return EnterStateSTANDBY();
    case kStateConfigured: return EnterStateCONFIGURED();
    case kStateReadback:   return EnterStateREADBACK();
    case kStateScrubbing:  return EnterStateSCRUBBING();
    case kStatePulserL0:   return EnterStatePULSER_L0();
    case kStatePulserL1:   return EnterStatePULSER_L1();
    case kStatePulserOff:  return EnterStatePULSER_OFF();
    case kStateWarmUp:     return EnterStateWARM_UP();
    case kStateOnTrigger:  return EnterStateON_TRIGGER();
    case kStateOnFreeRun:  return EnterStateON_FREE_RUN();
    case kStateOnLimited:  return EnterStateON_LIMITED();
    default:               return 0;
  }
}

int CEStateMachine::StateLeaveDispatcher(int state){
  switch(state){
    case kStateIdle:       return LeaveStateIDLE();
    case kStateRunning:    return LeaveStateRUNNING();
    case kStateReadback:   return LeaveStateREADBACK();
    case kStateScrubbing:  return LeaveStateSCRUBBING();
    case kStatePulserL1:   return LeaveStatePULSER_L1();
    case kStatePulserOff:  return LeaveStatePULSER_OFF();
    case kStateOnTrigger:  return LeaveStateON_TRIGGER();
    case kStateOnFreeRun:  return LeaveStateON_FREE_RUN();
    case kStateOnLimited:  return LeaveStateON_LIMITED();
    default:               return 0;
  }
}

const char* CEStateMachine::GetStateName(int state){
  switch(state){
    case kStateSequence0:       return kStateSequence0_name;
    case kStateSequence1:       return kStateSequence1_name;
    case kStateSequence2:       return kStateSequence2_name;
    case kStateSequence3:       return kStateSequence3_name;
    case kStateSequence4:       return kStateSequence4_name;
    case kStateSequence5:       return kStateSequence5_name;
    case kStateSequence6:       return kStateSequence6_name;
    case kStateSequence7:       return kStateSequence7_name;
    case kStateSequence8:       return kStateSequence8_name;
    case kStateSequence9:       return kStateSequence9_name;
    case kStateErrorRecover:    return kStateErrorRecover_name;
    case kStateSuperSafe:       return kStateSuperSafe_name;
    case kStatePulserL1:        return kStatePulserL1_name;
    case kStatePulserL0:        return kStatePulserL0_name;
    case kStatePulserOff:       return kStatePulserOff_name;
    case kStateFlashStop:       return kStateFlashStop_name;
    case kStateOnOnce:          return kStateOnOnce_name;
    case kStateOnEmbedded:      return kStateOnEmbedded_name;
    case kStateStbyEmbedded:    return kStateStbyEmbedded_name;
    case kStateOnLimited:       return kStateOnLimited_name;
    case kStateWarmingUp:       return kStateWarmingUp_name;
    case kStateWarmUp:          return kStateWarmUp_name;
    case kStateColdConfigured:  return kStateColdConfigured_name;
    case kStateOnTrigger:       return kStateOnTrigger_name;
    case kStateOnFreeRun:       return kStateOnFreeRun_name;
    case kStateRunInhibit:      return kStateRunInhibit_name;
    case kStateNoState:         return kStateNoState_name;
    case kStateOnShutterClosed: return kStateOnShutterClosed_name;
    case kStateOnShutterOpen:   return kStateOnShutterOpen_name;
    case kStateEorProg:         return kStateEorProg_name;
    case kStateSorProg:         return kStateSorProg_name;
    case kStateEorFailure:      return kStateEorFailure_name;
    case kStateSorFailure:      return kStateSorFailure_name;
    case kStateRunOk:           return kStateRunOk_name;
    case kStateScrubbing:       return kStateScrubbing_name;
    case kStateReadback:        return kStateReadback_name;
    case kStatkActionStopped:   return kStatkActionStopped_name;
    case kStateExpert:          return kStateExpert_name;
    case kStateOpen:            return kStateOpen_name;
    case kStateClosed:          return kStateClosed_name;
    case kStateLocked:          return kStateLocked_name;
    case kStateReadyDet:        return kStateReadyDet_name;
    case kStateOnDet:           return kStateOnDet_name;
    case kStateBeamTunDet:      return kStateBeamTunDet_name;
    case kStateInterTunDet:     return kStateInterTunDet_name;
    case kStateStbyConfFero:    return kStateStbyConfFero_name;
    case kStateStbyConfDet:     return kStateStbyConfDet_name;
    case kStateFailure:         return kStateFailure_name;
    case kStateReadyLocked:     return kStateReadyLocked_name;
    case kStateBeamTun:         return kStateBeamTun_name;
    case kStateMovReady:        return kStateMovReady_name;
    case kStateMovStbyConf:     return kStateMovStbyConf_name;
    case kStateMovBeamTun:      return kStateMovBeamTun_name;
    case kStateCalibrating:     return kStateCalibrating_name;
    case kStateRunning:         return kStateRunning_name;
    case kStateConfDdl:         return kStateConfDdl_name;
    case kStateIdle:            return kStateIdle_name;
    case kStateNotOk:           return kStateNotOk_name;
    case kStateOk:              return kStateOk_name;
    case kStateTripped:         return kStateTripped_name;
    case kStateRampUpPause:     return kStateRampUpPause_name;
    case kStateRampDownPause:   return kStateRampDownPause_name;
    case kStateInterlockWait:   return kStateInterlockWait_name;
    case kStateErrorLocal:      return kStateErrorLocal_name;
    case kStateInterlock:       return kStateInterlock_name;
    case kStateInterlockWent:   return kStateInterlockWent_name;
    case kStateSoftInterlock:   return kStateSoftInterlock_name;
    case kStateDownloading:     return kStateDownloading_name;
    case kStateMixedLo:         return kStateMixedLo_name;
    case kStateMixed:           return kStateMixed_name;
    case kStateProgressingDown: return kStateProgressingDown_name;
    case kStateProgressingUp:   return kStateProgressingUp_name;
    case kStateProgressing:     return kStateProgressing_name;
    case kStateSaving:          return kStateSaving_name;
    case kStateReady:           return kStateReady_name;
    case kStateStandby:         return kStateStandby_name;
    case kStateOff:             return kStateOff_name;
    case kStateConfigured:      return kStateConfigured_name;
    case kStateInter:           return kStateInter_name;
    case kStateError:           return kStateError_name;
    case kStateNotReady:        return kStateNotReady_name;
    case kStateOn:              return kStateOn_name;
    case kStateRampUpLo:        return kStateRampUpLo_name;
    case kStateRampDownLo:      return kStateRampDownLo_name;
    case kStateRampUp:          return kStateRampUp_name;
    case kStateRampDown:        return kStateRampDown_name;
    case kStateNoControl:       return kStateNoControl_name;
    case kStateDead:            return kStateDead_name;
    case kStateElse:            return kStateElse_name;
    default:                    return "";
  }
} 
