#include "messagebuffer.hpp"
#include "dcscMsgBufferInterface.h"
#include "selectmapInterface.h"

DevMsgbuffer::DevMsgbuffer(){
}
DevMsgbuffer::~DevMsgbuffer(){
}

DevMsgbufferDummy::DevMsgbufferDummy(): DevMsgbuffer(){
}
DevMsgbufferDummy::~DevMsgbufferDummy(){
  fMemory.clear();
}
int DevMsgbufferDummy::SingleWrite(uint32_t address, uint32_t data, uint32_t mode){
  if (address<fMemory.size()) fMemory[address]=data;
  return 1;
}
int DevMsgbufferDummy::SingleRead(uint32_t address, uint32_t* pData, uint32_t mode){
  if (pData && address<fMemory.size()) *pData=fMemory[address];
  return 1;
}
int DevMsgbufferDummy::SingleI2CWrite(uint32_t base, uint32_t address, uint32_t pData, uint32_t mode){
  return 1;
}
int DevMsgbufferDummy::SingleI2CRead(uint32_t base, uint32_t address, uint32_t* pData, uint32_t mode){
  return 1;
}
int DevMsgbufferDummy::SingleI2CReadReg(uint32_t address, uint32_t* pData, uint32_t mode){
  return 1;
}
int DevMsgbufferDummy::SingleRMWrite(uint8_t base, uint8_t address, uint16_t pData, uint8_t mode){
  return 1;
}
int DevMsgbufferDummy::SingleRMRead(uint8_t base, uint8_t address, uint16_t* pData, uint8_t mode){
  return 1;
}

int DevMsgbufferDummy::MultipleWrite(uint32_t address, uint32_t* pData, int iSize, int iDataSize, uint32_t mode){
  int i=0;
  for(i=0;i<iSize;i++) if(address<fMemory.size()) fMemory[address+i]=pData[i];
  return i;
}
int DevMsgbufferDummy::MultipleRead(uint32_t address, int iSize,uint32_t* pData, uint32_t mode){
  int i=0;
  pData=new uint32_t[iSize];
  for(i=0;i<iSize;i++) if(pData && address<fMemory.size()) pData[i]=fMemory[address+i];
  return i;
}
int DevMsgbufferDummy::FlashErase(int startSec, int stopSec){ 
  return stopSec-startSec; 
}
int DevMsgbufferDummy::DriverReload(){
  return 1;
}
int DevMsgbufferDummy::DriverUnload(){
  return 1;
}
int DevMsgbufferDummy::DriverLoad(){
  return 1;
}

