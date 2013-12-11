#include "phos_common.hpp"

DevFecPhs::DevFecPhs(std::string name, int id,  CEStateMachine* pParent, DevFecaccess* fecaccess, uint32_t fec): DevFec(name, id, pParent, fecaccess, fec){
}
int DevFecPhs::ArmorLocal(){
  std::string name;
  name=GetName();
  name+="_";
//General registers
/*
  fServices.push_back(new SerFecRegS(name+"UNLOCK"        , fFec, 0x00, 1      , 0   , fpFecaccess));
*/
//  fServices.push_back(new SerFecRegS(name+"L0CNT"         , fFec, 0x0B, 1      , 0   , fpFecaccess));
//  fServices.push_back(new SerFecRegS(name+"L2CNT"         , fFec, 0x0C, 1      , 0   , fpFecaccess));
/*
  fServices.push_back(new SerFecRegS(name+"SCLKCNT"       , fFec, 0x0D, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"CSR0"          , fFec, 0x11, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"CSR1"          , fFec, 0x12, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"CSR2"          , fFec, 0x13, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"CSR3"          , fFec, 0x14, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"BCVERSION"     , fFec, 0x20, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"VTS_HIGH"      , fFec, 0x21, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"VTS_LOW"       , fFec, 0x22, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"TH_HMGERR_HIGH", fFec, 0x23, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"TH_HMGERR_LOW" , fFec, 0x24, 1      , 0   , fpFecaccess));
*/
//  fServices.push_back(new SerFecRegS(name+"HV_FB1"        , fFec, 0x25, 1      , 0   , fpFecaccess));
//  fServices.push_back(new SerFecRegS(name+"HV_FB2"        , fFec, 0x26, 1      , 0   , fpFecaccess));
//  fServices.push_back(new SerFecRegS(name+"HV_HVMGERR1"   , fFec, 0x27, 1      , 0   , fpFecaccess));
//  fServices.push_back(new SerFecRegS(name+"HV_HVMGERR2"   , fFec, 0x28, 1      , 0   , fpFecaccess));
/*
//ADC Min Treshold Memory
  fServices.push_back(new SerFecRegF(name+"TEMP1_MIN_TH"  , fFec, 0x30, 0.25   , 0.0 , fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"D4V0_MIN_TH"   , fFec, 0x31, 0.00804, 0.0 , fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"D4V0C_MIN_TH"  , fFec, 0x32, 0.0298 , 0.0 , fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"D3V3_MIN_TH"   , fFec, 0x33, 0.00644, 0.0 , fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"D3V3C_MIN_TH"  , fFec, 0x34, 0.0429 , 0.0 , fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"TEMP2_MIN_TH"  , fFec, 0x35, 0.25   , 0.0 , fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"A6NV0_MIN_TH"  , fFec, 0x36, 0.00488, 0.0 , fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"A6NV0C_MIN_TH" , fFec, 0x37, 0.0293 , 0.0 , fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"A6PV0_MIN_TH"  , fFec, 0x38, 0.0114 , 0.0 , fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"A6PV0C_MIN_TH" , fFec, 0x39, 0.03473, 0.0 , fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"TEMP3_MIN_TH"  , fFec, 0x3A, 0.25   , 0.0 , fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"A3V3_MIN_TH"   , fFec, 0x3B, 0.00644, 0.0 , fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"A3V3C_MIN_TH"  , fFec, 0x3C, 0.0429 , 0.0 , fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"A13V0_MIN_TH"  , fFec, 0x3D, 0.0268 , 0.0 , fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"A13V0C_MIN_TH" , fFec, 0x3E, 0.0223 , 0.0 , fpFecaccess));
//ADC Max Treshold Memory
  fServices.push_back(new SerFecRegF(name+"TEMP1_MAX_TH"  , fFec, 0x40, 0.25   , 0.0 , fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"D4V0_MAX_TH"   , fFec, 0x41, 0.00804, 0.0 , fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"D4V0C_MAX_TH"  , fFec, 0x42, 0.0298 , 0.0 , fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"D3V3_MAX_TH"   , fFec, 0x43, 0.00644, 0.0 , fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"D3V3C_MAX_TH"  , fFec, 0x44, 0.0429 , 0.0 , fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"TEMP2_MAX_TH"  , fFec, 0x45, 0.25   , 0.0 , fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"A6NV0_MAX_TH"  , fFec, 0x46, 0.00488, 0.0 , fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"A6NV0C_MAX_TH" , fFec, 0x47, 0.0293 , 0.0 , fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"A6PV0_MAX_TH"  , fFec, 0x48, 0.0114 , 0.0 , fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"A6PV0C_MAX_TH" , fFec, 0x49, 0.03473, 0.0 , fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"TEMP3_MAX_TH"  , fFec, 0x4A, 0.25   , 0.0 , fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"A3V3_MAX_TH"   , fFec, 0x4B, 0.00644, 0.0 , fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"A3V3C_MAX_TH"  , fFec, 0x4C, 0.0429 , 0.0 , fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"A13V0_MAX_TH"  , fFec, 0x4D, 0.0268 , 0.0 , fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"A13V0C_MAX_TH" , fFec, 0x4E, 0.0223 , 0.0 , fpFecaccess));
*/
//ADC Data memory
  fServices.push_back(new SerFecRegF(name+"TEMP1"         , fFec, 0x50, 0.25   , 0.5 , fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"D4V0"          , fFec, 0x51, 0.00804, 0.05, fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"D4V0C"         , fFec, 0x52, 0.0298 , 0.5 , fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"D3V3"          , fFec, 0x53, 0.00644, 0.05, fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"D3V3C"         , fFec, 0x54, 0.0429 , 0.5 , fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"TEMP2"         , fFec, 0x55, 0.25   , 0.5 , fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"A6NV0"         , fFec, 0x56, 0.00488, 0.05, fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"A6NV0C"        , fFec, 0x57, 0.0293 , 0.1 , fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"A6PV0"         , fFec, 0x58, 0.0114 , 0.05, fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"A6PV0C"        , fFec, 0x59, 0.03473, 0.1, fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"TEMP3"         , fFec, 0x5A, 0.25   , 0.5 , fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"A3V3"          , fFec, 0x5B, 0.00644, 0.05, fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"A3V3C"         , fFec, 0x5C, 0.0429 , 0.1 , fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"A13V0"         , fFec, 0x5D, 0.0268 , 0.05, fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"A13V0C"        , fFec, 0x5E, 0.0223 , 0.1 , fpFecaccess));
/*
//HV DAC settings memory
  fServices.push_back(new SerFecRegS(name+"CSP23"         , fFec, 0x60, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"CSP22"         , fFec, 0x61, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"CSP21"         , fFec, 0x62, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"CSP20"         , fFec, 0x63, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"CSP19"         , fFec, 0x64, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"CSP18"         , fFec, 0x65, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"CSP17"         , fFec, 0x66, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"CSP16"         , fFec, 0x67, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"CSP00"         , fFec, 0x68, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"CSP01"         , fFec, 0x69, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"CSP02"         , fFec, 0x6A, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"CSP03"         , fFec, 0x6B, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"CSP04"         , fFec, 0x6C, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"CSP05"         , fFec, 0x6D, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"CSP06"         , fFec, 0x6E, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"CSP07"         , fFec, 0x6F, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"CSP08"         , fFec, 0x70, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"CSP09"         , fFec, 0x71, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"CSP10"         , fFec, 0x72, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"CSP11"         , fFec, 0x73, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"CSP12"         , fFec, 0x74, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"CSP13"         , fFec, 0x75, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"CSP14"         , fFec, 0x76, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"CSP15"         , fFec, 0x77, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"CSP31"         , fFec, 0x78, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"CSP30"         , fFec, 0x79, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"CSP29"         , fFec, 0x7A, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"CSP28"         , fFec, 0x7B, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"CSP27"         , fFec, 0x7C, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"CSP26"         , fFec, 0x7D, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"CSP25"         , fFec, 0x7E, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"CSP24"         , fFec, 0x7F, 1      , 0   , fpFecaccess));
*/
  return 0;
}

DevFecTru::DevFecTru(std::string name, int id,  CEStateMachine* pParent, DevFecaccess* fecaccess, uint32_t fec): DevFec(name, id, pParent, fecaccess, fec){
}
int DevFecTru::ArmorLocal(){
  std::string name;
  name=GetName();
  name+="_";
/*fServices.push_back(new SerFecRegF(name+"T1_TH"         , fFec, 0x01, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"A4V0_TH"       , fFec, 0x02, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"A4V0C_TH"      , fFec, 0x03, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"D4V2_TH"       , fFec, 0x04, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"D4V2C_TH"      , fFec, 0x05, 1      , 0   , fpFecaccess));*/
  fServices.push_back(new SerFecRegF(name+"TEMP1"         , fFec, 0x06, 0.25   , 0.5 , fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"A4V0"          , fFec, 0x07, 1      , 0.05, fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"A4V0C"         , fFec, 0x08, 1      , 0.1 , fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"D4V2"          , fFec, 0x09, 1      , 0.05, fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"D4V2C"         , fFec, 0x0a, 1      , 0.5 , fpFecaccess));
/*fServices.push_back(new SerFecRegS(name+"CSR0"          , fFec, 0x11, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"CSR1"          , fFec, 0x12, 1      , 0   , fpFecaccess));
//fServices.push_back(new SerFecRegS(name+"CHRDO"         , fFec, 0x1a, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"BC_VER"        , fFec, 0x20, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"T2_TH"         , fFec, 0x21, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"D3V3_TH"       , fFec, 0x22, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"D3V3C_TH"      , fFec, 0x23, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"D2V5_TH"       , fFec, 0x24, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"D1V5_TH"       , fFec, 0x25, 1      , 0   , fpFecaccess));*/
  fServices.push_back(new SerFecRegF(name+"TEMP2"         , fFec, 0x28, 0.25   , 0.5 , fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"D3V3"          , fFec, 0x29, 1      , 0.05, fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"D3V3C"         , fFec, 0x2a, 1      , 0.5 , fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"D2V5"          , fFec, 0x2b, 1      , 0.05, fpFecaccess));
  fServices.push_back(new SerFecRegF(name+"D1V5"          , fFec, 0x2c, 1      , 0.05, fpFecaccess));
/*fServices.push_back(new SerFecRegS(name+"ADC_ID"        , fFec, 0x2e, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"TEST_REG"      , fFec, 0x2f, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"VTS"           , fFec, 0x65, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"INT_MASK"      , fFec, 0x66, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"CARD_NO"       , fFec, 0x70, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"SER_NO"        , fFec, 0x71, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"TORSYNC"       , fFec, 0x78, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"ADCOUTSYNC"    , fFec, 0x79, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"ADCINSYNC"     , fFec, 0x7a, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"ADCCONTROL"    , fFec, 0x7c, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"MASK_REG1"     , fFec, 0x7d, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"MASK_REG2"     , fFec, 0x7e, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"MASK_REG3"     , fFec, 0x7f, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"MASK_REG4"     , fFec, 0x80, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"MASK_REG5"     , fFec, 0x81, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"MASK_REG6"     , fFec, 0x82, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"MASK_REG7"     , fFec, 0x83, 1      , 0   , fpFecaccess));
  fServices.push_back(new SerFecRegS(name+"CSSELECT"      , fFec, 0x92, 1      , 0   , fpFecaccess));
  char number[]="  ";
  std::string thrName;
  for(uint32_t j=0;j<91;j++){
    thrName=name;
    thrName+="4X4THR";
    snprintf(number,3,"%.2u",j);
    thrName+=number;
    fServices.push_back(new SerFecRegS(thrName, fFec, 0xa5+j, 1, 0, fpFecaccess));
  }
*/
 return 0;
}

/******************************************************************************/

DevPhs::DevPhs(int id, CEStateMachine* pParent, DevFecaccess* fecaccess):Dev("PHS", id, pParent),fpFecaccess(fecaccess){
  for(uint32_t i=0;i<32;i++){
    fpApdData[i]=NULL;
  }
}
int DevPhs::ReSynchronizeLocal(){
  if (fpFecaccess==NULL) return kStateError;
  return kStateStandby;
}
DevPhs::~DevPhs(){
  for(uint32_t i=0;i<32;i++){
    if(fpApdData[i]){
      delete fpApdData[i];
      fpApdData[i]=NULL;
    }
  }
}
int DevPhs::issue(uint32_t cmd, uint32_t parameter, const char* pData, int iDataSize, std::vector<uint32_t>& rb){
  int size=0;
  CE_Debug("CErcu::TranslateRcuCommand cmd=%#x parameter=%#x datasize=%d\n", cmd, parameter, iDataSize);
  switch(cmd){
  case PHOS_APD_INIT:
  {
    size=32*sizeof(uint32_t);
    if(iDataSize<size){
      CE_Error("data size missmatch, %d byte available in buffer but %d requested by command (%#x)\n", iDataSize, size, cmd);
      return -EINVAL;
    }
    if(parameter>32){
      CE_Error("invalid FEC number %d addressed, but only 0-31 supported\n", parameter);
      return -EINVAL;
    }
    if(fpApdData[parameter]==NULL) fpApdData[parameter]=new uint32_t[32];
    memcpy(fpApdData[parameter], pData, size);
    for(uint32_t i=0;i<8;i++)
      if(fpFecaccess->WriteFecReg(fpApdData[parameter][i+ 0], parameter, 0x67-i)<0)
        CE_Warning("write for FEC id %d called, but failed\n", parameter);
    for(uint32_t i=0;i<16;i++)
      if(fpFecaccess->WriteFecReg(fpApdData[parameter][i+ 8], parameter, 0x68+i)<0)
        CE_Warning("write for FEC id %d called, but failed\n", parameter);
    for(uint32_t i=0;i<8;i++)
      if(fpFecaccess->WriteFecReg(fpApdData[parameter][i+24], parameter, 0x7F-i)<0)
        CE_Warning("write for FEC id %d called, but failed\n", parameter);
    break;
  }
  default:
    CE_Warning("unrecognized command id (%#x)\n", cmd);
    return -ENOSYS;
  }
  return size;
}
int DevPhs::PreUpdateLocal(){
  uint32_t rawData=0;
  uint32_t hamming=0;
  for(uint32_t i=0;i<32;i++){
    if(fpApdData[i]==NULL) continue;
    fpFecaccess->ReadFecReg(rawData, i, 0x25);
    for(uint32_t j=0;j< 8;j++) hamming=hamming | ((rawData & (0x1<<j))<<(23-2*j));
    for(uint32_t j=8;j<16;j++) hamming=hamming | ((rawData & (0x1<<j))>>8);
    fpFecaccess->ReadFecReg(rawData, i, 0x26);
    for(uint32_t j=0;j< 8;j++) hamming=hamming | ((rawData & (0x1<<j))<<8);
    for(uint32_t j=8;j<16;j++) hamming=hamming | ((rawData & (0x1<<j))<<(31-2*j));
    for(uint32_t j=0;j<32;j++){
      if(((hamming&(0x1<<j))>>j)==0x1){
        if(j>= 0 && j< 8){
          if(fpFecaccess->WriteFecReg(fpApdData[i][j], i, 0x67-j)<0)
            CE_Warning("write for FEC id %d called, but failed\n", i);
        }
        else if(j>= 8 && j<24){
          if(fpFecaccess->WriteFecReg(fpApdData[i][j], i, 0x68+j)<0)
            CE_Warning("write for FEC id %d called, but failed\n", i);
        }
        else{ //Has to be (j>=24 && j<32)
          if(fpFecaccess->WriteFecReg(fpApdData[i][j], i, 0x7F-j)<0)
            CE_Warning("write for FEC id %d called, but failed\n", i);
        }
      }
    }
  }
  return 0;
}
int DevPhs::GetGroupId(){
  return FEESVR_CMD_PHOS;
}

