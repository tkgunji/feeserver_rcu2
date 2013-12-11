#include <cmath>
#include "service.hpp"
#include "messagebuffer.hpp"
#include "fec.hpp"
std::vector<Service*> Service::services;
Source::Source(){
}
SourceFec::SourceFec(unsigned int fec, unsigned int reg, DevFecaccess* fecaccess):
  Source(),
  fec(fec),
  reg(reg),
  fecaccess(fecaccess){
}
SourceReg::SourceReg(unsigned int reg, DevMsgbuffer* msgbuffer, unsigned int mode):
  Source(),
  reg(reg),
  msgbuffer(msgbuffer),
  mode(mode){
}
SourceVar::SourceVar():
  Source(){
}
SourceCall::SourceCall(void* callObject):
  Source(),
  callObject(callObject){
}
Service::Service(){
}
int Service::UpdateServices(std::string pattern){
  std::vector<Service*>::iterator it;
  for(it=Service::services.begin();it<Service::services.end();it++) if(((std::string)(*it)->getName()).find(pattern)) (*it)->Update();
  return true;
}
ServiceFloat::ServiceFloat(std::string name, float deadband, float factor, float offset):
  DimService(name.c_str(), ServiceFloat::value),
  Service(),
  value(-2000.0),
  deadband(deadband),
  factor(factor),
  offset(offset){
}
int ServiceFloat::Set(float value){
  if(fabs(ServiceFloat::value-(value+ServiceFloat::offset)*ServiceFloat::factor)>=fabs(ServiceFloat::deadband)){
    ServiceFloat::value=(value+ServiceFloat::offset)*ServiceFloat::factor;
    updateService();
  }
  return true;
}
ServiceInt::ServiceInt(std::string name, int deadband, int factor, int offset):
  DimService(name.c_str(), ServiceInt::value),
  Service(),
  value(-2000),
  deadband(deadband),
  factor(factor),
  offset(offset){
}
int ServiceInt::Set(int value){
  if(abs(ServiceInt::value-(value+ServiceInt::offset)*ServiceInt::factor)>=abs(ServiceInt::deadband)){
    ServiceInt::value=(value+ServiceInt::offset)*ServiceInt::factor;
    updateService();
  }
  return true;
}
ServiceString::ServiceString(std::string name):
  DimService(name.c_str(), ServiceString::value),
  Service(){
  strncpy(ServiceString::value, "-2000", BUF_SIZE);
}
int ServiceString::Set(std::string value){
  if(value.compare(ServiceString::value) != 0){
    strncpy(ServiceString::value, value.c_str(), BUF_SIZE);
    ServiceString::value[BUF_SIZE-1]='\0';
    updateService();
  }
  return true;
}
ServiceFloatFec::ServiceFloatFec(std::string name, unsigned int fec, unsigned int reg, DevFecaccess* fecaccess, float deadband, float factor, float offset):
  SourceFec(fec, reg, fecaccess),
  ServiceFloat(name, deadband, factor, offset){
}
int ServiceFloatFec::Create(std::string name, unsigned int fec, unsigned int reg, DevFecaccess* fecaccess, float deadband, float factor, float offset){
  Service* service=new ServiceFloatFec(name, reg, fec, fecaccess, deadband, factor, offset);
  services.push_back(service);
  return true;
}
int ServiceFloatFec::Update(){
  unsigned int rawValue=0;
  ServiceFloatFec::fecaccess->ReadFecReg(rawValue, ServiceFloatFec::fec, ServiceFloatFec::reg);  
  Set(rawValue);
  return true;
}
ServiceIntFec::ServiceIntFec(std::string name, unsigned int fec, unsigned int reg, DevFecaccess* fecaccess, int deadband, int factor, int offset):
  SourceFec(fec, reg, fecaccess),
  ServiceInt(name, deadband, factor, offset){
}
int ServiceIntFec::Create(std::string name, unsigned int fec, unsigned int reg, DevFecaccess* fecaccess, int deadband, int factor, int offset){
  Service* service=new ServiceIntFec(name, reg, fec, fecaccess, deadband, factor, offset);
  services.push_back(service);
  return true;
}
int ServiceIntFec::Update(){
  unsigned int rawValue=0;
  ServiceIntFec::fecaccess->ReadFecReg(rawValue, ServiceIntFec::fec, ServiceIntFec::reg);  
  Set(rawValue);
  return true;
}
ServiceFloatReg::ServiceFloatReg(std::string name, unsigned int reg, DevMsgbuffer* msgbuffer, float deadband, float factor, float offset, unsigned int mode):
  SourceReg(reg, msgbuffer, mode),
  ServiceFloat(name, deadband, factor, offset){
}
int ServiceFloatReg::Create(std::string name, unsigned int reg, DevMsgbuffer* msgbuffer, float deadband, float factor, float offset, unsigned int mode){
  Service* service=new ServiceFloatReg(name, reg, msgbuffer, deadband, factor, offset, mode);
  services.push_back(service);
  return true;
}
int ServiceFloatReg::Update(){
  unsigned int rawValue=0;
  ServiceFloatReg::msgbuffer->SingleRead(ServiceFloatReg::reg, &rawValue, ServiceFloatReg::mode);
  Set(rawValue);
  return true;
}
ServiceIntReg::ServiceIntReg(std::string name, unsigned int reg, DevMsgbuffer* msgbuffer, int deadband, int factor, int offset, unsigned int mode):
  SourceReg(reg, msgbuffer, mode),
  ServiceInt(name, deadband, factor, offset){
}
int ServiceIntReg::Create(std::string name, unsigned int reg, DevMsgbuffer* msgbuffer, int deadband, int factor, int offset, unsigned int mode){
  Service* service=new ServiceIntReg(name, reg, msgbuffer, deadband, factor, offset, mode);
  services.push_back(service);
  return true;
}
int ServiceIntReg::Update(){
  unsigned int rawValue=0;
  ServiceIntReg::msgbuffer->SingleRead(ServiceIntReg::reg, &rawValue, ServiceIntReg::mode);
  Set(rawValue);
  return true;
}
ServiceFloatVar::ServiceFloatVar(std::string name, float* var, float deadband, float factor, float offset):
  SourceVar(),
  ServiceFloat(name, deadband, factor, offset),
  var(var){
}
int ServiceFloatVar::Create(std::string name, float* var, float deadband, float factor, float offset){
  Service* service=new ServiceFloatVar(name, var, deadband, factor, offset);
  services.push_back(service);
  return true;
}
int ServiceFloatVar::Update(){
  Set(*var);
  return true;
}
ServiceIntVar::ServiceIntVar(std::string name, int* var, int deadband, int factor, int offset):
  SourceVar(),
  ServiceInt(name, deadband, factor, offset),
  var(var){
}
int ServiceIntVar::Create(std::string name, int* var, int deadband, int factor, int offset){
  Service* service=new ServiceIntVar(name, var, deadband, factor, offset);
  services.push_back(service);
  return true;
}
int ServiceIntVar::Update(){
  Set(*var);
  return true;
}
ServiceStringVar::ServiceStringVar(std::string name, std::string* var):
  SourceVar(),
  ServiceString(name),
  var(var){
}
int ServiceStringVar::Create(std::string name, std::string* var){
  Service* value=new ServiceStringVar(name, var);
  services.push_back(value);
  return true;
}
int ServiceStringVar::Update(){
  Set(*var);
  return true;
}
ServiceFloatCall::ServiceFloatCall(std::string name, callFunctionFloat callFunction,  void* callObject, float deadband, float factor, float offset):
  SourceCall(callObject),
  ServiceFloat(name, deadband, factor, offset),
  callFunction(callFunction){
}
int ServiceFloatCall::Create(std::string name, callFunctionFloat callFunction,  void* callObject, float deadband, float factor, float offset){
  Service* value=new ServiceFloatCall(name, callFunction, callObject, deadband, factor, offset);
  services.push_back(value);
  return true;
}
int ServiceFloatCall::Update(){
  Set(callFunction(callObject));
  return true;
}
ServiceIntCall::ServiceIntCall(std::string name, callFunctionInt callFunction, void* callObject, int deadband, int factor, int offset):
  SourceCall(callObject),
  ServiceInt(name, deadband, factor, offset),
  callFunction(callFunction){
}
int ServiceIntCall::Create(std::string name, callFunctionInt callFunction, void* callObject, int deadband, int factor, int offset){
  Service* value=new ServiceIntCall(name, callFunction, callObject, deadband, factor, offset);
  services.push_back(value);
  return true;
}
int ServiceIntCall::Update(){
  Set(callFunction(callObject));
  return true;
}
ServiceStringCall::ServiceStringCall(std::string name, callFunctionString callFunction, void* callObject):
  SourceCall(callObject),
  ServiceString(name),
  callFunction(callFunction){
}
int ServiceStringCall::Create(std::string name, callFunctionString callFunction, void* callObject){
  Service* value=new ServiceStringCall(name, callFunction, callObject);
  services.push_back(value);
  return true;
}
int ServiceStringCall::Update(){
  Set(callFunction(callObject));
  return true;
}

