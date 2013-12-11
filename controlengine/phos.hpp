#ifndef __PHOS_HPP
#define __PHOS_HPP

#include "rcu.hpp"
#include "phos_common.hpp"

class CtrRcuPhs: public CtrRcu{
public:
  CtrRcuPhs();
protected:
  /** The PHOS detector */
  DevPhs* fpPhs;
};
#endif //__PHOS_HPP

