#ifndef __SERVICE_HPP
#define __SERVICE_HPP
#include <vector>
#include "dis.hxx"
class DevMsgbuffer;
class DevFecaccess;
class Source{
protected:
  Source();
};
class SourceFec : public Source{
protected:
  SourceFec(unsigned int fec, unsigned int reg, DevFecaccess* fecaccess);
  unsigned int fec;
  unsigned int reg;
  DevFecaccess* fecaccess;
  virtual int Update()=0;
};
class SourceReg : public Source{
protected:
  SourceReg(unsigned int reg, DevMsgbuffer* msgbuffer, unsigned int mode=0);
  unsigned int reg;
  DevMsgbuffer* msgbuffer;
  unsigned int mode;
  virtual int Update()=0;
};
class SourceVar : public Source{
protected:
  SourceVar();
};
class SourceCall : public Source{
protected:
  SourceCall(void* callObject);
  void* callObject;
};
class Service : public virtual DimService{
protected:
  Service();
  static int UpdateServices(std::string pattern);
  virtual int Update()=0;
  static std::vector<Service*> services;
};
class ServiceFloat : public Service{
protected:
  ServiceFloat(std::string name, float deadband, float factor, float offset);
  int Set(float value);
  float value;
  float deadband;
  float factor;
  float offset;
};
class ServiceInt : public Service{
protected:
  ServiceInt(std::string name, int deadband, int factor, int offset);
  int Set(int value);
  int value;
  int deadband;
  int factor;
  int offset;
};
class ServiceString : public Service{
protected:
  ServiceString(std::string name);
  int Set(std::string value);
  static const int BUF_SIZE=256;
  char value[BUF_SIZE];
};
class ServiceFloatFec : public SourceFec, public ServiceFloat{
public:
  static int Create(std::string name, unsigned int fec, unsigned int reg, DevFecaccess* fecaccess, float deadband=0.0, float factor=1.0, float offset=0.0);
protected:
  ServiceFloatFec(std::string name, unsigned int fec, unsigned int reg, DevFecaccess* fecaccess, float deadband=0.0, float factor=1.0, float offset=0.0);
  int Update();
};
class ServiceIntFec : public SourceFec, public ServiceInt{
public:
  static int Create(std::string name, unsigned int fec, unsigned int reg, DevFecaccess* fecaccess, int deadband=0, int factor=1, int offset=0);
protected:
  ServiceIntFec(std::string name, unsigned int fec, unsigned int reg, DevFecaccess* fecaccess, int deadband=0, int factor=1, int offset=0);
  int Update();
};
class ServiceFloatReg : public SourceReg, public ServiceFloat{
public:
  static int Create(std::string name, unsigned int reg, DevMsgbuffer* msgbuffer, float deadband=0.0, float factor=1.0, float offset=0.0, unsigned int mode=0);
protected:
  ServiceFloatReg(std::string name, unsigned int reg, DevMsgbuffer* msgbuffer, float deadband=0.0, float factor=1.0, float offset=0.0, unsigned int mode=0);
  int Update();
};
class ServiceIntReg : public SourceReg, public ServiceInt{
public:
  static int Create(std::string name, unsigned int reg, DevMsgbuffer* msgbuffer, int deadband=0, int factor=1, int offset=0, unsigned int mode=0);
protected:
  ServiceIntReg(std::string name, unsigned int reg, DevMsgbuffer* msgbuffer, int deadband=0, int factor=1, int offset=0, unsigned int mode=0);
  int Update();
};
class ServiceFloatVar : public SourceVar, public ServiceFloat{
public:
  static int Create(std::string name, float* var, float deadband=0.0, float factor=1.0, float offset=0.0);
protected:
  ServiceFloatVar(std::string name, float* var, float deadband=0.0, float factor=1.0, float offset=0.0);
  int Update();
  float* var;
};
class ServiceIntVar : public SourceVar, public ServiceInt{
public:
  static int Create(std::string name, int* var, int deadband=0, int factor=1, int offset=0);
protected:
  ServiceIntVar(std::string name, int* var, int deadband=0, int factor=1, int offset=0);
  int Update();
  int* var;
};
class ServiceStringVar : public SourceVar, public ServiceString{
public:
  static int Create(std::string name, std::string* var);
protected:
  ServiceStringVar(std::string name, std::string* var);
  int Update();
  std::string* var;
};
class ServiceFloatCall : public SourceCall, public ServiceFloat{
public:
  typedef float(*callFunctionFloat)(void* callObject);
  static int Create(std::string name, callFunctionFloat callFunction,  void* callObject, float deadband=0.0, float factor=1.0, float offset=0.0);
protected:
  ServiceFloatCall(std::string name, callFunctionFloat callFunction, void* callObject, float deadband=0.0, float factor=1.0, float offset=0.0);
  int Update();
  callFunctionFloat callFunction;
};
class ServiceIntCall : public SourceCall, public ServiceInt{
public:
  typedef int(*callFunctionInt)(void* callObject);
  static int Create(std::string name, callFunctionInt callFunction,  void* callObject, int deadband=0, int factor=1, int offset=0);
protected:
  ServiceIntCall(std::string name, callFunctionInt callFunction,  void* callObject, int deadband=0, int factor=1, int offset=0);
  int Update();
  callFunctionInt callFunction;
};
class ServiceStringCall : public SourceCall, public ServiceString{
public:
  typedef std::string&(*callFunctionString)(void* callObject);
  static int Create(std::string name, callFunctionString callFunction,  void* callObject);
protected:
  ServiceStringCall(std::string name, callFunctionString callFunction,  void* callObject);
  int Update();
  callFunctionString callFunction;
};
#endif //__SERVICE_HPP

