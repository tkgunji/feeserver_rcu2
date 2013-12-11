#include <cstdlib>
#include <typeinfo>
#include <cstdio>
#include "phos.hpp"

/******************************************************************************/

void ce_exec_loop(){
  CE_Event("This is ControlEngine for PHOS\n");
  if(!Ctr::fpInstance) Ctr::fpInstance=new CtrRcuPhs();
  if(Ctr::fpInstance){
    CE_Event("ControlEngine '%s' (%i) of type '%s' created at %p\n", Ctr::fpInstance->GetName().c_str(), Ctr::fpInstance->GetDeviceId(), typeid(*Ctr::fpInstance).name(), Ctr::fpInstance);
    Ctr::fpInstance->RunCE();
  }
  else CE_Warning("ControlEngine not created at %p\n", Ctr::fpInstance);
  return;
}

/******************************************************************************/

CtrRcuPhs::CtrRcuPhs(){
  if(fpMsgbuffer) fpMsgbuffer->SingleWrite(V2_FECSWREG,1);

  if(!fpPhs) fpPhs=new DevPhs(subDevices.size(), this,  fpFecaccess);
  if(fpPhs){
    AddSubDevice(fpPhs);
    CE_Event("DevPhs '%s' (%i) of type '%s' created at %p with DevFecaccess at %p\n", fpPhs->GetName().c_str(), fpPhs->GetDeviceId(), typeid(*fpPhs).name(), fpPhs, fpFecaccess);
  }
  else CE_Warning("DevPhs not created at %p with DevFecaccess %p\n", fpPhs, fpFecaccess);

  if(fpFecaccess){
    const char* fecMonitor = NULL;
    char number[]="  ";
    fecMonitor = getenv("FEESERVER_FEC_MONITOR");
    //Turn on all cards by default
    if(!fecMonitor) fecMonitor="11111111111111101111111111111110";
    for(uint32_t i=0;i<32;i++){
      if(fecMonitor[i] && fecMonitor[i]=='1'){
        snprintf(number,3,"%.2u",i);
        if(i==0 || i==16) fpFecs[i]=new DevFecTru(number, subDevices.size(), this, fpFecaccess, i);
        else              fpFecs[i]=new DevFecPhs(number, subDevices.size(), this, fpFecaccess, i);
        AddSubDevice(fpFecs[i]);
        CE_Event("DevFec%.2u '%s' (%i) of type '%s' created at %p with DevFecaccess at %p\n", i, fpFecs[i]->GetName().c_str(), fpFecs[i]->GetDeviceId(), typeid(*fpFecs[i]).name(), fpFecs[i], fpFecaccess);
      }
      else{
        fpFecs[i]=NULL;
        CE_Event("DevFec%.2u not created at %p with DevFecaccess at %p\n", i, fpFecs[i], fpFecaccess);
      }
    }
  }
}

