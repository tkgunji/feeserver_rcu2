#ifndef __SER_HPP
#define __SER_HPP
#include <stdint.h>
#include <vector>
#include "dim_services.hh"
#include "messagebuffer.hpp"
class DevMsgbuffer;
class DevFecaccess;

class Ser{
public:
  struct TceServiceData {
    union {
      int iVal;
      float fVal;
      void* strVal;
    };
  };
  typedef int (*ceUpdateService)(TceServiceData* pData, int major, int minor, void* parameter);
  enum ceServiceDataType{
    eDataTypeUnknown = 0,
    eDataTypeInt,
    eDataTypeFloat,
    eDataTypeString,
  };
  struct ceUpdateData{
    TceServiceData data;
    DimService* service;
    ceServiceDataType type;
    ceUpdateService pFctUpdate;
    int option;
    int major;
    int minor;
    void* parameter;
  };
  Ser(std::string name);
  virtual int Get(TceServiceData* pData)=0;
  static int GetValue(TceServiceData* pData, int32_t type, int32_t unused, void* parameter);
  static int ceUpdateServices(const char* pattern);
  static int RegisterService(enum ceServiceDataType type, const char* name, float defDeadband=0, ceUpdateService pFctUpdate=NULL, int option=0, int major=0, int minor=0, void* parameter=NULL);
  static std::vector<ceUpdateData*> fDimServices;
protected:
  std::string fName;
};
class SerMbAddrS : public Ser{
public:
  SerMbAddrS(std::string name, uint32_t address, int32_t convfactor, int32_t deadband, DevMsgbuffer* msgbuffer, uint32_t mode=1);
  virtual int Get(TceServiceData* pData);
protected:
  uint32_t fAddress;
  int32_t fConvfactor;
  DevMsgbuffer *fpMsgbuffer;
  uint32_t fMode;
};
class SerMbAddrF : public Ser{
public:
  SerMbAddrF(std::string name, uint32_t address, float convfactor, float deadband, DevMsgbuffer* msgbuffer, uint32_t mode=1);
  virtual int Get(TceServiceData* pData);
protected:
  uint32_t fAddress;
  float fConvfactor;
  DevMsgbuffer *fpMsgbuffer;
  uint32_t fMode;
};
class SerFecRegS : public Ser{
public:
  SerFecRegS(std::string name, uint32_t fec, uint32_t reg, int32_t convfactor, int32_t deadband, DevFecaccess* fecaccess);
  virtual int Get(TceServiceData* pData);
protected:
  uint32_t fFec;
  uint32_t fReg;
  int32_t fConvfactor;
  DevFecaccess *fpFecaccess;
};
class SerFecRegF : public Ser{
public:
  SerFecRegF(std::string name, uint32_t fec, uint32_t reg, float convfactor, float deadband, DevFecaccess* fecaccess);
  virtual int Get(TceServiceData* pData);
protected:
  uint32_t fFec;
  uint32_t fReg;
  float fConvfactor;
  DevFecaccess *fpFecaccess;
};

#endif //__SER_HPP

