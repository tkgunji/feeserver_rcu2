#include <cmath>
#include "ser.hpp"
#include "ctr.hpp"
#include "fec.hpp"

std::vector<Ser::ceUpdateData*> Ser::fDimServices;
Ser::Ser(std::string name){
  fName=name;
}
int Ser::GetValue(TceServiceData* pData, int32_t type, int32_t unused, void* parameter){
  if(!type){
    CE_Warning("Invalid type %d passed to Ser:GetValue\n", type);
    return -1;
  }
  if(!parameter){
    CE_Warning("Invalid pData passed to Ser::GetValue for type %d\n", type);
    return -2;
  }
  Ser* pSer;
  switch(type){
    case 1: pSer=(SerMbAddrS*)parameter; break;
    case 2: pSer=(SerMbAddrF*)parameter; break;
    case 3: pSer=(SerFecRegS*)parameter; break;
    case 4: pSer=(SerFecRegF*)parameter; break;
    default: CE_Warning("Unmatched type %d passed to Ser::GetValue\n", type); return -3;
  }
  if(!pSer){
    CE_Warning("Extracting pSer failed in Ser::GetValue for type %d\n", type);
    return -4;
  }
  return pSer->Get(pData);
}

SerMbAddrS::SerMbAddrS(std::string name, uint32_t address, int32_t convfactor, int32_t deadband, DevMsgbuffer* msgbuffer, uint32_t mode):Ser(name){
  fAddress=address;
  fConvfactor=convfactor;
  fpMsgbuffer=msgbuffer;
  fMode=mode;
  RegisterService(eDataTypeInt, name.c_str(), deadband, Ser::GetValue, 0, 1, 0, this);
}
int SerMbAddrS::Get(TceServiceData* pData){
  int nRet=0;
  uint32_t rawValue=0;
  nRet=fpMsgbuffer->SingleRead(fAddress, &rawValue, fMode);
  pData->iVal=rawValue*fConvfactor;
  if(nRet<0) pData->iVal=nRet;
  return 0;
}

SerMbAddrF::SerMbAddrF(std::string name, uint32_t address, float convfactor, float deadband, DevMsgbuffer* msgbuffer, uint32_t mode):Ser(name){
  fAddress=address;
  fConvfactor=convfactor;
  fpMsgbuffer=msgbuffer;
  fMode=mode;
  RegisterService(eDataTypeFloat, name.c_str(), deadband, Ser::GetValue, 0, 2, 0, this);
}
int SerMbAddrF::Get(TceServiceData* pData){
  int nRet=0;
  uint32_t rawValue=0;
  nRet=fpMsgbuffer->SingleRead(fAddress, &rawValue, fMode);
  pData->fVal=rawValue*fConvfactor;
  if(nRet<0) pData->fVal=nRet;
  return 0;
}

SerFecRegS::SerFecRegS(std::string name, uint32_t fec, uint32_t reg, int32_t convfactor, int32_t deadband, DevFecaccess* fecaccess):Ser(name){
  fFec=fec;
  fReg=reg;
  fConvfactor=convfactor;
  fpFecaccess=fecaccess;
  RegisterService(eDataTypeInt, name.c_str(), deadband, Ser::GetValue, 0, 3, 0, this);
}
int SerFecRegS::Get(TceServiceData* pData){
  int nRet=0;
  uint32_t rawValue=0;
  nRet=fpFecaccess->ReadFecReg(rawValue, fFec, fReg);
  pData->iVal=rawValue*fConvfactor;
  if(nRet<0) pData->iVal=nRet;
  return 0;
}

SerFecRegF::SerFecRegF(std::string name, uint32_t fec, uint32_t reg, float convfactor, float deadband, DevFecaccess* fecaccess):Ser(name){
  fFec=fec;
  fReg=reg;
  fConvfactor=convfactor;
  fpFecaccess=fecaccess;
  RegisterService(eDataTypeFloat, name.c_str(), deadband, Ser::GetValue, 0, 4, 0, this);
}
int SerFecRegF::Get(TceServiceData* pData){
  int nRet=0;
  uint32_t rawValue=0;
  nRet=fpFecaccess->ReadFecReg(rawValue, fFec, fReg);
  pData->fVal=rawValue*fConvfactor;
  if(nRet<0) pData->fVal=nRet;
  return 0;
}

int Ser::ceUpdateServices(const char* pattern){
  std::vector<ceUpdateData*>::iterator it;
  for(it=Ser::fDimServices.begin();it<Ser::fDimServices.end();it++){
    if((*it)->pFctUpdate && (!pattern || (std::string((*it)->service->getName())).find(pattern)!=std::string::npos)){
      if(pattern){
        CE_Debug("Forcing update of service '%s', matching pattern '%s'\n", (*it)->service->getName(), pattern);
      }
      (*(*it)->pFctUpdate)(&(*it)->data, (*it)->major, (*it)->minor, (*it)->parameter);
      switch((*it)->type){
        case eDataTypeInt:    *((dim_int*)  (*it)->service)=(*it)->data.iVal; break;
        case eDataTypeFloat:  *((dim_float*)(*it)->service)=(*it)->data.fVal; break;
        case eDataTypeString: if((*it)->data.strVal) *((dim_string*)(*it)->service)=*(std::string*)(*it)->data.strVal; else *((dim_string*)(*it)->service)=(char*)"-2002"; break;
        default: CE_Warning("Unknown service type %i discarded\n", (*it)->type);  break;
      }
    }
  }
  return 0;
}
int Ser::RegisterService(enum ceServiceDataType type, const char* name, float defDeadband, ceUpdateService pFctUpdate, int option, int major, int minor, void* parameter){
  std::string fullName=Ctr::source;
  fullName+="_";
  fullName+=name;
  ceUpdateData* data=NULL;
  DimService* service=NULL;
  switch(type){
    case eDataTypeInt:    service=new dim_int(fullName);                 if(service) *(dim_int*)   service=        -2000;  break;
    case eDataTypeFloat:  service=new dim_float(fullName, defDeadband);  if(service) *(dim_float*) service=        -2000;  break;
    case eDataTypeString: service=new dim_string(fullName);              if(service) *(dim_string*)service=(char*)"-2000"; break;
    default: CE_Warning("Unknown service type %i discarded.\n", type);  break;
  }
  if(!service) return -1;
  data=new ceUpdateData();
  data->service=service;
  data->type=type;
  data->pFctUpdate=pFctUpdate;
  data->option=option;
  data->major=major;
  data->minor=minor;
  data->parameter=parameter;
  switch(type){
    case eDataTypeInt:    data->data.iVal=-2001; break;
    case eDataTypeFloat:  data->data.fVal=-2001; break;
    case eDataTypeString: data->data.strVal=(void*) new std::string("-2001"); break;
    default: CE_Warning("Unknown service type %i discarded.\n", type);  break;
  } 
  Ser::fDimServices.push_back(data);
  data=NULL;
  service=NULL;
  return 0;
}

