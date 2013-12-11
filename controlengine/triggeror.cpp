#include <cerrno>
#include <cstdlib>
#include <cstdio>
#include <typeinfo>
#include "triggeror.hpp"
#include "rcu_issue.h"
#include "dcb.hpp"
#include "ser.hpp"

/******************************************************************************/

void ce_exec_loop(){
  CE_Event("This is ControlEngine for TriggerOr\n");
  if(!Ctr::fpInstance) Ctr::fpInstance=new CtrTor();
  if(Ctr::fpInstance){
    CE_Event("ControlEngine '%s' (%i) of type '%s' created at %p\n", Ctr::fpInstance->GetName().c_str(), Ctr::fpInstance->GetDeviceId(), typeid(*Ctr::fpInstance).name(), Ctr::fpInstance);
    Ctr::fpInstance->RunCE();
  }
  else CE_Warning("ControlEngine not created at %p\n", Ctr::fpInstance);
  return;
}

/******************************************************************************/

CtrTor::CtrTor(): fpTor(NULL){
  if     (!fpMsgbuffer && fHwType==1) fpMsgbuffer=new DevMsgbufferDcb();
  else if(!fpMsgbuffer && fHwType==2) fpMsgbuffer=new DevMsgbufferDummy();
  if(fpMsgbuffer) CE_Event("DevMsgbuffer of type '%s' created at %p with HwType %i\n", typeid(*fpMsgbuffer).name(), fpMsgbuffer, fHwType);
  else CE_Warning("DevMsgbuffer not created at %p with HwType %i\n", fpMsgbuffer, fHwType);
  if(!fpTor && fpMsgbuffer) fpTor=new DevTor(subDevices.size(), this, fpMsgbuffer);
  if(fpTor){
    AddSubDevice(fpTor);
    CE_Event("DevTor '%s' (%i) of type '%s' created at %p with DevMsgbuffer at %p\n", fpTor->GetName().c_str(), fpTor->GetDeviceId(), typeid(*fpTor).name(), fpTor, fpMsgbuffer);
  }
  else CE_Warning("DevTor not created at %p with DevMsgbuffer %p\n", fpTor, fpMsgbuffer);
}

CtrTor::~CtrTor(){
  fpMsgbuffer=NULL; // deleted in sub-device handling
  fpTor=NULL;
}

/******************************************************************************/

DevTor::DevTor(int id,  CEStateMachine* pParent, DevMsgbuffer* msgbuffer):Dev("TOR", id, pParent),fpMsgbuffer(msgbuffer){
}
int DevTor::ReSynchronizeLocal(){
  if(!fpMsgbuffer) return kStateError;
  return kStateStandby;
}

/*******************
* Services
*******************/
int DevTor::ArmorLocal(){
  std::string name=GetName();
  name+="_";
  fServices.push_back(new SerMbAddrS(name+"TRIG0_OPTIONCODE"   	, Trig0_OptionCode, 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"TRIG0_SIGNATURE"    	, Trig0_Signature, 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"TRIG0_MESSAGEHEADER"	, Trig0_MessageHeader, 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"TRIG0_PROGRATELOW"   , Trig0_ProgRateLow, 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"TRIG0_PROGRATEHIGH"  , Trig0_ProgRateHigh, 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"TRIG0_PROGDELAY"     , Trig0_ProgDelay, 1, 0, fpMsgbuffer));
  
  fServices.push_back(new SerMbAddrS(name+"TRIG1_OPTIONCODE"    , Trig1_OptionCode, 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"TRIG1_SIGNATURE"    	, Trig1_Signature, 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"TRIG1_MESSAGEHEADER" , Trig1_MessageHeader, 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"TRIG1_PROGRATELOW"   , Trig1_ProgRateLow, 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"TRIG1_PROGRATEHIGH"  , Trig1_ProgRateHigh, 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"TRIG1_PROGDELAY"     , Trig1_ProgDelay, 1, 0, fpMsgbuffer));
  
  fServices.push_back(new SerMbAddrS(name+"TRIG1M_OPTIONCODE"   , Trig1M_OptionCode, 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"TRIG1M_SIGNATURE"   	, Trig1M_Signature, 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"TRIG1M_MESSAGEHEADER", Trig1M_MessageHeader, 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"TRIG1M_PROGRATELOW"  , Trig1M_ProgRateLow, 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"TRIG1M_PROGRATEHIGH" , Trig1M_ProgRateHigh, 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"TRIG1M_PROGDELAY"    , Trig1M_ProgDelay, 1, 0, fpMsgbuffer));
  
  fServices.push_back(new SerMbAddrS(name+"TRIG1H_OPTIONCODE"    , Trig1H_OptionCode, 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"TRIG1H_SIGNATURE"     , Trig1H_Signature, 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"TRIG1H_MESSAGEHEADER" , Trig1H_MessageHeader, 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"TRIG1H_PROGRATELOW"   , Trig1H_ProgRateLow, 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"TRIG1H_PROGRATEHIGH"  , Trig1H_ProgRateHigh, 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"TRIG1H_PROGDELAY"     , Trig1H_ProgDelay, 1, 0, fpMsgbuffer));
  
  fServices.push_back(new SerMbAddrS(name+"L1L_THRESHOLD" , L1L_Threshold, 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"L1M_THRESHOLD" , L1M_Threshold, 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"L1H_THRESHOLD" , L1H_Threshold, 1, 0, fpMsgbuffer));

  fServices.push_back(new SerMbAddrS(name+"L0TRIGGERMASK_12" , L0TriggerMask_12, 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"L0TRIGGERMASK_34" , L0TriggerMask_34, 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"L0TRIGGERMASK_5" , L0TriggerMask_5, 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"CTRLRESERVE_R" , CtrlReserve_r, 1, 0, fpMsgbuffer));

  /**
  Counters
  **/

  fServices.push_back(new SerMbAddrS(name+"L0LOWCOUNTER" , L0LowCounter, 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"L0HIGHCOUNTER" , L0HighCounter, 1, 0, fpMsgbuffer));

  fServices.push_back(new SerMbAddrS(name+"L1LLOWCOUNTER" , L0LowCounter, 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"L1LHIGHCOUNTER" , L0HighCounter, 1, 0, fpMsgbuffer));

  fServices.push_back(new SerMbAddrS(name+"L1MLOWCOUNTER" , L0LowCounter, 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"L1MHIGHCOUNTER" , L0HighCounter, 1, 0, fpMsgbuffer));

  fServices.push_back(new SerMbAddrS(name+"L1HLOWCOUNTER" , L0LowCounter, 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"L1HHIGHCOUNTER" , L0HighCounter, 1, 0, fpMsgbuffer));

  fServices.push_back(new SerMbAddrS(name+"FIRMWAREVERSION" , FirmwareVersion, 1, 0, fpMsgbuffer));

  fServices.push_back(new SerMbAddrS(name+"M11INPTRIGCOUNT" , M11InpTrigCount, 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"M12INPTRIGCOUNT" , M12InpTrigCount, 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"M13INPTRIGCOUNT" , M13InpTrigCount, 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"M14INPTRIGCOUNT" , M14InpTrigCount, 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"M15INPTRIGCOUNT" , M15InpTrigCount, 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"M16INPTRIGCOUNT" , M16InpTrigCount, 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"M17INPTRIGCOUNT" , M17InpTrigCount, 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"M18INPTRIGCOUNT" , M18InpTrigCount, 1, 0, fpMsgbuffer));

  fServices.push_back(new SerMbAddrS(name+"M21INPTRIGCOUNT" , M21InpTrigCount, 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"M22INPTRIGCOUNT" , M22InpTrigCount, 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"M23INPTRIGCOUNT" , M23InpTrigCount, 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"M24INPTRIGCOUNT" , M24InpTrigCount, 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"M25INPTRIGCOUNT" , M25InpTrigCount, 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"M26INPTRIGCOUNT" , M26InpTrigCount, 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"M27INPTRIGCOUNT" , M27InpTrigCount, 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"M28INPTRIGCOUNT" , M28InpTrigCount, 1, 0, fpMsgbuffer));

  fServices.push_back(new SerMbAddrS(name+"M31INPTRIGCOUNT" , M31InpTrigCount, 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"M32INPTRIGCOUNT" , M32InpTrigCount, 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"M33INPTRIGCOUNT" , M33InpTrigCount, 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"M34INPTRIGCOUNT" , M34InpTrigCount, 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"M35INPTRIGCOUNT" , M35InpTrigCount, 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"M36INPTRIGCOUNT" , M36InpTrigCount, 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"M37INPTRIGCOUNT" , M37InpTrigCount, 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"M38INPTRIGCOUNT" , M38InpTrigCount, 1, 0, fpMsgbuffer));

  fServices.push_back(new SerMbAddrS(name+"DBGRDOUT4LOW" , DbgRdout4Low, 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"DBGRDOUT4HIGH" , DbgRdout4High, 1, 0, fpMsgbuffer));

  fServices.push_back(new SerMbAddrS(name+"MAXPHOS32" , MaxPhos32, 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"MAXPHOS3" , MaxPhos3, 1, 0, fpMsgbuffer));
  fServices.push_back(new SerMbAddrS(name+"MAXPHOS123" , MaxPhos123, 1, 0, fpMsgbuffer));

  return 0;
}

/*******************
* Commands
*******************/


int DevTor::issue(uint32_t cmd, uint32_t parameter, const char* pData, int iDataSize, std::vector<uint32_t>& rb){
  int size=0;
  CE_Debug("DevTor::issue cmd=%#x parameter=%#x datasize=%d\n", cmd, parameter, iDataSize);
  switch (cmd) {
  //Write single registers
  case WRITE_TRIG0_OPTIONCODE:
    fpMsgbuffer->SingleWrite(Trig0_OptionCode, *(uint32_t*)pData);
  break;
  case WRITE_TRIG0_SIGNATURE:
    fpMsgbuffer->SingleWrite(Trig0_Signature, *(uint32_t*)pData);
  break;
  case WRITE_TRIG0_MESSAGEHEADER:
    fpMsgbuffer->SingleWrite(Trig0_MessageHeader, *(uint32_t*)pData);
  break;
  case WRITE_TRIG0_PROGRATELOW:
    fpMsgbuffer->SingleWrite(Trig0_ProgRateLow, *(uint32_t*)pData);
  break;
  case WRITE_TRIG0_PROGRATEHIGH:
    fpMsgbuffer->SingleWrite(Trig0_ProgRateHigh, *(uint32_t*)pData);
  break;
  case WRITE_TRIG0_PROGDELAY:
    fpMsgbuffer->SingleWrite(Trig0_ProgDelay, *(uint32_t*)pData);
  break;
  case WRITE_TRIG1_OPTIONCODE:
    fpMsgbuffer->SingleWrite(Trig1_OptionCode, *(uint32_t*)pData);
  break;
  case WRITE_TRIG1_SIGNATURE:
    fpMsgbuffer->SingleWrite(Trig1_Signature, *(uint32_t*)pData);
  break;
  case WRITE_TRIG1_MESSAGEHEADER:
    fpMsgbuffer->SingleWrite(Trig1_MessageHeader, *(uint32_t*)pData);
  break;
  case WRITE_TRIG1_PROGRATELOW:
    fpMsgbuffer->SingleWrite(Trig1_ProgRateLow, *(uint32_t*)pData);
  break;
  case WRITE_TRIG1_PROGRATEHIGH:
    fpMsgbuffer->SingleWrite(Trig1_ProgRateHigh, *(uint32_t*)pData);
  break;
  case WRITE_TRIG1_PROGDELAY:
    fpMsgbuffer->SingleWrite(Trig1_ProgDelay, *(uint32_t*)pData);
  break;
  case WRITE_TRIG1M_OPTIONCODE:
    fpMsgbuffer->SingleWrite(Trig1M_OptionCode, *(uint32_t*)pData);
  break;
  case WRITE_TRIG1M_SIGNATURE:
    fpMsgbuffer->SingleWrite(Trig1M_Signature, *(uint32_t*)pData);
  break;
  case WRITE_TRIG1M_MESSAGEHEADER:
    fpMsgbuffer->SingleWrite(Trig1M_MessageHeader, *(uint32_t*)pData);
  break;
  case WRITE_TRIG1M_PROGRATELOW:
    fpMsgbuffer->SingleWrite(Trig1M_ProgRateLow, *(uint32_t*)pData);
  break;
  case WRITE_TRIG1M_PROGRATEHIGH:
    fpMsgbuffer->SingleWrite(Trig1M_ProgRateHigh, *(uint32_t*)pData);
  break;
  case WRITE_TRIG1M_PROGDELAY:
    fpMsgbuffer->SingleWrite(Trig1M_ProgDelay, *(uint32_t*)pData);
  break;
  case WRITE_TRIG1H_OPTIONCODE:
    fpMsgbuffer->SingleWrite(Trig1H_OptionCode, *(uint32_t*)pData);
  break;
  case WRITE_TRIG1H_SIGNATURE:
    fpMsgbuffer->SingleWrite(Trig1H_Signature, *(uint32_t*)pData);
  break;
  case WRITE_TRIG1H_MESSAGEHEADER:
    fpMsgbuffer->SingleWrite(Trig1H_MessageHeader, *(uint32_t*)pData);
  break;
  case WRITE_TRIG1H_PROGRATELOW:
    fpMsgbuffer->SingleWrite(Trig1H_ProgRateLow, *(uint32_t*)pData);
  break;
  case WRITE_TRIG1H_PROGRATEHIGH:
    fpMsgbuffer->SingleWrite(Trig1H_ProgRateHigh, *(uint32_t*)pData);
  break;
  case WRITE_TRIG1H_PROGDELAY:
    fpMsgbuffer->SingleWrite(Trig1H_ProgDelay, *(uint32_t*)pData);
  break;
  
  
  case WRITE_L1L_THRESHOLD:
    fpMsgbuffer->SingleWrite(L1L_Threshold, *(uint32_t*)pData);
  break;
  case WRITE_L1M_THRESHOLD:
    fpMsgbuffer->SingleWrite(L1M_Threshold, *(uint32_t*)pData);
  break;
  case WRITE_L1H_THRESHOLD:
    fpMsgbuffer->SingleWrite(L1H_Threshold, *(uint32_t*)pData);
  break;
  case WRITE_L0TRIGGERMASK_12:
    fpMsgbuffer->SingleWrite(L0TriggerMask_12, *(uint32_t*)pData);
  break;
  case WRITE_L0TRIGGERMASK_34:
    fpMsgbuffer->SingleWrite(L0TriggerMask_34, *(uint32_t*)pData);
  break;
  case WRITE_L0TRIGGERMASK_5:
    fpMsgbuffer->SingleWrite(L0TriggerMask_5, *(uint32_t*)pData);
  break;
    case WRITE_CTRL_RESERVE_R:
    fpMsgbuffer->SingleWrite(CtrlReserve_r, *(uint32_t*)pData);
  break;
  default:
    CE_Warning("unrecognized command id (%#x)\n", cmd);
    return -ENOSYS;
  }
  size=1*sizeof(uint32_t);
  return size;
}

/*******************
* Highlevel Handler
*******************/

int DevTor::HighLevelHandler(const char* pCommand, std::vector<uint32_t>& rb){
  int keySize=0;
  uint32_t cmd=0;
  if(!pCommand) return 0;
  if      (strncmp(pCommand, "WRITE_L0TRIGGERMASK_5"       	, keySize=strlen("WRITE_L0TRIGGERMASK_5"       ))==0) cmd=WRITE_L0TRIGGERMASK_5;
  else if (strncmp(pCommand, "WRITE_L0TRIGGERMASK_34"   	, keySize=strlen("WRITE_L0TRIGGERMASK_34"   ))==0) cmd=WRITE_L0TRIGGERMASK_34;
  else if (strncmp(pCommand, "WRITE_L0TRIGGERMASK_12"  		, keySize=strlen("WRITE_L0TRIGGERMASK_12"  ))==0) cmd=WRITE_L0TRIGGERMASK_12;
  else if (strncmp(pCommand, "WRITE_L1H_THRESHOLD"  		, keySize=strlen("WRITE_L1H_THRESHOLD"  ))==0) cmd=WRITE_L1H_THRESHOLD;
  else if (strncmp(pCommand, "WRITE_L1M_THRESHOLD"  		, keySize=strlen("WRITE_L1M_THRESHOLD"  ))==0) cmd=WRITE_L1M_THRESHOLD;
  else if (strncmp(pCommand, "WRITE_L1L_THRESHOLD"    		, keySize=strlen("WRITE_L1L_THRESHOLD"    ))==0) cmd=WRITE_L1L_THRESHOLD;
  
  else if (strncmp(pCommand, "WRITE_TRIG1H_PROGDELAY"    	, keySize=strlen("WRITE_TRIG1H_PROGDELAY"    ))==0) cmd=WRITE_TRIG1H_PROGDELAY;
  else if (strncmp(pCommand, "WRITE_TRIG1H_PROGRATEHIGH"	, keySize=strlen("WRITE_TRIG1H_PROGRATEHIGH"))==0) cmd=WRITE_TRIG1H_PROGRATEHIGH;
  else if (strncmp(pCommand, "WRITE_TRIG1H_PROGRATELOW"		, keySize=strlen("WRITE_TRIG1H_PROGRATELOW"))==0) cmd=WRITE_TRIG1H_PROGRATELOW;
  else if (strncmp(pCommand, "WRITE_TRIG1H_SIGNATURE" 		, keySize=strlen("WRITE_TRIG1H_SIGNATURE"  ))==0) cmd=WRITE_TRIG1H_SIGNATURE;
  else if (strncmp(pCommand, "WRITE_TRIG1H_OPTIONCODE" 		, keySize=strlen("WRITE_TRIG1H_OPTIONCODE"  ))==0) cmd=WRITE_TRIG1H_OPTIONCODE;
  else if (strncmp(pCommand, "WRITE_TRIG1H_MESSAGEHEADER" 	, keySize=strlen("WRITE_TRIG1H_MESSAGEHEADER"  ))==0) cmd=WRITE_TRIG1H_MESSAGEHEADER;
  
  else if (strncmp(pCommand, "WRITE_TRIG1M_PROGDELAY"    	, keySize=strlen("WRITE_TRIG1M_PROGDELAY"    ))==0) cmd=WRITE_TRIG1M_PROGDELAY;
  else if (strncmp(pCommand, "WRITE_TRIG1M_PROGRATEHIGH"	, keySize=strlen("WRITE_TRIG1M_PROGRATEHIGH"))==0) cmd=WRITE_TRIG1M_PROGRATEHIGH;
  else if (strncmp(pCommand, "WRITE_TRIG1M_PROGRATELOW"		, keySize=strlen("WRITE_TRIG1M_PROGRATELOW"))==0) cmd=WRITE_TRIG1M_PROGRATELOW;
  else if (strncmp(pCommand, "WRITE_TRIG1M_SIGNATURE" 		, keySize=strlen("WRITE_TRIG1M_SIGNATURE"  ))==0) cmd=WRITE_TRIG1M_SIGNATURE;
  else if (strncmp(pCommand, "WRITE_TRIG1M_OPTIONCODE" 		, keySize=strlen("WRITE_TRIG1M_OPTIONCODE"  ))==0) cmd=WRITE_TRIG1M_OPTIONCODE;
  else if (strncmp(pCommand, "WRITE_TRIG1M_MESSAGEHEADER" 	, keySize=strlen("WRITE_TRIG1M_MESSAGEHEADER"  ))==0) cmd=WRITE_TRIG1M_MESSAGEHEADER;
  
  else if (strncmp(pCommand, "WRITE_TRIG1_PROGDELAY"    	, keySize=strlen("WRITE_TRIG1_PROGDELAY"    ))==0) cmd=WRITE_TRIG1_PROGDELAY;
  else if (strncmp(pCommand, "WRITE_TRIG1_PROGRATEHIGH"		, keySize=strlen("WRITE_TRIG1_PROGRATEHIGH"))==0) cmd=WRITE_TRIG1_PROGRATEHIGH;
  else if (strncmp(pCommand, "WRITE_TRIG1_PROGRATELOW"		, keySize=strlen("WRITE_TRIG1_PROGRATELOW"))==0) cmd=WRITE_TRIG1_PROGRATELOW;
  else if (strncmp(pCommand, "WRITE_TRIG1_SIGNATURE" 		, keySize=strlen("WRITE_TRIG1_SIGNATURE"  ))==0) cmd=WRITE_TRIG1_SIGNATURE;
  else if (strncmp(pCommand, "WRITE_TRIG1_OPTIONCODE" 		, keySize=strlen("WRITE_TRIG1_OPTIONCODE"  ))==0) cmd=WRITE_TRIG1_OPTIONCODE;
  else if (strncmp(pCommand, "WRITE_TRIG1_MESSAGEHEADER" 	, keySize=strlen("WRITE_TRIG1_MESSAGEHEADER"  ))==0) cmd=WRITE_TRIG1_MESSAGEHEADER;
  
  else if (strncmp(pCommand, "WRITE_TRIG0_PROGDELAY"    	, keySize=strlen("WRITE_TRIG0_PROGDELAY"    ))==0) cmd=WRITE_TRIG0_PROGDELAY;
  else if (strncmp(pCommand, "WRITE_TRIG0_PROGRATEHIGH"		, keySize=strlen("WRITE_TRIG0_PROGRATEHIGH"))==0) cmd=WRITE_TRIG0_PROGRATEHIGH;
  else if (strncmp(pCommand, "WRITE_TRIG0_PROGRATELOW"		, keySize=strlen("WRITE_TRIG0_PROGRATELOW"))==0) cmd=WRITE_TRIG0_PROGRATELOW;
  else if (strncmp(pCommand, "WRITE_TRIG0_SIGNATURE" 		, keySize=strlen("WRITE_TRIG0_SIGNATURE"  ))==0) cmd=WRITE_TRIG0_SIGNATURE;
  else if (strncmp(pCommand, "WRITE_TRIG0_OPTIONCODE" 		, keySize=strlen("WRITE_TRIG0_OPTIONCODE"  ))==0) cmd=WRITE_TRIG0_OPTIONCODE;
  else if (strncmp(pCommand, "WRITE_TRIG0_MESSAGEHEADER" 	, keySize=strlen("WRITE_TRIG0_MESSAGEHEADER"  ))==0) cmd=WRITE_TRIG0_MESSAGEHEADER;

  else if (strncmp(pCommand, "WRITE_L1L_THRESHOLD"    	, keySize=strlen("WRITE_L1L_THRESHOLD"    ))==0) cmd=WRITE_L1L_THRESHOLD;
  else if (strncmp(pCommand, "WRITE_L1M_THRESHOLD"    	, keySize=strlen("WRITE_L1L_THRESHOLD"    ))==0) cmd=WRITE_L1L_THRESHOLD;
  else if (strncmp(pCommand, "WRITE_L1H_THRESHOLD"    	, keySize=strlen("WRITE_L1L_THRESHOLD"    ))==0) cmd=WRITE_L1L_THRESHOLD;
  else if (strncmp(pCommand, "WRITE_L0TRIGGERMASK_12" 		, keySize=strlen("WRITE_L0TRIGGERMASK_12"  ))==0) cmd=WRITE_L0TRIGGERMASK_12;
  else if (strncmp(pCommand, "WRITE_L0TRIGGERMASK_34" 		, keySize=strlen("WRITE_L0TRIGGERMASK_34"  ))==0) cmd=WRITE_L0TRIGGERMASK_34;
  else if (strncmp(pCommand, "WRITE_L0TRIGGERMASK_5" 		, keySize=strlen("WRITE_L0TRIGGERMASK_5"  ))==0) cmd=WRITE_L0TRIGGERMASK_5;

  else if (strncmp(pCommand, "WRITE_CTRL_RESERVE_R" 	, keySize=strlen("WRITE_CTRL_RESERVE_R"  ))==0) cmd=WRITE_CTRL_RESERVE_R;
  if(cmd<=0 && keySize<=0) return -EINVAL;
  const char* pBuffer=pCommand;
  pBuffer+=keySize;
  int len=strlen(pCommand);
  len-=keySize;
  while(*pBuffer==' ' && *pBuffer!=0 && len>0) {pBuffer++; len--;} // skip all blanks
  uint32_t data=0;
  int iResult=0;
  switch(cmd){
  default:
    sscanf(pBuffer, "%d", &data);
    iResult=issue(cmd, 0, (const char*)&data, 4, rb);
    break;
  }
  return iResult;
}

int DevTor::GetGroupId(){
  return FEESVR_CMD_TOR;
}

