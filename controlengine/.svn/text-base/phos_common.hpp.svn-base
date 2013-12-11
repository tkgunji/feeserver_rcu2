#ifndef __PHOS_COMMON_HPP
#define __PHOS_COMMON_HPP

#include "fec.hpp"
#include "ctr.hpp"

/******************************************************************************/

class DevFecaccess;

class DevFecPhs: public DevFec{
public:
  DevFecPhs(std::string name, int id, CEStateMachine* pParent, DevFecaccess* fecaccess, uint32_t fec);
  virtual int ArmorLocal();
};

class DevFecTru: public DevFec{
public:
  DevFecTru(std::string name, int id,  CEStateMachine* pParent, DevFecaccess* fecaccess, uint32_t fec);
  virtual int ArmorLocal();
};

/******************************************************************************/

class DevFecaccess;

class DevPhs: public Dev{
public:
  DevPhs(int id,  CEStateMachine* pParent, DevFecaccess* fecaccess);
  virtual ~DevPhs();
  virtual int issue(uint32_t cmd, uint32_t parameter, const char* pData, int iDataSize, std::vector<uint32_t>& rb);
  virtual int PreUpdateLocal();
protected:
  virtual int ReSynchronizeLocal();
  virtual int GetGroupId();
  DevFecaccess* fpFecaccess;
  uint32_t* fpApdData[32];
};
#endif //__PHOS_COMMON_HPP

