#include "dcb.hpp"
#include "dcscMsgBufferInterface.h"
#include "selectmapInterface.h"
#include "ctr.hpp"

DevMsgbufferDcb::DevMsgbufferDcb(){
  DriverLoad();
}
DevMsgbufferDcb::~DevMsgbufferDcb(){
  DriverUnload();
}
int DevMsgbufferDcb::SingleWrite(uint32_t address, uint32_t data, uint32_t mode){
  switch(mode){
  case 3:
    if(!rcuBusControlCmd(eCheckFlash))rcuBusControlCmd(eEnableFlash);
    return rcuFlashWrite(address, &data, 1, 2);
    //case 2:
    //if(!rcuBusControlCmd(eCheckSelectmap))rcuBusControlCmd(eEnableSelectmap);
    //return smRegisterWrite(address, data);
  case 1:
    if(!rcuBusControlCmd(eCheckMsgBuf))rcuBusControlCmd(eEnableMsgBuf);
    return rcuSingleWrite(address, data);
  }
  return -mode;
}
int DevMsgbufferDcb::SingleRead(uint32_t address, uint32_t* pData, uint32_t mode){
  CE_Debug("DevMsgbufferDcb::SingleRead, address = 0x%x\n",address); 
  switch(mode){
  case 3:
    if(!rcuBusControlCmd(eCheckFlash))rcuBusControlCmd(eEnableFlash);
    return rcuFlashRead(address, 1, pData);
    //case 2:
    //if(!rcuBusControlCmd(eCheckSelectmap))rcuBusControlCmd(eEnableSelectmap);
    //return smRegisterRead(address, pData);
  case 1:
    //if(!rcuBusControlCmd(eCheckMsgBuf))rcuBusControlCmd(eEnableMsgBuf);
    //return rcuSingleRead(address, pData);
    return Rcu2SingleRead(address, 1, pData);
  }
  return -mode;
}
int DevMsgbufferDcb::MultipleWrite(uint32_t address, uint32_t* pData, int iSize, int iDataSize, uint32_t mode){
  switch(mode){
  case 3:
    if(!rcuBusControlCmd(eCheckFlash))rcuBusControlCmd(eEnableFlash);
    return rcuFlashWrite(address, pData, iSize, 2);
    //case 2:{
    //if(!rcuBusControlCmd(eCheckSelectmap))rcuBusControlCmd(eEnableSelectmap);
    //uint32_t i=0;
    //for(i=0;i<iSize;i++) smRegisterWrite(address+i, pData[i]);
    //return i;
    //}
  case 1:{
    if(!rcuBusControlCmd(eCheckMsgBuf))rcuBusControlCmd(eEnableMsgBuf);
    return rcuMultipleWrite(address, pData, iSize, 4);
  }
  }
  return -mode;
}
int DevMsgbufferDcb::MultipleRead(uint32_t address, int iSize, uint32_t* pData, uint32_t mode){
  switch(mode){
  case 3:
    if(!rcuBusControlCmd(eCheckFlash))rcuBusControlCmd(eEnableFlash);
    return rcuFlashRead(address, iSize, pData);
    //case 2:{
    //if(!rcuBusControlCmd(eCheckSelectmap))rcuBusControlCmd(eEnableSelectmap);
    //uint32_t i=0;
    //pData=new uint32_t[iSize];
    //for(i=0;i<iSize;i++) smRegisterRead(address+i, &pData[i]);
    //return i;
    //}
  case 1:{
    if(!rcuBusControlCmd(eCheckMsgBuf))rcuBusControlCmd(eEnableMsgBuf);
    return rcuMultipleRead(address, iSize, pData);
  }
  }
  return -mode;
}
int DevMsgbufferDcb::FlashErase(int startSec, int stopSec){ 
  if(!rcuBusControlCmd(eCheckFlash))rcuBusControlCmd(eEnableFlash);
  return rcuFlashErase(startSec, stopSec); 
}
int DevMsgbufferDcb::DriverUnload(){
  if(releaseRcuAccess()<0){
    CE_Error("initRcuAccess finished with error code\n");
  }
//  if(releaseSmAccess()<0){
//    CE_Error("initRcuAccess finished with error code\n");
//  }
  return 1;
}
int DevMsgbufferDcb::DriverLoad(){
  if(initRcuAccess(NULL)<0){
    CE_Error("initRcuAccess finished with error code\n");
   }
//  if(initSmAccess(NULL)<0){
//    CE_Error("initSmAccess finished with error code\n");
//  }
  return 1;
} 
int DevMsgbufferDcb::DriverReload(){
  DriverUnload();
  DriverLoad();
  return 1;
}
