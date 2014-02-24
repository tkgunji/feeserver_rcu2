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

#ifndef __GATEPULSER_HPP
#define __GATEPULSER_HPP

#include <vector>
#include <csignal>
#include "ctr.hpp"
#include "issuehandler.hpp"
#include "dev.hpp"

/******************************************************************************/

class CtrGpulser: public Ctr{
public:
  CtrGpulser();
  virtual ~CtrGpulser();
  static void InterruptHandler(int sig);
protected:
  //Gpulser sub device
  Dev* fpGpulser;
  int fp;
};

/******************************************************************************/

class DevMsgbuffer;

class DevGpulser: public Dev{

public:

  DevGpulser(int id, CEStateMachine* pParent, DevMsgbuffer* msgbuffer);

  virtual int issue(uint32_t cmd, uint32_t parameter, const char* pData, int iDataSize, std::vector<uint32_t>& rb);

  virtual int HighLevelHandler(const char* pCommand, std::vector<uint32_t>& rb);

protected:

  virtual int SynchronizeLocal();

  virtual int ReSynchronizeLocal();

  virtual int EnterStatePULSER_L0();

  virtual int EnterStatePULSER_L1();

  virtual int LeaveStatePULSER_L1();

  virtual int EnterStatePULSER_OFF();

  virtual int LeaveStatePULSER_OFF();

  virtual int ArmorLocal();

  virtual int GetGroupId();

  virtual int WriteDivRatio(uint32_t parameter);

  virtual int WriteMode(uint32_t parameter);

  virtual int WritePulseWidth(uint32_t parameter);

  DevMsgbuffer* fpMsgbuffer;
};

/******************************************************************************/
#define BASE_FPGA_FABRIC_FIC1   0x50000000


/*************************************
 **** Configuration Registers   ******
 ************************************/

#define ConfReg                0x0001
#define ConfReg_WIDTH          17
#define ConfReg_SIZE           1

#define ConfRegMaskEnable      0x10000
#define ConfRegMaskClkDivRatio 0xC000
#define ConfRegMaskMode        0x2000
#define ConfRegMaskPulseWidth  0x1FFF


#define PulseStatus            0x0002
#define PulseStatusReg_WIDTH   32
#define PulseStatusReg_SIZE    1

#define PulseCounter           0x00C1
#define PulseCounter_WIDTH     32
#define PulseCounter_SIZE      1

#define MaskCrate1             0x0004
#define MaskCrate1_WIDTH       9
#define MaskCrate1_SIZE        1

#define MaskCrate2             0x0005
#define MaskCrate2_WIDTH       9
#define MaskCrate2_SIZE        1

#define MaskCrate3             0x0006
#define MaskCrate3_WIDTH       9
#define MaskCrate3_SIZE        1

#define MaskCrate4             0x0007
#define MaskCrate4_WIDTH       9
#define MaskCrate4_SIZE        1

#define MaskCrate5             0x0008
#define MaskCrate5_WIDTH       9
#define MaskCrate5_SIZE        1

#define MaskCrate6             0x0009
#define MaskCrate6_WIDTH       9
#define MaskCrate6_SIZE        1

#define OpenFrameCrate1        0x000A
#define OpenFrameCrate1_WIDTH  24
#define OpenFrameCrate1_SIZE   1

#define OpenFrameCrate2        0x000B
#define OpenFrameCrate2_WIDTH  24
#define OpenFrameCrate2_SIZE   1

#define OpenFrameCrate3        0x000C
#define OpenFrameCrate3_WIDTH  24
#define OpenFrameCrate3_SIZE   1

#define OpenFrameCrate4        0x000D
#define OpenFrameCrate4_WIDTH  24
#define OpenFrameCrate4_SIZE   1

#define OpenFrameCrate5        0x000E
#define OpenFrameCrate5_WIDTH  24
#define OpenFrameCrate5_SIZE   1

#define OpenFrameCrate6        0x000F
#define OpenFrameCrate6_WIDTH  24
#define OpenFrameCrate6_SIZE   1

#define CloseFrameCrate1       0x001A
#define CloseFrameCrate1_WIDTH 24
#define CloseFrameCrate1_SIZE  1

#define CloseFrameCrate2       0x001B
#define CloseFrameCrate2_WIDTH 24
#define CloseFrameCrate2_SIZE  1

#define CloseFrameCrate3       0x001C
#define CloseFrameCrate3_WIDTH 24
#define CloseFrameCrate3_SIZE  1

#define CloseFrameCrate4       0x001D
#define CloseFrameCrate4_WIDTH 24
#define CloseFrameCrate4_SIZE  1

#define CloseFrameCrate5       0x001E
#define CloseFrameCrate5_WIDTH 24
#define CloseFrameCrate5_SIZE  1

#define CloseFrameCrate6       0x001F
#define CloseFrameCrate6_WIDTH 24
#define CloseFrameCrate6_SIZE  1

#define ErrorMonitor           0x002F
#define ErrorMonitor_WIDTH     12
#define ErrorMonitor_SiZE      1

#define RCUVersion             0x0020
#define RCUVersion_WIDTH       32
#define RCUVersion_SIZE        1

#define RestTestReg            (0x5301 | BASE_FPGA_FABRIC_FIC1)
#define RestTestReg_WIDTH      32
#define RestTestReg_SIZE       1

#define GlobalRest             (0x5300 | BASE_FPGA_FABRIC_FIC1)
#define GlobalRest_WIDTH       32
#define GlobalRest_SIZE        1

#define ARM_SYNCH              (0x530A | BASE_FPGA_FABRIC_FIC1)
#define ARM_SYNCH_WIDTH        32
#define ARM_SYNCH_SIZE         1

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

#endif //__GATEPULSER_HPP

