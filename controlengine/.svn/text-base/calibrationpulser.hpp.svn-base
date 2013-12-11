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

#ifndef __CALIBRATIONPULSER_HPP
#define __CALIBRATIONPULSER_HPP

#include "ctr.hpp"
#include "dev.hpp"

/******************************************************************************/

class CtrCpulser: public Ctr{
public:
  CtrCpulser();
  virtual ~CtrCpulser();
protected:
  //Cpulser sub device
  Dev* fpCpulser;
};

/******************************************************************************/

class DevMsgbuffer;

class DevCpulser: public Dev{
public:
  DevCpulser(int id, CEStateMachine* pParent, DevMsgbuffer* msgbuffer);
  virtual int issue(uint32_t cmd, uint32_t parameter, const char* pData, int iDataSize, std::vector<uint32_t>& rb);
  virtual int HighLevelHandler(const char* pCommand, std::vector<uint32_t>& rb);
protected:
  virtual int GetGroupId();
  virtual int ArmorLocal();
  virtual int SynchronizeLocal();
  virtual int ReSynchronizeLocal();
  DevMsgbuffer* fpMsgbuffer;
};

/******************************************************************************/

/*************************************
 **** Configuration Registers   ******
 ************************************/

#define FineDelay		0x5100
#define FineDelay_WIDTH         8
#define FineDelay_SIZE          1

#define Channel0		0x5101
#define Channel0_WIDTH          8
#define Channel0_SIZE           1

#define Channel1	        0x5102
#define Channel1_WIDTH          8
#define Channel1_SIZE           1

#define Control                 0x5103
#define Control_WIDTH           8
#define Control_SIZE            1

#define CoarseDelay		0x5104
#define CoarseDelay_WIDTH       8
#define CoarseDelay_SIZE        1

#define Amplitude		0x5105
#define Amplitude_WIDTH         8
#define Amplitude_SIZE          1

#define PulseShape		0x5106
#define PulseShape_WIDTH        8
#define PulseShape_SIZE         1

#define InstructionRegister     0x5107
#define InstructionRegister_WIDTH     8   
#define InstructionRegister_SIZE      1  

#define IssuedTriggers		0x5112
#define IssuedTriggers_WIDTH    32
#define IssuedTriggers_SIZE     1  

#define PulserFW		0x5111
#define PulserFW_WIDTH          32
#define PulserFW_SIZE           1

#define PulserConf		0x5201
#define PulserConf_WIDTH        32
#define PulserConf_SIZE         1

/*****************************************
 ********   Command registers   **********
 *****************************************/
#define SwitchToLocal           0x5507
#define SwitchToLocal_WIDTH     8
#define SwitchToLocal_SIZE      1

#define SwitchToRemote          0x5508
#define SwitchToRemote_WIDTH    8
#define SwitchToRemote_SIZE     1

#define AllChannelsOff          0x550A
#define AllChannelsOff_WIDTH    8
#define AllChannelsOff_SIZE     1

#define AllChannelsOn           0x550B
#define AllChannelsOn_WIDTH     8
#define AllChannelsOn_SIZE      1

#define RemotkActionReset             0x550F
#define RemotkActionReset_WIDTH       32
#define RemotkActionReset_SIZE        1

#define TestTrigger             0x5301

#define GlobalReset             0x5300

//To send the ArmSync command, send 0xffffffff to register 0x530A
#define ArmSync                 0x530A
#define ArmSync_WIDTH           32
#define ArmSync_SIZE            1

/*****************************************
 **** Trigger Interface registers   ******
 *****************************************/
#define TTCControl		0x4000
#define TTCControl_WIDTH        24
#define TTCControl_SIZE         1

#define TTCModulkActionReset          0x4001
#define TTCModulkActionReset_WIDTH    1
#define TTCModulkActionReset_SIZE     1

#define TTCROIConfig1	        0x4002
#define TTCROIConfig1_WIDTH     18
#define TTCROIConfig1_SIZE      1

#define TTCROIConfig2	        0x4003
#define TTCROIConfig2_WIDTH     18
#define TTCROIConfig2_SIZE      1

#define TTCResetCounters        0x4004
#define TTCReserCounters_WIDTH  1
#define TTCReserCounters_SIZE   1

#define TTCIssueTestmode        0x4005
#define TTCIssueTestmode_WIDTH  1
#define TTCIssueTestmode_SIZE   1

#define TTCL1Latency     	0x4006
#define TTCL1Latency_WIDTH      16
#define TTCL1Latency_SIZE       1

#define TTCL2Latency	        0x4007
#define TTCL2Latency_WIDTH      32
#define TTCL2Latency_SIZE       1

#define TTCRoILatency	        0x4009
#define TTCRoILatency_WIDTH     32
#define TTCRoILatency_SIZE      1

#define TTCL1MsgLatency	        0x400A
#define TTCL1MsgLatency_WIDTH   32
#define TTCL1MsgLatency_SIZE    1

#define TTCPrePulseCnt	        0x400B
#define TTCPrePulseCnt_WIDTH    16
#define TTCPrePulseCnt_SIZE     1

#define TTCBCIDLocal	        0x400C
#define TTCBCIDLocal_WIDTH      12
#define TTCBCIDLocal_SIZE       1

#define TTCL0Counter	        0x400D
#define TTCL0Counter_WIDTH      16
#define TTCL0Counter_SIZE       1

#define TTCL1Counter	        0x400E
#define TTCL1Counter_WIDTH      16
#define TTCL1Counter_SIZE       1

#define TTCL1MsgCounter	        0x400F
#define TTCL1MsgCounter_WIDTH   16
#define TTCL1MsgCounter_SIZE    1

#define TTCL2aCounter	        0x4010
#define TTCL2aCounter_WIDTH     16
#define TTCL2aCounter_SIZE      1

#define TTCL2rCounter	        0x4011
#define TTCL2rCounter_WIDTH     16
#define TTCL2rCounter_SIZE      1

#define TTCRoICounter	        0x4012
#define TTCRoICounter_WIDTH     16
#define TTCRoICounter_SIZE      1

#define TTCBunchCounter	        0x4013
#define TTCBunchCounter_WIDTH   12
#define TTCBunchCounter_SIZE    1

#define TTCHammingErrorCnt	0x4016
#define TTCHammingErrorCnt_WIDTH 32
#define TTCHammingErrorCnt_SIZE 1

#define TTCErrorCnt	        0x4017
#define TTCErrorCnt_WIDTH       32
#define TTCErrorCnt_SIZE        1

#define TTCBufferedEvents	0x4020
#define TTCBufferedEvents_WIDTH 5
#define TTCBufferedEvent_SIZE   1

#define TTCDAQHeader1	        0x4021
#define TTCDAQHeader1_WIDTH      32
#define TTCDAQHeader1_SIZE       1

#define TTCDAQHeader2	        0x4022
#define TTCDAQHeader2_WIDTH      32
#define TTCDAQHeader2_SIZE       1

#define TTCDAQHeader3	        0x4023
#define TTCDAQHeader3_WIDTH      32
#define TTCDAQHeader3_SIZE       1

#define TTCDAQHeader4	        0x4024
#define TTCDAQHeader4_WIDTH      32
#define TTCDAQHeader4_SIZE       1

#define TTCDAQHeader5	        0x4025
#define TTCDAQHeader5_WIDTH      32
#define TTCDAQHeader5_SIZE       1

#define TTCDAQHeader6	        0x4026
#define TTCDAQHeader6_WIDTH      32
#define TTCDAQHeader6_SIZE       1

#define TTCDAQHeader7	        0x4027
#define TTCDAQHeader7_WIDTH      32
#define TTCDAQHeader7_SIZE       1

#define TTCEventInfo	        0x4028
#define TTCEventInfo_WIDTH      18
#define TTCEventInfo_SIZE       1

#define TTCEventError	        0x4029
#define TTCEventError_WIDTH     25
#define TTCEventError_SIZE      1

#define TTCL1MsgHeader	        0x4030
#define TTCL1MsgHeader_WIDTH    12
#define TTCL1MsgHeader_SIZE     1

#define TTCL1MsgData	        0x4031
#define TTCL1MsgData_WIDTH      12
#define TTCL1MsgData_SIZE       1

#define TTCL2aMsgHeader 	0x4035
#define TTCL2aMsgHeader_WIDTH   12
#define TTCL2aMsgHeader_SIZE    1

#define TTCL2aMsgData	        0x4036
#define TTCL2aMsgData_WIDTH     12
#define TTCL2aMsgData_SIZE      1

#define TTCL2rMsgHeader	        0x403D
#define TTCL2rMsgHeader_WIDTH   12
#define TTCL2rMsgHeader_SIZE    1

#define TTCRoIMsgHeader	        0x403E
#define TTCRoIMsgHeader_WIDTH   12
#define TTCRoIMsgHeader_SIZE    1

#define TTCRoIMsgData	        0x403F
#define TTCRoIMsgData_WIDTH     12
#define TTCRoIMsgData_SIZE      1

#define TTCFIFODAQHeader	0x4081
#define TTCFIFODAQHeader_WIDTH  32
#define TTCFIFODAQHeader_SIZE   1

#endif //__CALIBRATIONPULSER_HPP

