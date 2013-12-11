#include "dev.hpp"
#include "ctr.hpp"
Dev::Dev(std::string name, int id,  CEStateMachine* pParent): CEStateMachine(name, id, pParent){
}
Dev::~Dev(){
  fServices.clear();
}
//int Dev::PreUpdate(){
//  return 0;
//}
int Dev::issue(uint32_t cmd, uint32_t parameter, const char* pData, int iDataSize, std::vector<uint32_t>& rb){
  CE_Warning("Command handling not implemented for command %#x\n", cmd);
  return -1;
}

