#ifndef __CTR_HPP
#define __CTR_HPP

#include <vector>
#include "issuehandler.hpp"
#include "statemachine.hpp"
#include "lockguard.hpp"
#include "ser.hpp"
#include "ce_interface.h"

class CEStateMachine;
class Ctr:public CEStateMachine,public CEIssueHandler{
public:
  Ctr();
  virtual ~Ctr();
  int RunCE();
  virtual int GetGroupId();
  virtual int HighLevelHandler(const char* pCommand, std::vector<uint32_t>& rb);
  virtual int issue(uint32_t cmd, uint32_t parameter, const char* pData, int iDataSize, std::vector<uint32_t>& rb);
  struct message_t{
    unsigned int eventType;
    char         detector[4];
    char         source[256];
    char         description[256];
    char         date[20];
  };
  virtual uint32_t EvaluateHwAddress(uint32_t hwAddress, uint32_t mask) const;
  static message_t message;
  static message_t messageDim;
  static void* logservice;
  static void* enservice;
  static std::string source;
  static std::string tmpDir;
  static int enableLogging;
  static int enableServices;
  static int loglevel;
  static int terminating;
  static int updateSleep;
  static Ctr* fpInstance;
  static CE_Mutex fMutex;
  static int ceLogMessage(const char* file, int lineNo, int loglevel, const char* format, ...);
  static int updateServickActionStartTime(Ser::TceServiceData* pData, int major, int minor, void* parameter);
  static int updateServiceUpdateTime(Ser::TceServiceData* pData, int major, int minor, void* parameter);
  static int updateServiceUpdateCounter(Ser::TceServiceData* pData, int major, int minor, void* parameter);
  enum{
    eCEAll = 0,
    eCEDebug = 2,
    eCEEvent = 4,
    eCEWarning = 5,
    eCEError = 6,
  };
protected:
  virtual int SynchronizeLocal();
  virtual int ReSynchronizeLocal();
  virtual int EnterStateIDLE();
  virtual int EnterStateSTANDBY(); 
  DevMsgbuffer* fpMsgbuffer;
  uint32_t fHwType;
  ShellCommandHandler* fpShellCommandHandler;
  static std::vector<uint32_t> configurations;
  static int callShellProgram(const char* pHeadingArg, const char* pCmdString, int iStringSize, const char** arrayTrailingArg, int iNofTrailingArgs, std::vector<uint32_t>* pRB, int iVerbosity);
  static int spawnShellProgram(char const **argv, int bNonBlocking, std::vector<uint32_t>* pRB, int iVerbosity);
  static int readStreamIntoVector(FILE* fp, int iDefaultBufferSize, std::vector<uint32_t>* pRB);
  static int flushStreamBuffer(std::vector<uint32_t>* pRB, FILE* fpOutput, int bComplete, const char* pMsg);
};

#define CE_Debug(format, ...)      Ctr::ceLogMessage(__FILE__, __LINE__, Ctr::eCEDebug,     format, ##__VA_ARGS__)
#define CE_Event(format, ...)      Ctr::ceLogMessage(__FILE__, __LINE__, Ctr::eCEEvent,     format, ##__VA_ARGS__)
#define CE_Warning(format, ...)    Ctr::ceLogMessage(__FILE__, __LINE__, Ctr::eCEWarning,   format, ##__VA_ARGS__)
#define CE_Error(format, ...)      Ctr::ceLogMessage(__FILE__, __LINE__, Ctr::eCEError,     format, ##__VA_ARGS__)

#endif //__CTR_HPP

