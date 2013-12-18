#include <cerrno>
#include <cstdlib>
#include <typeinfo>
#include <cstdarg>
#include <cstdio>
#include <sys/wait.h>
#include <unistd.h>
#include "ctr.hpp"
#include "rcu_issue.h"
#include "issuehandler.hpp"

Ctr::message_t Ctr::message;
Ctr::message_t Ctr::messageDim;
std::string Ctr::source="";
std::string Ctr::tmpDir="/tmp";
void* Ctr::logservice=NULL;
void* Ctr::enservice=NULL;
int Ctr::enableLogging=false;
int Ctr::enableServices=false;
int Ctr::loglevel=eCEEvent;
int Ctr::terminating=false;
int Ctr::updateSleep=1;
Ctr* Ctr::fpInstance=NULL;
CE_Mutex Ctr::fMutex;
std::vector<uint32_t> Ctr::configurations;

Ctr::Ctr():CEStateMachine("MAIN", -1, NULL),fpMsgbuffer(NULL),fHwType(0){
  char* envVar = getenv("FEE_SERVER_HW");
  if(envVar){
    if     (strncmp(envVar, "REAL",  strlen("REAL" ))==0) fHwType=1;
    else if(strncmp(envVar, "DUMMY", strlen("DUMMY"))==0) fHwType=2;
    //TODO: Use register probing to determine if real HW is available?
  }
  if(fHwType==0) fHwType=1;
  fpShellCommandHandler=new ShellCommandHandler();
  if(fpShellCommandHandler) CE_Event("ShellCommandHandler of type '%s' created at %p\n", typeid(*fpShellCommandHandler).name(), fpShellCommandHandler);
  else CE_Warning("ShellCommandHandler not created at %p\n", fpShellCommandHandler);
}

int Ctr::RunCE(){
  enableLogging=false;
  enableServices=false;
  source=getenv("FEE_SERVER_NAME");
  if(getenv("FEE_SERVER_TMP_DIR")) tmpDir=getenv("FEE_SERVER_TMP_DIR");
  CE_Event("TMP directory: %s\n", tmpDir.c_str());
  logservice=(void*)new DimService((source+"_Message").c_str(),"I:1;C:4;C:256;C:256;C:20",(void*)&messageDim,sizeof(message_t));
  enservice=(void*)new dim_int((source+"_CEDBG_EN_SERVICE_UPDATE").c_str());
  Ser::RegisterService(Ser::eDataTypeInt, "START_TIME", 0.5, updateServickActionStartTime, 0, 0, Ser::eDataTypeFloat, 0);
  Ser::RegisterService(Ser::eDataTypeString, "START_TIME_NAME", 0.0, updateServickActionStartTime, 0, 0, Ser::eDataTypeString, 0);
  Ser::RegisterService(Ser::eDataTypeInt, "UPDATE_TIME", 0.5, updateServiceUpdateTime, 0, 0, Ser::eDataTypeFloat, 0);
  Ser::RegisterService(Ser::eDataTypeString, "UPDATE_TIME_NAME", 0.0, updateServiceUpdateTime, 0, 0, Ser::eDataTypeString, 0);
  Ser::RegisterService(Ser::eDataTypeInt, "UPDATE_COUNTER", 0.5, updateServiceUpdateCounter, 0, 0, Ser::eDataTypeFloat, 0);
  Armor();
  Synchronize();
  enableLogging=true;
  enableServices=true;
  while(!terminating){
    sleep(updateSleep);
    if(enservice && ((*((dim_int*)enservice))!=enableServices)){
      (*((dim_int*)enservice))=enableServices;
      ((dim_int*)enservice)->updateService();
    }
    if(!enableServices) continue;
    CE_LockGuard g(fMutex);
    PreUpdate();
    Ser::ceUpdateServices(NULL);
  }
  CE_Event("RunCE terminated\n");
  enableServices=false;
  return 0;
}

Ctr::~Ctr(){
}

int Ctr::SynchronizeLocal(){
  int state=kStateElse;
  GetFileState(state);
  if(state==kStateElse){
    state=kStateStandby;
    CE_Event("State '%s' (%i) from state file for device '%s' (%i), using state '%s' (%i) instead\n", GetStateName(kStateElse), kStateElse, GetName().c_str(), GetDeviceId(), GetStateName(state), state);
  }
  else{
    CE_Event("State '%s' (%i) from state file for device '%s' (%i)\n", GetStateName(state), state, GetName().c_str(), GetDeviceId());
  }
  return state;
}

int Ctr::ReSynchronizeLocal(){
  int state=fState;
  SetFileState(state);
  return state;
}

int Ctr::EnterStateIDLE(){
  CE_Debug("EnterStateIDLE called\n");
  configurations.clear();
  return 0;
}

int Ctr::EnterStateSTANDBY(){
  CE_Debug("EnterStateSTANDBY called\n");
  configurations.clear();
  return 0;
}

uint32_t Ctr::EvaluateHwAddress(uint32_t hwAddress, uint32_t mask) const{
  uint32_t address=hwAddress;
  if (mask==0) return 0;
  while ((mask&0x1)==0) {mask>>=1; address>>=1;}
  address&=mask;
  return address;
}

int Ctr::issue(uint32_t cmd, uint32_t parameter, const char* pData, int iDataSize, std::vector<uint32_t>& rb){
  switch (cmd){
  case FEE_CONFIGURE:
  case FEE_CONFIGURE_32:
  case FEE_CONFIGURE_NOSTATE:
  case FEE_CONFIGURE_32_NOSTATE:
    {
      int headerSize=0;
      if(cmd!=FEE_CONFIGURE_32 && cmd!=FEE_CONFIGURE_32_NOSTATE) headerSize=2*4;
      else headerSize=3*4;
      if(iDataSize<headerSize){
        CE_Error("Command block size %i too small to contain configuration command\n");
        SetErrorState();
        return 0;
      }
      int dataSize=0;
      if(cmd!=FEE_CONFIGURE_32 && cmd!=FEE_CONFIGURE_32_NOSTATE) dataSize=parameter*4;
      else dataSize=*((uint32_t*)pData)*4;
      uint32_t hardware=0;
      if(cmd!=FEE_CONFIGURE_32 && cmd!=FEE_CONFIGURE_32_NOSTATE) hardware=*((uint32_t*)pData);
      else hardware=*((uint32_t*)(pData+4));
      configurations.push_back(hardware);
      CE_Debug("Configuring hardware %p with command block size %i\n", hardware,dataSize);
      if(cmd!=FEE_CONFIGURE_NOSTATE && cmd!=FEE_CONFIGURE_32_NOSTATE && GetCurrentState()!=kStateDownloading){
        CE_Event("Executing FEE_CONFIGURE command block\n");
        if(fpInstance->TriggerTransition(fpInstance->FindTransition(kActionConf))<0){
          CE_Error("Could not change state\n");
          SetErrorState();
          return headerSize+dataSize;
        }
      }
      int result=Issue(pData+headerSize, dataSize, rb);
      if(result!=dataSize){
        CE_Error("Configuration command failed, expected %i but returned %i\n",dataSize,result);
        SetErrorState();
        return headerSize+dataSize;
      }
      return headerSize+dataSize;
    }
  case FEE_CONFIGURE_END:
  case FEE_CONFIGURE_END_NOSTATE:
    {
      if(iDataSize<1*4){
        CE_Error("Command block size %i too small to contain end of configuration command\n");
        SetErrorState();
        return 0;
      }
      uint32_t hardware=*((uint32_t*)pData);
      if(hardware!=configurations.back()){
        CE_Error("Expected end of configuration of hardware %p, but got %p\n", hardware, configurations.back());
        SetErrorState();
        return 1*4;
      }
      configurations.pop_back();
      CE_Debug("Configuring hardware %p end\n", hardware);
      if(cmd!=FEE_CONFIGURE_END_NOSTATE && configurations.size()==0 && GetCurrentState()!=kStateConfDdl){
        if(fpInstance->TriggerTransition(fpInstance->FindTransition(kActionConfigureDone))<0){
          CE_Error("Could not change state\n");
          SetErrorState();
          return 1*4;
        }
      }
      return 1*4;
    }
  case FEE_VERIFICATION:
    CE_Debug("Configuration command block size %i\n", iDataSize);
    if (parameter>0){
      CE_Warning("Verification command currently not supported\n");
      return (2+parameter)*4;
    }
    else return 0;
  case FEE_EXTERNAL_CONFIGURATION:
    CE_Debug("FEE_EXTERNAL_CONFIGURATION hw address %#x\n", parameter);
    fpInstance->TriggerTransition(fpInstance->FindTransition(kActionStartDaqConf));
    return 0;
  case CEDBG_EN_SERVICE_UPDATE:
    CE_Debug("set enableServices to %d\n", parameter);
    enableServices=parameter;
    return 0;
  case CE_SET_LOGGING_LEVEL:
    CE_Debug("set loglevel to %d\n", parameter);
    loglevel=parameter;
    return 0;
  case CE_FORCE_CH_UPDATE:
    CE_Debug("Updated channels with pattern %s\n", pData);
    Ser::ceUpdateServices(pData);
    return 0;
  case CE_TERMINATE:
    CE_Debug("Terminating FeeServer\n");
    terminating=true;
    return 0;
  case CE_REBOOT:
    CE_Debug("Rebooting computer\n");
    terminating=true;
    system("reboot");
    return 0;
  case CE_UPDATE_SLEEP:
    CE_Debug("set updateSleep to %d\n", parameter);
    updateSleep=parameter;
    return 0;
  case CE_RECONFIGURE_PLD:
    CE_Debug("Reconfiguring PLD\n");
    callShellProgram("/usr/local/sbin/reconf", NULL, 0, NULL, 0, &rb, 0);
    //system("/usr/local/sbin/reconf");
    return 0;
  case CE_TEST_ERROR:
    CE_Debug("Testing error state\n");
    SetErrorState();
    return 0;
  case CE_TRIGGER_TRANSITION:
    { 
      CE_Debug("Triggering transition\n");
      std::string data=pData;
      TriggerTransition(data);
      return parameter;
    }
  }
  CE_Warning("Unrecognized command %p\n", cmd);
  return 0;
}

int Ctr::HighLevelHandler(const char* pCommand, std::vector<uint32_t>& rb){
  if(!pCommand){
    CE_Error("Hight level fee command buffer does not exist\n");
    return -1;
  }
  uint32_t cmd=0;
  size_t keySize=0;
  std::string command=pCommand;
  std::string test="";
  if(command.find(test="CEDBG_EN_SERVICE_UPDATE",0)==0){ keySize=test.size(); cmd=CEDBG_EN_SERVICE_UPDATE; }
  else if(command.find(test="CE_FORCE_CH_UPDATE"     ,0)==0){ keySize=test.size(); cmd=CE_FORCE_CH_UPDATE     ; }
  else if(command.find(test="CE_SET_LOGGING_LEVEL"   ,0)==0){ keySize=test.size(); cmd=CE_SET_LOGGING_LEVEL   ; }
  else if(command.find(test="CE_TERMINATE"           ,0)==0){ keySize=test.size(); cmd=CE_TERMINATE           ; }
  else if(command.find(test="CE_REBOOT"              ,0)==0){ keySize=test.size(); cmd=CE_REBOOT              ; }
  else if(command.find(test="CE_UPDATE_SLEEP"        ,0)==0){ keySize=test.size(); cmd=CE_UPDATE_SLEEP        ; }
  else if(command.find(test="CE_RECONFIGURE_PLD"     ,0)==0){ keySize=test.size(); cmd=CE_RECONFIGURE_PLD     ; }
  else if(command.find(test="CE_TEST_ERROR"          ,0)==0){ keySize=test.size(); cmd=CE_TEST_ERROR          ; }
  else return 0;
  std::string parameter="";
  if(keySize<command.size()){
    if(command[keySize]!=' '){
      CE_Debug("High level fee command '%s' not found in parameter group %i\n", command.c_str(), GetGroupId());
      return -2;
    }
    parameter=command.substr(keySize+1,command.size());
  }
  if(int iResult=issue(cmd, atoi(parameter.c_str()), parameter.c_str(), parameter.size(), rb)<0){
    CE_Warning("High level fee command '%s' issued binary command %p with parameter %p, failed with error %i\n", command.c_str(), cmd, atoi(parameter.c_str()), iResult);
    return -3;
  }
  return 1;
}

int Ctr::GetGroupId(){
  return FEESERVER_CE_CMD;
}

void ce_initialize(const char* feeserver_name){
  //  CE_Debug("initialize called %s\n", feeserver_name);
  CE_Event("initialize called for FeeServer %s\n", feeserver_name);
}

void ce_cleanup(){
  if(!Ctr::fpInstance) return;
  CE_Debug("Ctr::TerminateCE\n");
  CE_LockGuard g(Ctr::fMutex);
  Ctr::terminating=true;
  return;
}

int Ctr::ceLogMessage(const char* file, int lineNo, int loglevel, const char* format, ...){
  if(loglevel<Ctr::loglevel) return 0;
  memset(&message,0,sizeof(message_t));
  time_t rawtime;
  time(&rawtime);
  tm* timeinfo=localtime(&rawtime);
  snprintf(message.date,20,"%04i-%02i-%02i %02i:%02i:%02i",timeinfo->tm_year+1900,timeinfo->tm_mon+1,timeinfo->tm_mday,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
  snprintf(message.detector,4,"%s","CE");
  snprintf(message.source,256,"%s:%i",file,lineNo);
  va_list args;
  va_start(args, format);
  vsnprintf(message.description,256,format,args);
  message.eventType=loglevel;
  if(loglevel>eCEError && Ctr::enableLogging && Ctr::logservice){
    memcpy(&messageDim, &message, sizeof(message_t));
    ((DimService*)Ctr::logservice)->updateService();
  }
  printf("%s %s %i %s %s",message.detector,message.date,message.eventType,message.source,message.description);
  fflush(stdout);
  return 1;
}

int Ctr::updateServiceUpdateTime(Ser::TceServiceData* pData, int major, int minor, void* parameter){
  time_t rawtime;
  time(&rawtime);
  switch(minor){
  case Ser::eDataTypeFloat:
    pData->iVal=rawtime;
    break;
  case Ser::eDataTypeString:
    tm* timeinfo=localtime(&rawtime);
    char date[20];
    snprintf(date,20,"%04i-%02i-%02i %02i:%02i:%02i",timeinfo->tm_year+1900,timeinfo->tm_mon+1,timeinfo->tm_mday,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
    std::string* data=(std::string*)pData->strVal;
    if(data) *data=date;
    break;
  }
  return 0;
}

int Ctr::updateServickActionStartTime(Ser::TceServiceData* pData, int major, int minor, void* parameter){
  static time_t rawtime;
  if(!rawtime) time(&rawtime);
  switch(minor){
  case Ser::eDataTypeFloat:
    pData->iVal=rawtime;
    break;
  case Ser::eDataTypeString:
    tm* timeinfo=localtime(&rawtime);
    char date[20];
    snprintf(date,20,"%04i-%02i-%02i %02i:%02i:%02i",timeinfo->tm_year+1900,timeinfo->tm_mon+1,timeinfo->tm_mday,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
    std::string* data=(std::string*)pData->strVal;
    if(data) *data=date;
    break;
  }
  return 0;
}

int Ctr::updateServiceUpdateCounter(Ser::TceServiceData* pData, int major, int minor, void* parameter){
  if(pData->iVal<0)pData->iVal=0;
  else pData->iVal++;
  return 0;
}

int Ctr::readStreamIntoVector(FILE* fp, int iDefaultBufferSize, std::vector<uint32_t>* pRB){
  int iResult=0;
  if(fp && pRB){
    int iBufferSize=iDefaultBufferSize;
    if (iBufferSize==0) iBufferSize=1024;
    int offset=0;
    // this is just to keep the old code for reading of the childs
    // stdout channel. The loop was originally in spawnShellProgram
    do{
      CE_Debug("resize result buffer to %d\n", offset+iBufferSize);
      pRB->resize(offset+iBufferSize, 0);
      int iRead=fread(&(*pRB)[offset], sizeof(std::vector<uint32_t>::value_type), iBufferSize, fp);
      if (iRead<0) {
        CE_Error("internal error, can not read from stream\n");
        iResult=-EBADF;
      }
      else{
	offset+=iRead;
	if(iRead<iBufferSize){
	  CE_Debug("%d bytes read from childs stdout channel, resize result buffer to %d\n", iRead*sizeof(std::vector<uint32_t>::value_type), offset);
          pRB->resize(offset);      
	}
      }
    }
    while (iResult>=0 && !feof(fp));
    if (iResult>=0) iResult=pRB->size()*sizeof(std::vector<uint32_t>::value_type);
  }
  else{
    CE_Error("invalid pointer\n");
    iResult=-EINVAL;
  }
  return iResult;
}

int Ctr::spawnShellProgram(char const **argv, int bNonBlocking, std::vector<uint32_t>* pRB, int iVerbosity){
  int iResult=0;
  if (argv) {
    pid_t child_pid=0;
    int result_pipe[2]; // childs stdout channel -> parents result buffer
    int error_pipe[2];  // childs stderr channel -> parents CE logging
    int abnorm_pipe[2]; // childs exit value in case of execution error 
    iResult=pipe(result_pipe);
    if (iResult<0) {
      CE_Error("can not create pipe: error %d\n", iResult);
      return iResult;
    }
    iResult=pipe(error_pipe);
    if (iResult<0)  {
      CE_Error("can not create pipe: error %d\n", iResult);
      return iResult;
    }
    iResult=pipe(abnorm_pipe);
    if (iResult<0)  {
      CE_Error("can not create pipe: error %d\n", iResult);
      return iResult;
    }
    //child_pid=fork();
    child_pid=vfork();
    if (child_pid==0) {
      // child process
      close(result_pipe[0]); // close read end of the pipe
      close(error_pipe[0]); // close read end of the pipe
      close(abnorm_pipe[0]); // close read end of the pipe
      dup2(result_pipe[1], STDOUT_FILENO);
      dup2(error_pipe[1], STDERR_FILENO);
      // write a value to the pipe in order to avoid blocking
      // when parent reads from the pipe
      FILE* abnormStream=fdopen(abnorm_pipe[1], "w");
      fwrite(&iResult, sizeof(int), 1, abnormStream);
      fclose(abnormStream);
      execvp(argv[0], (char* const*)argv);
      // here we should never get if the exec was succesfull
      fprintf(stderr, "error executing child process %s\n", argv[0]);
      iResult=-EIO;
      abnormStream=fdopen(abnorm_pipe[1], "w");
      fwrite(&iResult, sizeof(int), 1, abnormStream);
      fclose(abnormStream);
      exit(iResult);
    } else {
      // parent process
      close(result_pipe[1]); // close write end of the pipe
      close(error_pipe[1]); // close write end of the pipe
      close(abnorm_pipe[1]); // close write end of the pipe
      FILE* resStream=fdopen(result_pipe[0], "r");
      FILE* errStream=fdopen(error_pipe[0], "r");
      // check the childs system status, but the child returns properly
      // even if the execution failed. So the direct pipe for the 
      // announcement of the failure was created
      int child_status=0;
      wait(&child_status);
      // check the direct pipe for the return value
      FILE* abnormStream=fdopen(abnorm_pipe[0], "r");
      if (abnormStream) {
	fread(&iResult, sizeof(int), 1, abnormStream);
      } else {
	CE_Error("severe internal system error, can not get file descriptor\n");
      }
      fclose(abnormStream);
      if(iResult>=0 && WIFEXITED(child_status)) {
	char retCode=WEXITSTATUS(child_status);
	if (iVerbosity>0) CE_Debug("program %s finished with exit code %d\n", argv[0], retCode);
	if (pRB) {
	  // read the stdout of the child
	  iResult=readStreamIntoVector(resStream, 1024, pRB);
	}
      } else {
	CE_Error("program %s exited abnormally\n", argv[0]);
      }
	if (iVerbosity>0) {
	  // read the stderr of the child
	  std::vector<uint32_t> errChannel;
	  if (readStreamIntoVector(errStream, 1024, &errChannel)>0) {
	    flushStreamBuffer(&errChannel, NULL, 1, NULL);
	  }
	}
	if (resStream) fclose(resStream);
	if (errStream) fclose(errStream);
    }
  } else {
    CE_Error("invalid pointer\n");
    iResult=-EINVAL;
  }
  //CE_Debug("result=%d\n", iResult);
  return iResult;
}

int Ctr::callShellProgram(const char* pHeadingArg, const char* pCmdString, int iStringSize, const char** arrayTrailingArg, int iNofTrailingArgs, std::vector<uint32_t>* pRB, int iVerbosity) {
  int iResult=0;
  int argc=0;
  int iLen=0;
  char* pBuffer=NULL;
  if (pCmdString) while (iLen<iStringSize && pCmdString[iLen]!=0 && pCmdString[iLen]!='\n') iLen++;
  if (iLen>0) {
    int i=0;
    int bQuote=0;
    int bDblQuote=0;
    int bSlash=0;
    int iLastBlank=0;
    pBuffer=(char*)malloc(iLen+1);
    if (pBuffer) {
      // make a working copy of the string 
      memset(pBuffer, 0, iLen+1);
      strncpy(pBuffer, pCmdString, iLen);
      // scan the string and replace blanks to isolate arguments
      for (i=0; i<=iLen; i++) {
	switch (pBuffer[i]) {
	case ' ':
	  if (!bQuote && !bDblQuote) {
	    pBuffer[i]=0;
	    if (i-iLastBlank>1) argc++;
	    iLastBlank=i;
	  }
	  break;
	case '\'':
	  if (!bSlash) {
	    if (bQuote) {
	      bQuote=0;
	      if (i-iLastBlank>1) argc++;
	    } else {
	      bQuote=1;
	    }
	    pBuffer[i]=0;
	    iLastBlank=i;
	  }
	  break;
	case '\"':
	  if (!bSlash) {
	    if (bDblQuote) {
	      bDblQuote=0;
	      if (i-iLastBlank>1) argc++;
	    } else {
	      bDblQuote=1;
	    }
	    pBuffer[i]=0;
	    iLastBlank=i;
	  }
	  break;
	}
	if (pBuffer[i]=='\\' && !bSlash) bSlash=1;
	bSlash=0;
      }
      if (i-iLastBlank>1) argc++;
    } else {
      CE_Error("no memory available, failed to allocate %d bytes\n", iLen+1);
      iResult=-ENOMEM;
    }
  }

  if (iResult>=0) {
    if (argc>0) {
      //CE_Debug("found %d arguments in \'%s\'\n", argc, pCmdString);
    }
    if (pHeadingArg) argc++; // space for the first argument
    if (arrayTrailingArg && iNofTrailingArgs>0) argc+=iNofTrailingArgs; // space for the last argument
    if (argc>0) {
      argc++; // one additional NULL element to terminate the arguments
      const char** argv=(const char**)malloc(argc*sizeof(char*));
      if (argv) {
	memset(argv, 0, argc*sizeof(char*));
	int j=0;
	int i=0;
	if (pHeadingArg) argv[j++]=pHeadingArg;
	char* pNewArg=NULL;
	for (i=0;iLen>0 && i<=iLen && pBuffer!=NULL && j<argc-1; i++) {
	  //CE_Debug("char # %d %c\n", i, pBuffer[i]);
	  if (pBuffer[i]!=0 && pNewArg==NULL) {
	    //CE_Debug("start argument %d: %s\n", j, &pBuffer[i]);
	    pNewArg=&pBuffer[i];
	  }else if (pBuffer[i]==0 && pNewArg!=NULL) {
	    //CE_Debug("set argument %d: %s\n", j, pNewArg);
	    argv[j++]=pNewArg;
	    pNewArg=NULL;
	  }
	}
	if (arrayTrailingArg) 
	  for (i=0; i<iNofTrailingArgs && j<argc-1; i++) argv[j++]=arrayTrailingArg[i]; 
	iResult=spawnShellProgram(argv, 0, pRB, iVerbosity);
	free(argv);
	argv=NULL;
      } else {
	CE_Error("no memory available, failed to allocate %d bytes\n", argc*sizeof(char*));
	iResult=-ENOMEM;
      }
    } else {
      CE_Warning("empty command\n");
    }
    if (pBuffer!=NULL) free(pBuffer);
    pBuffer=NULL;
  }
  return iResult;
}

int Ctr::flushStreamBuffer(std::vector<uint32_t>* pRB, FILE* fpOutput, int bComplete, const char* pMsg){
  int iResult=0;
  if (pRB) {
    int iSize=pRB->size()*sizeof(std::vector<uint32_t>::value_type);
    if (iSize>0) {
      char* pBuffer=(char*)&(*pRB)[0];
      if (fpOutput) {
	// the simple case just writes the stream buffer as they are to the file
	fwrite(pBuffer, 1, iSize, fpOutput);
	pRB->resize(0);
      } else {
	// using the CE logging method is more complicated, print only full lines
	// unless bComplete==true
	char* pStart=pBuffer;
	char* pNewline=NULL;
	while (pStart!=NULL && iResult>=0 && 
	       ((pNewline=strchr(pStart, '\n'))!=NULL     // print until the next newline 
		|| (bComplete && (pStart<pBuffer+iSize)) // or the remaining content even if there was no newline
		)
		 ) {
	    //CE_Debug("stepping into print loop\n");
	    if (pNewline) {
	      //CE_Debug("found newline at %p\n", pNewline);
	      if (pNewline<pBuffer+iSize) {
		*pNewline=0; // terminate the string at the position of the newline
	      } else {
		CE_Error("unterminated stream buffer\n");
		iResult=-EFAULT;
		break;
	      }
	    }
	    const char* pHeader=pMsg;
	    if (pHeader==NULL) pHeader="message from program stderr channel";
	    CE_Debug("%s:\n%s\n", pHeader, pStart);
	    if (pNewline && (pNewline+1)<pBuffer+iSize) pStart=pNewline+1; 
	    else pStart=NULL;
	  }
	if (pStart!=NULL && pStart<pBuffer+iSize) {
	  CE_Warning("partial flushing currentky not implemented, print out truncated\n");
	}
	pRB->resize(0);
      }
    }
  } else {
    CE_Error("invalid pointer\n");
    iResult=-EINVAL;
  }
  return iResult=0;
}

