#ifndef __ISSUEHANDLER_HPP
#define __ISSUEHANDLER_HPP

#include <stdint.h>
#include <vector>
#include <string>
#include <sys/types.h>
#include "rcu2_issue.h"

/**
 * @class CEIssueHandler
 * @brief helper class for the handling of the Command interpretation for the RCU FeeServer
 *
 * This class implements a framework for handling Command interpretation in the RCU FeeServer
 * Here we can register and unregister the different commands in a global list. The list of the 
 * commandos will be processed in the FeeServer
 *
 * @ingroup rcu_ce
 */

class CEIssueHandler {
  
public:
  
  /**
   * standard constructor. The object is automatically registered in the
   * global CEIssueHandler-list
   */
  CEIssueHandler();
  
  /**
   * standard destructor. The object is automatically unregistered from the global
   * CEIssueHandler-list
   */
  virtual ~CEIssueHandler();
  
  /**
   * Virtual method for returning the ID of the Command-group. 
   * This function will be overwritten by the different classes for the command-groups.
   * @return             GroupId, -1 if id not valid
   */
  virtual int GetGroupId();

  /**
   * Virtual method for handling commands in FeeServer
   * This function will be overwritten by the different classes for the command-groups.
   * @param cmd       complete command id (upper 16 bit of 4 byte header)
   * @param parameter command parameter extracted from the 4 byte header (lower 16 bit)
   * @param pData     pointer to the data following the header
   * @param iDataSize size of the data in bytes
   * @return          number of bytes of the payload buffer which have been processed
   *                  neg. error code if failed
   *                  - -ENOSYS  unknown command
   */
  virtual int issue(uint32_t cmd, uint32_t parameter, const char* pData, int iDataSize, std::vector<uint32_t>& rb)=0;

  /**
   * Handler for high level commands.
   * The handler gets a single string containing the command. The general form of
   * a high level command is:
   * <pre>
   *    \<fee\> command \</fee\>
   * </pre>
   * The header and trailer keywords are cur out by the framework.
   * @param command   command string
   * @return >0 if processed, 0 if not
   *         - -EEXIST   command available, but target/device wrong
   *         - -ENOSYS   handler not implemented
   */
  virtual int HighLevelHandler(const char* pCommand, std::vector<uint32_t>& rb);
  
  static std::vector<CEIssueHandler*> issueHandlers;
  static std::vector<uint32_t> rb;
  static int hardware;
  static int Issue(const char* command, int size, std::vector<uint32_t>& rb);
  static int IssueBinary(const char* command, int size, std::vector<uint32_t>& rb);
  static int IssueFee(std::string& command, std::vector<uint32_t>& rb);
  static int IssueAction(std::string& command, std::vector<uint32_t>& rb);
  static int IssueConfig(std::string& command, std::vector<uint32_t>& rb);
};

/**
 * @class ShellCommandHandler
 * CEIssueHandler for the Shell command group.
 * <b>Note:</b> none of shell execution commands can appear in a command
 * sequence since they take all the payload as the script/programm and
 * don't care about end-markers.
 *
 * @ingroup rcu_ce_base_services
 */
class ShellCommandHandler : public CEIssueHandler{
 public:
  /** constructor */
  ShellCommandHandler();

  /** destructor */
  ~ShellCommandHandler();

  /**
   * Get group id.
   */
  int GetGroupId();

  /**
   * Issue handler for shell command group.
   * Main switch for the @ref FEESVR_CMD_SHELL shell command execution group.
   * <b>Note:</b> none of shell execution commands can appear in a command
   * sequence since they take all the payload as the script/programm and
   * don't care about end-markers.
   * @see CEIssueHandler::issue for parameters and return values
   */
  int issue(uint32_t cmd, uint32_t parameter, const char* pData, int iDataSize, std::vector<uint32_t>& rb);

  /* @struct  streamBuffer
   * @ingroup CE_shex
   * @brief   The stream buffer descriptor
   */
  typedef struct streamBuffer_t TstreamBuffer;
  struct streamBuffer_t {
    int iSize;
    char* pBuffer;
    TstreamBuffer* pNext;
  };
};
#endif //__ISSUEHANDLER_HPP

