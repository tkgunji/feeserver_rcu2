#ifndef __DCB_HPP
#define __DCB_HPP
#include <vector>
#include "messagebuffer.hpp"

class DevMsgbufferDcb : public DevMsgbuffer {
public:
  DevMsgbufferDcb();
  virtual ~DevMsgbufferDcb();
  virtual int SingleWrite(uint32_t address, uint32_t data, uint32_t mode=1);
  virtual int SingleRead(uint32_t address, uint32_t* pData, uint32_t mode=1);
  virtual int SingleI2CWrite(uint32_t base, uint32_t address, uint32_t pData, uint32_t mode=1);
  virtual int SingleI2CRead(uint32_t base, uint32_t address, uint32_t* pData, uint32_t mode=1);
  virtual int SingleI2CReadReg(uint32_t address, uint32_t* pData, uint32_t mode=1);

  virtual int SingleRMWrite(uint8_t base, uint8_t address, uint16_t pData, uint8_t mode=1);
  virtual int SingleRMRead(uint8_t base, uint8_t address, uint16_t* pData, uint8_t mode=1);

  virtual int MultipleWrite(uint32_t address, uint32_t* pData, int iSize, int iDataSize=4, uint32_t mode=1);
  virtual int MultipleRead(uint32_t address, int iSize,uint32_t* pData, uint32_t mode=1);
  virtual int FlashErase(int startSec, int stopSec);
  virtual int DriverReload();
  virtual int DriverUnload();
  virtual int DriverLoad();
};
#endif //__DCB_HPP

