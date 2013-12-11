#ifndef __TPC_COMMON_HPP
#define __TPC_COMMON_HPP

#include "fec.hpp"

class DevFecaccess;

class DevFecTpc: public DevFec{
public:
  DevFecTpc(std::string name, int id,  CEStateMachine* pParent, DevFecaccess* fecaccess, uint32_t fec);
  virtual int ArmorLocal();
};
#endif //__TPC_COMMON_HPP

