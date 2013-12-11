#ifndef __RCU2_HPP
#define __RCU2_HPP

#include <vector>
#include "ctr.hpp"
#include "dev.hpp"
#include "fec.hpp"

/******************************************************************************/

#define RCU2_FECActiveList       0x8000
#define RCU2_FECActiveList_WIDTH 32
#define RCU2_FECActiveList_SIZE  1

#define RCU2StatusReg 0x0000

/******************************************************************************/

class DevRcu2: public Dev{
public:
  DevRcu2(int id,  CEStateMachine* pParent, DevMsgbuffer* msgbuffer);
  virtual int GetGroupId();
  virtual int HighLevelHandler(const char* pCommand, std::vector<uint32_t>& rb);
  virtual int issue(uint32_t cmd, uint32_t parameter, const char* pData, int iDataSize, std::vector<uint32_t>& rb);
  virtual int SetAfl(uint32_t value);
  static int ReadStatusReg(Ser::TceServiceData* pData, int32_t adc, int32_t unused, void* parameter);
protected:
  virtual int ArmorLocal();
  virtual int ReSynchronizeLocal();
  DevMsgbuffer* fpMsgbuffer;
};

/******************************************************************************/

class CtrRcu2: public Ctr{
public:
  CtrRcu2();
  virtual ~CtrRcu2();
  static int CtrRcu2GO_IDLE(CEStateMachine* callbackObject);
  //RCU2 sub device
  DevRcu2* fpRcu2;
protected:
  /** The FEC access */
  DevFecaccess* fpFecaccess;
  /** The FECs */
  DevFec* fpFecs[32];
};

/******************************************************************************/

//Class for access via rcu2
class DevFecaccessRcu2: public DevFecaccess{
public:
  DevFecaccessRcu2(DevMsgbuffer* msgbuffer);
  virtual int AccessFecReg(uint32_t &data, uint32_t fec, uint32_t reg, uint32_t rnw);
};

/******************************************************************************/

class DevMsgbufferRcu2: public DevMsgbuffer {
public:
  DevMsgbufferRcu2();
  virtual ~DevMsgbufferRcu2();
  virtual int SingleWrite(uint32_t address, uint32_t data, uint32_t mode=1);
  virtual int SingleRead(uint32_t address, uint32_t* pData, uint32_t mode=1);
  virtual int MultipleWrite(uint32_t address, uint32_t* pData, int iSize, int iDataSize=4, uint32_t mode=1);
  virtual int MultipleRead(uint32_t address, int iSize,uint32_t* pData, uint32_t mode=1);
  virtual int FlashErase(int startSec, int stopSec);
  virtual int DriverReload();
  virtual int DriverUnload();
  virtual int DriverLoad();
};
#endif //__RCU2_HPP

