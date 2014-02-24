#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <unistd.h>
#include <fstream>
#include <sys/wait.h>
#include "issuehandler.hpp"
#include "ctr.hpp"

std::vector<CEIssueHandler*> CEIssueHandler::issueHandlers;
std::vector<uint32_t> CEIssueHandler::rb;
int CEIssueHandler::hardware=0;

CEIssueHandler::CEIssueHandler(){
  issueHandlers.push_back(this);
}

CEIssueHandler::~CEIssueHandler(){
}

int CEIssueHandler::HighLevelHandler(const char* pCommand, std::vector<uint32_t>& rb){
  return -1;
}

int CEIssueHandler::GetGroupId(){
  return -1;
}

int ce_issue(char* command, char** result, int* size){
  if(!size) return 0;
  if(Ctr::terminating||!command||!result||!Ctr::fpInstance){
    *size=0;
    return 0;
  }
  CE_Event("Starting executing command block of size %i\n",*size);
  CE_LockGuard g(Ctr::fMutex);
  CEIssueHandler::rb.clear();
  int processed=CEIssueHandler::Issue(command,*size,CEIssueHandler::rb);
  int inSize=*size;
  *size=Ctr::rb.size()*4;
  *result=(char*)&(CEIssueHandler::rb[0]);
  CE_Event("Finished executing command block of size %i, of which %i were processed with return block size %i\n", inSize, processed, *size);
  return processed;
}

int CEIssueHandler::Issue(const char* command, int size, std::vector<uint32_t>& rb){
  if(size==0) return 0;
  if(size<4){
    CE_Debug("Command block of size %i is too small\n",size);
    return 0;
  }
  if(command[0]=='<' && command[1]=='f'){
    std::string commandString=std::string(command,size);
    CE_Debug("Executing command block of size %i for high level fee command\n",size);
    int processed=IssueFee(commandString, rb);
    if(processed==0) return 0;
    return processed+Issue(command+processed, size-processed, rb);
  }
  if(command[0]=='<' && command[1]=='a'){
    std::string commandString=std::string(command,size);
    CE_Debug("Executing command block of size %i for high level action command\n",size);
    int processed=IssueAction(commandString, rb);
    if(processed==0) return 0;
    return processed+Issue(command+processed, size-processed, rb);
  }
  if(command[0]=='<' && command[1]=='c'){
    std::string commandString=std::string(command,size);
    CE_Debug("Executing command block of size %i for high level config command\n",size);
    int processed=IssueConfig(commandString, rb);
    if(processed==0) return 0;
    return processed+Issue(command+processed, size-processed, rb);
  }
  if(((*(uint32_t*)command)&0xf0000000)==0xf0000000){
    CE_Debug("Executing command block of size %i for binary command, header %p\n",size,*(uint32_t*)command);
    int processed=IssueBinary(command, size, rb);
    if(processed==0) return 0;
    return processed+Issue(command+processed, size-processed, rb);
  }
  if(size%4){
    CE_Debug("Re-aligning command block of size %i by %i\n",size, size%4);
    return size%4+Issue(command+size%4, size-size%4, rb);
  }
  CE_Warning("Command block of size %i does not contain any command, header %p\n",size,*(uint32_t*)command);
  return 0;
}

int CEIssueHandler::IssueFee(std::string& command, std::vector<uint32_t>& rb){
  CE_Event("Executing high level fee command '%s' of size %i\n",command.c_str(),command.size());
  if(command.find("<fee>",0)!=0) return 0;
  size_t pos=command.find("</fee>",5);
  if(pos==std::string::npos){
    CE_Error("Unterminated high level fee command '%s'\n", command.c_str());
    return command.size();
  }
  pos+=6;
  std::string commandNew=command.substr(5, -5+pos-6);
  std::vector<CEIssueHandler*>::iterator issueHandler=issueHandlers.begin();
  int ikey = 0;
  while(issueHandler!=issueHandlers.end()){
    if((*issueHandler)->HighLevelHandler(commandNew.c_str(),rb)>0){
      ikey = 1;
      return pos;
    }
    issueHandler++;
  }
  if(ikey==0){
    CE_Warning("Unknown high level fee command '%s'\n", commandNew.c_str());
  }
  return pos;
}

int CEIssueHandler::IssueAction(std::string& command, std::vector<uint32_t>& rb){
  CE_Event("Executing high level action command '%s' of size %i\n",command.c_str(),command.size());
  if(command.find("<action>",0)!=0) return 0;
  size_t pos=command.find("</action>",8);
  if(pos==std::string::npos){
    CE_Error("Unterminated high level action command '%s'\n", command.c_str());
    return command.size();
  }
  pos+=9;
  std::string commandNew=command.substr(8,-8+pos-9);
  int strSize=(-8+pos-9+3)/4;
  int size=1+strSize+1;
  std::vector<uint32_t> cmd(size);
  cmd[0]=CE_TRIGGER_TRANSITION | strSize*4;
  memcpy((char*)&cmd[1], commandNew.c_str(), commandNew.size());
  cmd[1+strSize+0]=CE_CMD_ENDMARKER;
  if(int iResult=Issue((char*)&(cmd[0]), size*4, rb)<0){
    CE_Debug("High level action command '%s' issued binary command %p with parameter %p, failed with error %i\n", commandNew.c_str(), CE_TRIGGER_TRANSITION, strSize*4, iResult);
  }
  return pos;
}

int CEIssueHandler::IssueConfig(std::string& command, std::vector<uint32_t>& rb){
  CE_Event("Executing high level action command '%s' of size %i\n",command.c_str(),command.size());
  int n=0;
  size_t pos=0;
  do{
    size_t startPos=command.find("<config>",pos);
    size_t endPos=command.find("</config>",pos);
    if(startPos<endPos){
      if(startPos!=std::string::npos){
        n++;
        pos=startPos+8;
      }
      else{
        pos=startPos;
      }
    }
    else{
      if(endPos!=std::string::npos){
        n--;
        pos=endPos+9;
      }
      else{
        pos=endPos;
      }
    }
  }
  while(n>0 && pos!=std::string::npos);
  if(n!=0){
    CE_Error("Inconsistent high level config command '%s'\n", command.c_str());
    return command.size();
  } 
  if(pos==std::string::npos){
    CE_Error("Unterminated high level config command '%s'\n", command.c_str());
    return command.size();
  }
  int strSize=(-8+pos-9+3)/4;
  int size=4+strSize+6;
  std::vector<uint32_t> cmd(size);
  cmd[0]=FEE_CONFIGURE_32;
  cmd[1]=-4+size-1;
  cmd[2]=hardware++;
  cmd[3]=0;
  std::string commandNew=command.substr(8, -8+pos-9);
  memcpy((char*)&cmd[4], commandNew.c_str(), commandNew.size());
  cmd[4+strSize+0]=FEE_VERIFICATION;
  cmd[4+strSize+1]=CE_CMD_ENDMARKER;
  cmd[4+strSize+2]=FEE_CONFIGURE_END;
  cmd[4+strSize+3]=cmd[2];
  cmd[4+strSize+4]=CE_CMD_ENDMARKER;
  cmd[4+strSize+5]=CE_CMD_ENDMARKER;
  if(int iResult=Issue((char*)&(cmd[0]), size*4, rb)<0){
    CE_Debug("High level config command '%s' issued binary command %p with parameter %p, failed with error %i\n", command.c_str(), CE_TRIGGER_TRANSITION, 0, iResult);
  }
  return pos;
}

int CEIssueHandler::IssueBinary(const char* command, int size, std::vector<uint32_t>& rb){
  uint32_t u32header=*((uint32_t*)command);
  uint32_t cmd=u32header&(FEESERVER_CMD_MASK|FEESERVER_CMD_ID_MASK|FEESERVER_CMD_SUB_MASK);
  uint32_t cmdId=u32header&(FEESERVER_CMD_MASK|FEESERVER_CMD_ID_MASK);
  uint32_t parameter=u32header&FEESERVER_CMD_PARAM_MASK;
  if((u32header&FEESERVER_CMD_MASK)!=FEESERVER_CMD){
    CE_Debug("Unknown command header %p\n", u32header);
    return 0;
  }
  std::vector<CEIssueHandler*>::iterator issueHandler=issueHandlers.begin();
  while(issueHandler!=issueHandlers.end()){
    if((*issueHandler)->GetGroupId()==(int)cmdId){
      int processed=(*issueHandler)->issue(cmd, parameter, command+4, -4+size-4, rb);
      if(processed<0){
        CE_Error("Binary command %p with parameter %p failed with error %i\n", u32header, parameter, processed);
        return size;
      }
      uint32_t u32tailer=*((uint32_t*)(command+4+processed));
      if((u32tailer&CE_CMD_EM_MASK) != CE_CMD_ENDMARKER){
        CE_Error("End marker %p for binary command %p with parameter %i not found, instead %p = %p & %p\n", CE_CMD_ENDMARKER, cmd, parameter, u32tailer&CE_CMD_EM_MASK, u32tailer, CE_CMD_EM_MASK);
        return size;
      }
      return 4+processed+4;
    }
    issueHandler++;
  }
  CE_Error("Unknown binary command group %p\n", cmdId);
  return 0;
}

/******************************************************************************************/

ShellCommandHandler::ShellCommandHandler(){
}

ShellCommandHandler::~ShellCommandHandler(){
}

int ShellCommandHandler::GetGroupId(){
  return FEESVR_CMD_SHELL;
}

int ShellCommandHandler::issue(uint32_t cmd, uint32_t parameter, const char* pData, int iDataSize, std::vector<uint32_t>& rb){
  CE_Debug("translateShellCommand cmd=%#x parameter=%#x\n", cmd, parameter);
  std::string command;
  std::string inFileName(Ctr::tmpDir + "/FEESERVER_EXEC_IN");
  std::string outFileName(Ctr::tmpDir + "/FEESERVER_EXEC_OUT");
  switch (cmd) {
  case FEESRV_RCU2SH_SCRIPT:
  case FEESRV_EXECUTE_SCRIPT:
  case FEESRV_BINARY_PGM:
    {
      std::ofstream outFile(outFileName.c_str(), std::ofstream::binary);
      if(outFile.fail()){
        CE_Warning("Opening file '%s' failed\n", outFileName.c_str());
        break;
      }
      outFile.write(pData, parameter);
     outFile.close();
     break;
    }
  case FEESRV_EXECUTE_PGM:
    break;
  }
  switch (cmd) {
  case FEESRV_RCU2SH_SCRIPT:
    command = "rcu-sh b " + outFileName;
    break;
  case FEESRV_EXECUTE_SCRIPT:
  case FEESRV_BINARY_PGM:
    system(std::string("chmod a+x " + outFileName).c_str());
    command = outFileName;
    break;
  case FEESRV_EXECUTE_PGM:
    command=pData;
    break;
  }
  command+=" 1> " + inFileName + " 2>&1";
  system(command.c_str());
  std::ifstream inFile(inFileName.c_str(), std::ios::in|std::ios::binary);
  if(inFile.fail()) CE_Warning("Opening file %s failed\n", inFileName.c_str());
  inFile.seekg(0, std::ios::end);
  uint32_t fileSize = inFile.tellg();
  inFile.seekg (0, std::ios::beg);
  uint32_t rbSize=rb.size()*4;
  rb.resize((rbSize+fileSize+3)/4);
  inFile.read((char*)&rb[rbSize], fileSize);
  inFile.close();
  system(std::string("rm -f " + outFileName + " " + inFileName).c_str());
  return parameter;
}

