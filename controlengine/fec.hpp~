#ifndef __FEC_HPP
#define __FEC_HPP

#include "dev.hpp"
#include <map>
#include <stdint.h>

/******************************************************************************/

class DevFecaccess;

class DevFec: public Dev{
public:
  DevFec(std::string name, int id,  CEStateMachine* pParent, DevFecaccess* fecaccess, uint32_t fec);
  virtual int ReSynchronizeLocal();
  virtual int PreUpdateLocal();
  static int CtrRcuGO_OFF(CEStateMachine* callbackObject);
  static int CtrRcuGO_ON(CEStateMachine* callbackObject);
protected:
  virtual int EnterStateON();
  DevFecaccess * fpFecaccess;
  uint32_t fFec;
};

/******************************************************************************/

class DevMsgbuffer;
/*
Error codes returned:
 0     everything OK, no remarks
-210X: from DevFecaccess
-211X: from DevFecaccessDummy
-212X: from DevFecaccessI2c
-213X: from DevFecaccessI2c
-214X: from DevFecaccessGtl
-215X: from DevFecaccessGtl
-216X: from DevFecaccessRcu2
-217X: from DevFecaccessRcu2
*/

class DevFecaccess{
public:
  DevFecaccess(DevMsgbuffer* msgbuffer);
  virtual int ReadFecReg(uint32_t &data, uint32_t fec, uint32_t reg);
  virtual int WriteFecReg(uint32_t data, uint32_t fec, uint32_t reg);
  virtual int AccessFecReg(uint32_t &data, uint32_t fec, uint32_t reg, uint32_t rnw)=0;
  virtual int DetectFecs(uint32_t &detectedFecs);
  virtual int GetFecAflStatus(uint32_t &status, uint32_t fec);
  virtual int SetFecAflStatus(uint32_t status, uint32_t fec);
  virtual int GetAfl(uint32_t &value);
  virtual int SetAfl(uint32_t value);
protected:
  DevMsgbuffer* fpMsgbuffer;
};
//Dummy implementation
class DevFecaccessDummy : public DevFecaccess{
public:
  DevFecaccessDummy(DevMsgbuffer* msgbuffer);
  virtual ~DevFecaccessDummy();
  virtual int AccessFecReg(uint32_t &data, uint32_t fec, uint32_t reg,  uint32_t rnw);
protected:
  std::map<uint32_t,uint32_t> fMemory;
};
#endif //__FEC_HPP

