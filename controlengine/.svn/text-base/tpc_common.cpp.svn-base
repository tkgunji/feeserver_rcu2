#include "tpc_common.hpp"

DevFecTpc::DevFecTpc(std::string name, int id,  CEStateMachine* pParent, DevFecaccess* fecaccess, uint32_t fec): DevFec(name, id, pParent, fecaccess, fec){
}
int DevFecTpc::ArmorLocal(){
  std::string name;
  name=GetName();
  name+="_";
/*
  fServices.push_back(new SerFecRegF(name+"T_TH"          , fFec, 0x01, 0.25   , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"AV_TH"         , fFec, 0x02, 0.0043 , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"AC_TH"         , fFec, 0x03, 0.017  , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"DV_TH"         , fFec, 0x04, 0.0043 , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"DC_TH"         , fFec, 0x05, 0.03   , 0   , fpFecaccess));
*/
  fServices.push_back(new SerFecRegF(name+"TEMP"          , fFec, 0x06, 0.25   , 0.5 , fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"AV"            , fFec, 0x07, 0.0043 , 0.05, fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"AC"            , fFec, 0x08, 0.017  , 0.1 , fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"DV"            , fFec, 0x09, 0.0043 , 0.05, fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"DC"            , fFec, 0x0a, 0.03   , 0.5 , fpFecaccess));
/*
  fServices.push_back(new SerFecRegS(name+"FECCSR0"       , fFec, FECCSR0, 1   , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"FECCSR1"       , fFec, FECCSR1, 1   , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"FECCSR2"       , fFec, FECCSR2, 1   , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"FECCSR3"       , fFec, FECCSR3, 1   , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"L1CNT"         , fFec, 0x0b, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"L2CNT"         , fFec, 0x0c, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"SCLKCNT"       , fFec, 0x0d, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"DSTBCNT"       , fFec, 0x0e, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"TSMWORD"       , fFec, 0x0f, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"USRATIO"       , fFec, 0x10, 1      , 0   , fpFecaccess));
  fServices.push_back(current);
*/
  return 0;
}

