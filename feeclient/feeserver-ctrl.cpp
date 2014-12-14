#include <iostream>
#include <cstdlib>
#include <cstring>
#include "FeeSampleClient.hpp"

int main(int argc, char** arg){
  if(argc==1){
    std::cerr << "Error:       no FeeServers specified, at least one required" << std::endl;
    std::cerr << "Usage:       feeserver-ctrl [FeeServer name 1] ... [FeeServer name n]" << std::endl;
    std::cerr << "Description: commands to be sendt to FeeServers are read from stdin; results are written to stdout" << std::endl;
    std::cerr << "             no processing is performed on the input/output data" << std::endl;
    std::cerr << "             supports any command type" << std::endl;
    std::cerr << "Example:     feeserver-ctrl test_feeserver < command.bin" << std::endl;
    std::cerr << "             echo \"<action>GO_IDLE</action>\" | feeserver-ctrl test_feeserver another_feeserver" << std::endl;
    exit(-1);
  }
  if(!getenv("DIM_DNS_NODE")){
    std::cerr << "Error: DIM_DNS_NODE not defined" << std::endl;
    exit(-2);
  }
  dcs::fee::FeeSampleClient* feeClient = new dcs::fee::FeeSampleClient();
  if(!feeClient){
    std::cerr << "Error: could not create FeeClient" << std::endl;
    exit(-3);
  }
  for(int i=1; i<argc; i++){
    if(!feeClient->registerFeeServerName(arg[i])){
      std::cerr << "Error: could not register FeeServer " << arg[i] << std::endl;
    }
  }
 if(!feeClient->startFeeClient()){
    std::cerr << "Error: could not start FeeClient" << std::endl;
    exit(-4);
  }
  size_t readSize=2;
  size_t size=0;
  dcs::fee::FeeSampleClient::DataArray buffer;
  do{
    readSize*=2;
    buffer.resize(buffer.size()+readSize/4);
    std::cin.read((char*)&buffer[buffer.size()-readSize/4], readSize);
    size += std::cin.gcount();
  }
  while(readSize==(unsigned int)std::cin.gcount());
  std::string server;
  size_t sizeCopy;
  dcs::fee::FeeSampleClient::DataArray bufferCopy;
  dcs::fee::FeeSampleClient::FlagBits flags;
  short errorCode;
  short status;
  for(int i=1; i<argc; i++){
    server=arg[i];
    bufferCopy=buffer;
    sizeCopy=size;
    flags=0;
    errorCode=0;
    status=0;
    if(!feeClient->writeReadData(server, sizeCopy, bufferCopy, flags, errorCode, status)){
      std::cerr << "Error: could not communicate with FeeServer " << arg[i] << std::endl;
    }
    else{
      std::cout<<"results of the executation : size = "<<sizeCopy<<std::endl;
      if(bufferCopy[0]!=NULL){
	std::cout<<std::hex<<"0x"<<bufferCopy[0]<<std::endl;
      }
      //std::cout.write((char*)&bufferCopy[0],sizeCopy);
    }
  }
  feeClient->stopFeeClient();
  delete feeClient;
  exit(0);
}

