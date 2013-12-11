#include <cstdlib>
#include <typeinfo>
#include <cstdio>
#include "tpc2.hpp"
#include "tpc_common.hpp"

/******************************************************************************/

void ce_exec_loop(){
  CE_Event("This is ControlEngine for TPC2\n");
  if(!Ctr::fpInstance) Ctr::fpInstance=new CtrRcu2Tpc();
  if(Ctr::fpInstance){
    CE_Event("ControlEngine '%s' (%i) of type '%s' created at %p\n", Ctr::fpInstance->GetName().c_str(), Ctr::fpInstance->GetDeviceId(), typeid(*Ctr::fpInstance).name(), Ctr::fpInstance);
    Ctr::fpInstance->RunCE();
  }
  else CE_Warning("ControlEngine not created at %p\n", Ctr::fpInstance);
  return;
}

/******************************************************************************/

CtrRcu2Tpc::CtrRcu2Tpc(){
  if(fpFecaccess){
    const char* fecMonitor = NULL;
    char number[]="  ";
    uint32_t fecNr=0;
    fecMonitor = getenv("FEESERVER_FEC_MONITOR");
    //Turn on all cards by default
    if(!fecMonitor) fecMonitor="11111111111110001111111111110000";
    for(int i=15;i>=0;i--){
      if(fecMonitor[i] && fecMonitor[i]=='1'){
        snprintf(number,3,"%.2u",fecNr);
        fpFecs[i]=new DevFecTpc(number, subDevices.size(), this, fpFecaccess, i);
        AddSubDevice(fpFecs[i]);
        CE_Event("DevFec%.2u '%s' (%i) of type '%s' created at %p with DevFecaccess at %p\n", i, fpFecs[i]->GetName().c_str(), fpFecs[i]->GetDeviceId(), typeid(*fpFecs[i]).name(), fpFecs[i], fpFecaccess);
        fecNr++;
      }
      else{
        fpFecs[i]=NULL;
        CE_Event("DevFec%.2u not created at %p with DevFecaccess at %p\n", i, fpFecs[i], fpFecaccess);
      }
    }
    for(int i=16;i<32;i++){
      if(fecMonitor[i] && fecMonitor[i]=='1'){
        snprintf(number,3,"%.2u",fecNr);
        fpFecs[i]=new DevFecTpc(number, subDevices.size(), this, fpFecaccess, i);
        AddSubDevice(fpFecs[i]);
        CE_Event("DevFec%.2u '%s' (%i) of type '%s' created at %p with DevFecaccess at %p\n", i, fpFecs[i]->GetName().c_str(), fpFecs[i]->GetDeviceId(), typeid(*fpFecs[i]).name(), fpFecs[i], fpFecaccess);
        fecNr++;
      }
      else{
        fpFecs[i]=NULL;
        CE_Event("DevFec%.2u not created at %p with DevFecaccess at %p\n", i, fpFecs[i], fpFecaccess);
      }
    }
  }
}

