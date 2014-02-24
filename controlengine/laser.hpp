/************************************************************************
** This file is property of and copyright by the Experimental Nuclear 
** Physics Group, Dep. of Physics and Technology
** University of Bergen, Norway, 2009
** This file has been written by Dominik Fehlker
** Please report bugs to Dominik.Fehlker@uib.no
**
** Permission to use, copy, modify and distribute this software and its  
** documentation strictly for non-commercial purposes is hereby granted  
** without fee, provided that the above copyright notice appears in all  
** copies and that both the copyright notice and this permission notice  
** appear in the supporting documentation. The authors make no claims    
** about the suitability of this software for any purpose. It is         
** provided "as is" without express or implied warranty.                 
**  
*************************************************************************/

#ifndef __LASER_HPP
#define __LASER_HPP

#include <vector>
#include "ctr.hpp"
#include "issuehandler.hpp"
#include "dev.hpp"

/******************************************************************************/

class CtrLaser: public Ctr{
public:
  CtrLaser();
  virtual ~CtrLaser();
protected:
  //Laser sub device
  Dev* fpLaser;
};

/******************************************************************************/

class DevMsgbuffer;

/**
 * The FeeServer ControlEngine for the Laser Sync Box.
 * This Controlengine communicates with and controls the Laser Sync Box,
 * which synchronises the Laser system with the TPC readout. It also incorporates a
 * warmup mode which avoids Laser wear.
 * 
 * The implemented statemachine reflects the different running modes:
 *
 *
 *       
 *                        69
 *                   +--------------+
 *                   |  ON_LIMITED  |---------------------------+
 *                   +--------------+                           |
 *                       ^                                      |
 *                       |                                      |
 *        65             |               64                     |
 *  +--------------+     |          +--------------+            |
 *  |  ON_TRIGGER  |     |          | ON_FREE_RUN  |    GO_STBY_CONFIGURED
 *  +--------------+     |          +--------------+            |
 *     |      ^      GO_ON_LIMITED     ^          |             |
 *     |       \         |            /           |             |
 *     |        \        |           /             \            |
 *     |         \       |          /               \           |
 *     |          \      |         /                 \          |
 *     |           \     |   GO_ON_FREE_RUN           \         |
 *     |  GO_ON_TRIGGER  |       /                     |        |
 *     |             \   |      /                      |        |
 *     |              \  |     /                       |        |
 *     |               \ | 67 /                        |        |
 *     |             +--------------+     GO_STBY_CONFIGURED    |
 *     |             |    WARM_UP   |                 |         |
 *     |             +--------------+                 |         |
 *     |                 ^        |                   |         |
 *GO_STBY_CONFIGURED     |        |                   |         |
 *     |                 |        |                   |         |
 *     |       GO_WARM_UP|        |GO_STBY_CONFIGURED |         |
 *     |                 |        |                   |         |
 *     |                 |        |                   |         |
 *     |                 |        |                   |         |
 *     |                 |   3    v                   |         |
 *     |            +-----------------+               |         |
 *     +-------+--->| STBY_CONFIGURED |----+----------+---------+
 *             |    +-----------------+    |
 *             |                           |
 *  LOAD_RECIPE|                           |GO_STANDBY
 *             |             5             |
 *             |      +--------------+     |
 *             +------|   STANDBY    |<----+
 *                    +--------------+ 
 * 
 * 
 * 
 * 
 * 
 **/

class DevLaser: public Dev{

public:

  DevLaser(int id,  CEStateMachine* pParent, DevMsgbuffer* msgbuffer);

/**
  * Handling of binary Laser commands.
  * @see CEIssueHandler::issue for parameters and return values.
  *
  * @ingroup rcu_issue
  */ 
  virtual int issue(uint32_t cmd, uint32_t parameter, const char* pData, int iDataSize, std::vector<uint32_t>& rb); 

  /**
   * Handler for high level commands.
   * @see CEIssueHandler::HighLevelHandler.
   * @param command   command string
   * @param rb        result buffer
   * @return 1 if processed, 0 if not, neg. error code if failled 
   */
  virtual int HighLevelHandler(const char* pCommand, std::vector<uint32_t>& rb);

protected:
  virtual int EnterStateCONFIGURED();

  /**
   * In the case of this Mode, the following is performed:
   * - enable global
   * - check that ModeFreqVal is not equal to 0
   * @return            neg. error code if failed
   */
  virtual int EnterStateON_FREE_RUN();

  /**
   * In the case of this Mode, the following is performed:
   * - disable global
   * @return            neg. error code if failed
   */
  virtual int LeaveStateON_FREE_RUN();

  /**
   * In the case of this Mode, the following is performed:
   * - setting ModeFreqVal to 0
   * - global enable
   * @return            neg. error code if failed
   */
  virtual int EnterStateON_TRIGGER();

  /**
   * In the case of this Mode, the following is performed:
   * - global disable
   * @return            neg. error code if failed
   */
  virtual int LeaveStateON_TRIGGER();

  /**
   * In the case of this Mode, the following is performed:
   * - enable the warmup mode
   * @return            neg. error code if failed
   */
  virtual int EnterStateWARM_UP();
  
  /**
   * @return            neg. error code if failed
   */
  virtual int EnterStateON_LIMITED();
  
  /**
   * @return            neg. error code if failed
   */
  virtual int LeaveStateON_LIMITED();

  /**
   * Evaluate the state of the hardware.
   * The function does not change any internal data, it performes a number of evaluation
   * operations.
   * @return             state
   */
  virtual int SynchronizeLocal();


  virtual int ReSynchronizeLocal();

  /**
   * Internal function called during the @ref CEStateMachine::Armor procedure.
   * The function is called from the @ref CEStateMachine base class to carry out
   * device specific start-up tasks.
   * 
   *@return result value
   */
  virtual int ArmorLocal();

  /**
   * Sets and removes the clear counters bit in the Laser firmware RunMode register
   * 
   * @return result value
   **/
  virtual int ClearCounters();

  /**
   * Calculates the Ekspla Start Time from the parameters.
   *
   * @param pDataOrig The list of parameters needed by the command:
   * - Shuttertime
   * - Flash Ekspla Time
   * - Shift
   *
   * Formula: Shuttertime * Cms + (Flash Ekspla Time + Shift) * Cus
   *
   * @return result value
   **/
  virtual int SetLaserFlashStartTime(const char* pDataOrig, uint32_t reg);

  /**
   * Calculates the Ekspla End Time from the parameters.
   *
   * @param pDataOrig The list of parameters needed by the command:
   * - Shuttertime
   * - Flash Ekspla Time
   * - Flash ekspla duration
   * - Shift
   *
   * Formula: Shuttertime * Cms + (Flash Ekspla Time + Flash Ekspla duration + Shift) * Cus
   *
   * @return result value
   **/
  virtual int SetLaserFlashEndTime(const char* pDataOrig, uint32_t reg);


  /**
   * Calculates the L0 Request Time
   * @param pdataOrig The list of parameters needed by the command
   * - Shuttertime
   * - L0
   * - Shift
   *
   * Formula: Shuttertime * Cms + (L0 + Shift) * Cus
   *
   * @return result value
   **/
  virtual int SetL0RequestTime(const char* pDataOrig);

  /**
   * Calculates the L0 return window start
   * @param pdataOrig The list of parameters needed by the command
   * - Shuttertime
   * - L0
   * - L01
   * - Shift
   *
   * Formula: Shuttertime * Cms + (L0 + L01 + Shift) * Cus
   *
   * @return result value
   **/
  virtual int SetL0ReturnWindowStart(const char* pDataOrig);

  /**
   * Calculates the L0 return window end
   * @param pdataOrig The list of parameters needed by the command
   * - Shuttertime
   * - L0
   * - L01
   * - L02
   * - Shift
   *  
   * Formula: Shuttertime * Cms + (L0 + L01 + L02 + Shift) * Cus
   *
   * @return result value
   **/
  virtual int SetL0ReturnWindowEnd(const char* pDataOrig);

  /**
   * Calculate the Ekspla Q-Switch start time
   * @param pdataOrig The list of parameters needed by the command
   * - Shuttertime
   * - Q-Switch Ekpla time
   * - Shift
   *
   * Formula: Shuttertime * Cms + (Q-Switch Ekspla start time + Shift) * Cus
   *
   * @return result value
   **/
  virtual int SetLaser1QSwitchStart(const char* pDataOrig);


  /**
   * Calculate the Ekspla Q-Switch end time
   * @param pdataOrig The list of parameters needed by the command
   * - Shuttertime
   * - Q-Switch Ekpla time
   * - Q-Switch Ekspla duration
   * - Shift
   *
   * Formula: Shuttertime * Cms + (Q-Switch Ekspla start time + Q-Switch Ekspla duration + Shift) * Cus
   *
   * @return result value
   **/
  virtual int SetLaser1QSwitchEnd(const char* pDataOrig);

  /**
   * Sets the Enable Ekspla Bit in the RunMode register
   *
   * @return result value
   **/
  virtual int EnableLaser1();

  /**
   * Removes the Enable Ekspla Bit in the RunMode register
   *
   * @return result value
   **/
  virtual int EnableLaser2();
      
  /**
   * Sets the WarmupLock Mode bit in the RunMode register
   *
   * @return result value
   **/
  virtual int EnableWarmupLock();

  /**
   * Sets the Global Enable bit in the RunMode register
   *
   * @return result value
   **/
  virtual int EnableGlobal();

  /**
   * Removes the Enable Ekspla Bit in the RunMode register
   *
   * @return result value
   **/
  virtual int DisableLaser1();

  /**
   * Removes the Enable Spectron Bit in the RunMode register
   *
   * @return result value
   **/
  virtual int DisableLaser2();

  /**
   * Removes the WarmupLock bit in the RunMode register
   *
   * @return result value
   **/
  virtual int DisableWarmupLock();

  /**
   *
   *
   * @return result value
   **/
  virtual int DisableGlobal();

  /**
   *
   *
   * @return result value
   **/
  virtual int EnableLaser();

  /**
   *
   *
   * @return result value
   **/
  virtual int DisableLaser();



  
  /**
   * Get id of the RCU command group
   * @return          the command id (@ref FEESVR_CMD_RCU)
   */
  virtual int GetGroupId();

  /*
   * The instance of the messagebuffer interface
   */
  DevMsgbuffer* fpMsgbuffer;
};

/******************************************************************************/
#define BASE_FPGA_FABRIC_FIC1   0x50000000


/*************************************
 **** Configuration Registers   ******
 ************************************/

#define ModeFreqVal                     0x0001
#define ModeFreqVal_WIDTH               32
#define ModeFreqVal_SIZE                1

#define Laser1_FlashStartTime            0x0002
#define Laser1_FlashStartTime_WIDTH      32
#define Laser1_FlashStartTime_SIZE       1

#define Laser1_FlashEndTime              0x0003
#define Laser1_FlashEndTime_WIDTH        32
#define Laser1_FlashEndTime_SIZE         1

#define Laser2_FlashStartTime          0x0004
#define Laser2_FlashStartTime_WIDTH    32
#define Laser2_FlashStartTime_SIZE     1

#define Laser2_FlashEndTime            0x0005
#define Laser2_FlashEndTime_WIDTH      32
#define Laser2_FlashEndTime_SIZE       1

#define L0RequestTime                   0x0007
#define L0RequestTime_WIDTH             32
#define L0RequestTime_SIZE              1

#define L0ReturnWindowStart             0x0008
#define L0ReturnWindowStart_WIDTH       32
#define L0ReturnWindowStart_SIZE        1

#define L0ReturnWindowStop              0x0009
#define L0ReturnWindowStop_WIDTH        32
#define L0ReturnWindowStop_SIZE         1

#define Laser1_QswitchStart              0x000A
#define Laser1_QswitchStart_WIDTH        32
#define Laser1_QswitchStart_SIZE         1

#define Laser1_QswitchStop               0x000B
#define Laser1_QswitchStop_WIDTH         32
#define Laser1_QswitchStop_SIZE          1

#define Laser2_QswitchStart              0x0014
#define Laser2_QswitchStart_WIDTH        32
#define Laser2_QswitchStart_SIZE         1

#define Laser2_QswitchStop               0x0015
#define Laser2_QswitchStop_WIDTH         32
#define Laser2_QswitchStop_SIZE          1


/**
RunMode register
00: standby
01: warmup
10: laser
**/
#define RunMode                         0x000C
#define RunMode_WIDTH                   32
#define RunMode_SIZE                    1

#define SampleClockDivider              0x000D
#define SampleClockDivider_WIDTH        32
#define SampleClockDivider_SIZE         1

#define TriggerConfiguration1           0x000E
#define TriggerConfiguration1_WIDTH     32
#define TriggerConfiguration1_SIZE      1

#define TriggerConfiguration2           0x000F
#define TriggerConfiguration2_WIDTH     32
#define TriggerConfiguration2_SIZE      1

/**
Maximum number of Flash events in warmup or laser mode
**/
#define MaxNumberOfFlashEvents          0x0012
#define MaxNumberOfFlashEvents_WIDTH    32
#define MaxNumberOfFlashEvents_SIZE     1

/**
Number of laser events wanted in a burst, 0 - infinity
 **/
#define NumberOfEventsInBurst           0x0011
#define NumberOfEventsInBurst_WIDTH     32
#define NumberOfEventsInBurst_SIZE      1

/**
Laser enable register
Bit 0: general enable
Bit 1: enable Laser1
Bit 2: enable Laser2
Bit 3: clear counters
**/
#define LaserEnables                    0x0013
#define LaserEnables_WIDTH              32
#define LaserEnables_SIZE               1

/****************************************
 **** Monitoring Registers   ******
 ***************************************/

#define RCUVersion                      (0x5106 | BASE_FPGA_FABRIC_FIC1)
#define RCUVersion_WIDTH                32
#define RCUVersion_SIZE                 1

#define ArmSync                         (0x530A | BASE_FPGA_FABRIC_FIC1)
#define ArmSync_WIDTH                   32
#define ArmSync_SIZE                    1

//Touch to clear the SOD flag.
#define ClearSOD                        (0x530B | BASE_FPGA_FABRIC_FIC1)
#define ClearSOD_WIDTH                  32
#define ClearSOD_SIZE                   1

#define CTPsignature                    0x0010
#define CTPsignature_WIDTH              32
#define CTPsignature_SIZE               1

#define ShutterCounter                  0x0021
#define ShutterCounter_WIDTH            32
#define ShutterCounter_SIZE             1

#define Laser1_FlashCounter               0x0022
#define Laser1_FlashCounter_WIDTH         32
#define Laser1_FlashCounter_SIZE          1

#define Laser2_FlashCounter             0x0023
#define Laser2_FlashCounter_WIDTH       32
#define Laser2_FlashCounter_SIZE        1

#define Laser1_QswitchCounter             0x0024
#define Laser1_QswitchCounter_WIDTH       32
#define Laser1_QswitchCounter_SIZE        1

#define Laser2_QswitchCounter           0x0025
#define Laser2_QswitchCounter_WIDTH     32
#define Laser2_QswitchCounter_SIZE      1

#define L0RequestCounter                 0x0026
#define L0RequestCounter_WIDTH           32
#define L0RequestCounter_SIZE            1

#define L0ReceivedCounter                0x0027
#define L0ReceivedCounter_WIDTH          32
#define L0ReceivedCounter_SIZE           1

#define L0ReceivedInWindowCounter        0x0028
#define L0ReceivedInWindowCounter_WIDTH  32
#define L0ReceivedInWindowCounter_SIZE   1

#define L0TimeoutCounter                 0x0029
#define L0TimeoutCounter_WIDTH           32
#define L0TimeoutCounter_SIZE            1

#define L0ReturnTimer                    0x002A
#define L0ReturnTimer_WIDTH              32
#define L0ReturnTimer_SIZE               1

#define ActualLaserRate                  0x002B
#define ActualLaserRate_WIDTH            32
#define ActualLaserRate_SIZE             1

#define CycleTimer                       0x002C
#define CycleTimer_WIDTH                 32
#define CycleTimer_SIZE                  1

#define Heartbeat                        0x002D
#define Heartbeat_WIDTH                  32
#define Heartbeat_SIZE                   1

#define MonitoringRegister               0x002E
#define MonitoringRegister_WIDTH         32
#define MonitoringRegister_SIZE          1

#define FlashCounter                     0x002F
#define FlashCounter_WIDTH               32
#define FlashCounter_SIZE                1

#define LaserCounter                     0x0030
#define LaserCounter_WIDTH               32
#define LaserCounter_SIZE                1

/*****************************************
 **** Bitmasks                      ******
 *****************************************/

//RunMode Register
#define RunModeMaskWarmup                0x1
#define RunModeMaskLaser                 0x2

//Enable Register
#define EnableMaskGlobal                 0x1
#define EnableMaskLaser1                 0x2
#define EnableMaskLaser2                 0x4
#define EnableClearCounters              0x8

//Monitoring Register
#define MonitoringMaskFSMState           0x38
#define MonitoringMaskGeneralEnable      0x1
#define MonitoringMaskEnableLaser1       0x2
#define MonitoringMaskEnableLaser2       0x4
#define MonitoringMaskWarmUp             0x8
#define MonitoringMaskLaser              0x10
#define MonitoringMaskStopFlash          0x20
#define MonitoringMaskSODreceived        0x40

/*****************************************
 **** Trigger Interface registers   ******
 *****************************************/

#define TTCControl		(0x4000 | BASE_FPGA_FABRIC_FIC1)
#define TTCControl_WIDTH        24
#define TTCControl_SIZE         1

#define TTCModulkActionReset          (0x4001 | BASE_FPGA_FABRIC_FIC1)
#define TTCModulkActionReset_WIDTH    1
#define TTCModulkActionReset_SIZE     1

#define TTCROIConfig1	        (0x4002 | BASE_FPGA_FABRIC_FIC1)
#define TTCROIConfig1_WIDTH     18
#define TTCROIConfig1_SIZE      1

#define TTCROIConfig2	        (0x4003 | BASE_FPGA_FABRIC_FIC1)
#define TTCROIConfig2_WIDTH     18
#define TTCROIConfig2_SIZE      1

#define TTCResetCounters        (0x4004 | BASE_FPGA_FABRIC_FIC1)
#define TTCReserCounters_WIDTH  1
#define TTCReserCounters_SIZE   1

#define TTCIssueTestmode        (0x4005 | BASE_FPGA_FABRIC_FIC1)
#define TTCIssueTestmode_WIDTH  1
#define TTCIssueTestmode_SIZE   1

#define TTCL1Latency     	(0x4006 | BASE_FPGA_FABRIC_FIC1)
#define TTCL1Latency_WIDTH      16
#define TTCL1Latency_SIZE       1

#define TTCL2Latency	        (0x4007 | BASE_FPGA_FABRIC_FIC1)
#define TTCL2Latency_WIDTH      32
#define TTCL2Latency_SIZE       1

#define TTCRoILatency	        (0x4009 | BASE_FPGA_FABRIC_FIC1)
#define TTCRoILatency_WIDTH     32
#define TTCRoILatency_SIZE      1

#define TTCL1MsgLatency	        (0x400A | BASE_FPGA_FABRIC_FIC1)
#define TTCL1MsgLatency_WIDTH   32
#define TTCL1MsgLatency_SIZE    1

#define TTCPrePulseCnt	        (0x400B | BASE_FPGA_FABRIC_FIC1)
#define TTCPrePulseCnt_WIDTH    16
#define TTCPrePulseCnt_SIZE     1

#define TTCBCIDLocal	        (0x400C | BASE_FPGA_FABRIC_FIC1)
#define TTCBCIDLocal_WIDTH      12
#define TTCBCIDLocal_SIZE       1

#define TTCL0Counter	        (0x400D | BASE_FPGA_FABRIC_FIC1)
#define TTCL0Counter_WIDTH      16
#define TTCL0Counter_SIZE       1

#define TTCL1Counter	        (0x400E | BASE_FPGA_FABRIC_FIC1)
#define TTCL1Counter_WIDTH      16
#define TTCL1Counter_SIZE       1

#define TTCL1MsgCounter	        (0x400F | BASE_FPGA_FABRIC_FIC1)
#define TTCL1MsgCounter_WIDTH   16
#define TTCL1MsgCounter_SIZE    1

#define TTCL2aCounter	        (0x4010 | BASE_FPGA_FABRIC_FIC1)
#define TTCL2aCounter_WIDTH     16
#define TTCL2aCounter_SIZE      1

#define TTCL2rCounter	        (0x4011 | BASE_FPGA_FABRIC_FIC1)
#define TTCL2rCounter_WIDTH     16
#define TTCL2rCounter_SIZE      1

#define TTCRoICounter	        (0x4012 | BASE_FPGA_FABRIC_FIC1)
#define TTCRoICounter_WIDTH     16
#define TTCRoICounter_SIZE      1

#define TTCBunchCounter	        (0x4013 | BASE_FPGA_FABRIC_FIC1)
#define TTCBunchCounter_WIDTH   12
#define TTCBunchCounter_SIZE    1

#define TTCHammingErrorCnt	(0x4016 | BASE_FPGA_FABRIC_FIC1)
#define TTCHammingErrorCnt_WIDTH 32
#define TTCHammingErrorCnt_SIZE 1

#define TTCErrorCnt	        (0x4017 | BASE_FPGA_FABRIC_FIC1)
#define TTCErrorCnt_WIDTH       32
#define TTCErrorCnt_SIZE        1

#define TTCBufferedEvents	(0x4020 | BASE_FPGA_FABRIC_FIC1)
#define TTCBufferedEvents_WIDTH 5
#define TTCBufferedEvent_SIZE   1

#define TTCDAQHeader1	        (0x4021 | BASE_FPGA_FABRIC_FIC1)
#define TTCDAQHeader1_WIDTH      32
#define TTCDAQHeader1_SIZE       1

#define TTCDAQHeader2	        (0x4022 | BASE_FPGA_FABRIC_FIC1)
#define TTCDAQHeader2_WIDTH      32
#define TTCDAQHeader2_SIZE       1

#define TTCDAQHeader3	        (0x4023 | BASE_FPGA_FABRIC_FIC1)
#define TTCDAQHeader3_WIDTH      32
#define TTCDAQHeader3_SIZE       1

#define TTCDAQHeader4	        (0x4024 | BASE_FPGA_FABRIC_FIC1)
#define TTCDAQHeader4_WIDTH      32
#define TTCDAQHeader4_SIZE       1

#define TTCDAQHeader5	        (0x4025 | BASE_FPGA_FABRIC_FIC1)
#define TTCDAQHeader5_WIDTH      32
#define TTCDAQHeader5_SIZE       1

#define TTCDAQHeader6	        (0x4026 | BASE_FPGA_FABRIC_FIC1)
#define TTCDAQHeader6_WIDTH      32
#define TTCDAQHeader6_SIZE       1

#define TTCDAQHeader7	        (0x4027 | BASE_FPGA_FABRIC_FIC1)
#define TTCDAQHeader7_WIDTH      32
#define TTCDAQHeader7_SIZE       1

#define TTCEventInfo	        (0x4028 | BASE_FPGA_FABRIC_FIC1)
#define TTCEventInfo_WIDTH      18
#define TTCEventInfo_SIZE       1

#define TTCEventError	        (0x4029 | BASE_FPGA_FABRIC_FIC1)
#define TTCEventError_WIDTH     25
#define TTCEventError_SIZE      1

#define TTCL1MsgHeader	        (0x4030 | BASE_FPGA_FABRIC_FIC1)
#define TTCL1MsgHeader_WIDTH    12
#define TTCL1MsgHeader_SIZE     1

#define TTCL1MsgData	        (0x4031 | BASE_FPGA_FABRIC_FIC1)
#define TTCL1MsgData_WIDTH      12
#define TTCL1MsgData_SIZE       1

#define TTCL2aMsgHeader 	(0x4035 | BASE_FPGA_FABRIC_FIC1)
#define TTCL2aMsgHeader_WIDTH   12
#define TTCL2aMsgHeader_SIZE    1

#define TTCL2aMsgData	        (0x4036 | BASE_FPGA_FABRIC_FIC1)
#define TTCL2aMsgData_WIDTH     12
#define TTCL2aMsgData_SIZE      1

#define TTCL2rMsgHeader	        (0x403D | BASE_FPGA_FABRIC_FIC1)
#define TTCL2rMsgHeader_WIDTH   12
#define TTCL2rMsgHeader_SIZE    1

#define TTCRoIMsgHeader	        (0x403E | BASE_FPGA_FABRIC_FIC1)
#define TTCRoIMsgHeader_WIDTH   12
#define TTCRoIMsgHeader_SIZE    1

#define TTCRoIMsgData	        (0x403F | BASE_FPGA_FABRIC_FIC1)
#define TTCRoIMsgData_WIDTH     12
#define TTCRoIMsgData_SIZE      1

#define TTCFIFODAQHeader	(0x4081 | BASE_FPGA_FABRIC_FIC1)
#define TTCFIFODAQHeader_WIDTH  32
#define TTCFIFODAQHeader_SIZE   1

#endif //__LASER_HPP
