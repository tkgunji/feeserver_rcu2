#ifndef __DEV_HPP
#define __DEV_HPP
#include <vector>
#include <cerrno>
#include "issuehandler.hpp"
#include "statemachine.hpp"
#include "rcu2_issue.h"
class Ser;

class Dev : public CEStateMachine, public CEIssueHandler {
/*New functions in Dev:*/
public:
  Dev(std::string name, int id,  CEStateMachine* pParent);
  virtual ~Dev();
//virtual int PreUpdate();
protected:
  std::vector<Ser*> fServices;
/*Virtual functions from CEThreadManager: none.*/

/*Virtual functions from CEStateMachine:*/
public:
//virtual int UpdateStateChannel();
protected:
//virtual int CreateStateChannel();
//virtual int CreateAlarmChannel();
//virtual int SendAlarm(int alarm);
//virtual int IsAllowedTransition(CETransition* pT);
//virtual int TransitionDispatcher(CETransition* transition, CEDevice* pDevice, int iParam, void* pParam);
//virtual const char* GetDefaultStateName(int state);
//virtual const char* GetDefaultTransitionName(CETransition* pTransition);

/*Virtual functions from CEDevice:*/
protected:
//virtual int ArmorLocal();
//virtual int SynchronizeLocal();
private:
//virtual int SubDevicesLocked();

/*Virtual functions from CEDimDevice: none.*/

/*Virtual functions from CEIssueHandler:*/
public:
//virtual int GetGroupId(); //Pure virtual
//virtual int CheckCommand(uint32_t cmd, uint32_t parameter);
  virtual int issue(uint32_t cmd, uint32_t parameter, const char* pData, int iDataSize, std::vector<uint32_t>& rb); //Pure virtual
//virtual int HighLevelHandler(const char* pCommand, std::vector<uint32_t>& rb);
};
#endif //__DEV_HPP
