#ifndef __TRIGGEROR_HPP
#define __TRIGGEROR_HPP

#include <vector>
#include "ctr.hpp"
#include "issuehandler.hpp"
#include "dev.hpp"

/******************************************************************************/

class CtrTor: public Ctr{
public:
  CtrTor();
  virtual  ~CtrTor();
protected:
  //BB sub device
  Dev* fpTor;
};

/******************************************************************************/

class DevMsgbuffer;

class DevTor: public Dev{
public:
  DevTor(int id,  CEStateMachine* pParent, DevMsgbuffer* msgbuffer);
  virtual int issue(uint32_t cmd, uint32_t parameter, const char* pData, int iDataSize, std::vector<uint32_t>& rb);
  virtual int HighLevelHandler(const char* pCommand, std::vector<uint32_t>& rb);
protected:
  virtual int ReSynchronizeLocal();
  virtual int ArmorLocal();
  virtual int GetGroupId();
  DevMsgbuffer* fpMsgbuffer;
};

/******************************************************************************/

/*************************************
 **** Configuration Registers   ******
 ************************************/

#define Trig0_OptionCode		0x00
#define Trig0_OptionCode_WIDTH		32
#define Trig0_OptionCode_SIZE		1

#define Trig0_Signature			0x01
#define Trig0_Signature_WIDTH		32
#define Trig0_Signature_SIZE		1

#define Trig0_MessageHeader		0x02
#define Trig0_MessageHeader_WIDTH	32
#define Trig0_MessageHeader_SIZE	1

#define Trig0_ProgRateLow		0x03
#define Trig0_ProgRateLow_WIDTH		32
#define Trig0_ProgRateLow_SIZE		1

#define Trig0_ProgRateHigh		0x04
#define Trig0_ProgRateHigh_WIDTH	32
#define Trig0_ProgRateHigh_SIZE		1

#define Trig0_ProgDelay			0x05
#define Trig0_ProgDelay_WIDTH		32
#define Trig0_ProgDelay_SIZE		1

#define Trig1_OptionCode		0x06
#define Trig1_OptionCode_WIDTH		32
#define Trig1_OptionCode_SIZE		1

#define Trig1_Signature			0x07
#define Trig1_Signature_WIDTH		32
#define Trig1_Signature_SIZE		1

#define Trig1_MessageHeader		0x08
#define Trig1_MessageHeader_WIDTH	32
#define Trig1_MessageHeader_SIZE	1

#define Trig1_ProgRateLow		0x09
#define Trig1_ProgRateLow_WIDTH		32
#define Trig1_ProgRateLow_SIZE		1

#define Trig1_ProgRateHigh		0x0A
#define Trig1_ProgRateHigh_WIDTH	32
#define Trig1_ProgRateHigh_SIZE		1

#define Trig1_ProgDelay			0x0B
#define Trig1_ProgDelay_WIDTH		32
#define Trig1_ProgDelay_SIZE		1

#define Trig1M_OptionCode		0x0C
#define Trig1M_OptionCode_WIDTH		32
#define Trig1M_OptionCode_SIZE		1

#define Trig1M_Signature		0x0D
#define Trig1M_Signature_WIDTH		32
#define Trig1M_Signature_SIZE		1

#define Trig1M_MessageHeader		0x0E
#define Trig1M_MessageHeader_WIDTH	32
#define Trig1M_MessageHeader_SIZE	1

#define Trig1M_ProgRateLow		0x0F
#define Trig1M_ProgRateLow_WIDTH	32
#define Trig1M_ProgRateLow_SIZE		1

#define Trig1M_ProgRateHigh		0x10
#define Trig1M_ProgRateHigh_WIDTH	32
#define Trig1M_ProgRateHigh_SIZE	1

#define Trig1M_ProgDelay		0x11
#define Trig1M_ProgDelay_WIDTH		32
#define Trig1M_ProgDelay_SIZE		1

#define Trig1H_OptionCode		0x12
#define Trig1H_OptionCode_WIDTH		32
#define Trig1H_OptionCode_SIZE		1

#define Trig1H_Signature		0x13
#define Trig1H_Signature_WIDTH		32
#define Trig1H_Signature_SIZE		1

#define Trig1H_MessageHeader		0x14
#define Trig1H_MessageHeader_WIDTH	32
#define Trig1H_MessageHeader_SIZE	1

#define Trig1H_ProgRateLow		0x15
#define Trig1H_ProgRateLow_WIDTH	32
#define Trig1H_ProgRateLow_SIZE		1

#define Trig1H_ProgRateHigh		0x16
#define Trig1H_ProgRateHigh_WIDTH	32
#define Trig1H_ProgRateHigh_SIZE	1

#define Trig1H_ProgDelay		0x17
#define Trig1H_ProgDelay_WIDTH		32
#define Trig1H_ProgDelay_SIZE		1



#define L1L_Threshold			0x18
#define L1L_Threshold_WIDTH		32
#define L1L_Threshold_SIZE		1

#define L1M_Threshold			0x19
#define L1M_Threshold_WIDTH		32
#define L1M_Threshold_SIZE		1

#define L1H_Threshold			0x1A
#define L1H_Threshold_WIDTH		32
#define L1H_Threshold_SIZE		1

#define L0TriggerMask_12		0x1B
#define L0TriggerMask_12_WIDTH		32
#define L0TriggerMask_12_SIZE		1

#define L0TriggerMask_34		0x1C
#define L0TriggerMask_34_WIDTH		32
#define L0TriggerMask_34_SIZE		1

#define L0TriggerMask_5			0x1D
#define L0TriggerMask_5_WIDTH		32
#define L0TriggerMask_5_SIZE		1

#define CtrlReserve_r			0x1E
#define CtrlReserver_r_WIDTH		32
#define CtrlReserve_r_SIZE		1

/**
Counters and Informationregisters
**/

#define L0LowCounter                     0x1F
#define L0LowCounter_WIDTH	         16
#define L0LowCounter_SIZE		 1

#define L0HighCounter                    0x20
#define L0HighCounter_WIDTH	         16
#define L0HighCounter_SIZE		 1

#define L1LLowCounter                    0x21
#define L1LLowCounter_WIDTH	         16
#define L1LLowCounter_SIZE		 1

#define L1LHighCounter                   0x22
#define L1LHighCounter_WIDTH	         16
#define L1LHighCounter_SIZE		 1

#define L1MLowCounter                    0x23
#define L1MLowCounter_WIDTH	         16
#define L1MLowCounter_SIZE	 	 1

#define L1MHighCounter                   0x24
#define L1MHighCounter_WIDTH	         16
#define L1MHighCounter_SIZE	 	 1

#define L1HLowCounter                    0x25
#define L1HLowCounter_WIDTH	         16
#define L1HLowCounter_SIZE		 1

#define L1HHighCounter                   0x26
#define L1HHighCounter_WIDTH	         16
#define L1HHighCounter_SIZE		 1

#define FirmwareVersion                  0x27
#define FirmwareVersion_WIDTH	         16
#define FirmwareVersion_SIZE		 1

#define M11InpTrigCount                  0x28
#define M11InpTrigCount_WIDTH	         16
#define M11InpTrigCount_SIZE		 1

#define M12InpTrigCount                  0x29
#define M12InpTrigCount_WIDTH	         16
#define M12InpTrigCount_SIZE		 1

#define M13InpTrigCount                  0x2A
#define M13InpTrigCount_WIDTH	         16
#define M13InpTrigCount_SIZE		 1

#define M14InpTrigCount                  0x2B
#define M14InpTrigCount_WIDTH	         16
#define M14InpTrigCount_SIZE		 1

#define M15InpTrigCount                  0x2C
#define M15InpTrigCount_WIDTH	         16
#define M15InpTrigCount_SIZE		 1

#define M16InpTrigCount                  0x2D
#define M16InpTrigCount_WIDTH	         16
#define M16InpTrigCount_SIZE		 1

#define M17InpTrigCount                  0x2E
#define M17InpTrigCount_WIDTH	         16
#define M17InpTrigCount_SIZE		 1

#define M18InpTrigCount                  0x2F
#define M18InpTrigCount_WIDTH	         16
#define M18InpTrigCount_SIZE		 1

#define M21InpTrigCount                  0x30
#define M21InpTrigCount_WIDTH	         16
#define M21InpTrigCount_SIZE		 1

#define M22InpTrigCount                  0x31
#define M22InpTrigCount_WIDTH	         16
#define M22InpTrigCount_SIZE		 1

#define M23InpTrigCount                  0x32
#define M23InpTrigCount_WIDTH	         16
#define M23InpTrigCount_SIZE		 1

#define M24InpTrigCount                  0x33
#define M24InpTrigCount_WIDTH	         16
#define M24InpTrigCount_SIZE		 1

#define M25InpTrigCount                  0x34
#define M25InpTrigCount_WIDTH	         16
#define M25InpTrigCount_SIZE		 1

#define M26InpTrigCount                  0x35
#define M26InpTrigCount_WIDTH	         16
#define M26InpTrigCount_SIZE		 1

#define M27InpTrigCount                  0x36
#define M27InpTrigCount_WIDTH	         16
#define M27InpTrigCount_SIZE		 1

#define M28InpTrigCount                  0x37
#define M28InpTrigCount_WIDTH	         16
#define M28InpTrigCount_SIZE		 1

#define M31InpTrigCount                  0x38
#define M31InpTrigCount_WIDTH	         16
#define M31InpTrigCount_SIZE		 1

#define M32InpTrigCount                  0x39
#define M32InpTrigCount_WIDTH	         16
#define M32InpTrigCount_SIZE		 1

#define M33InpTrigCount                  0x3A
#define M33InpTrigCount_WIDTH	         16
#define M33InpTrigCount_SIZE		 1

#define M34InpTrigCount                  0x3B
#define M34InpTrigCount_WIDTH	         16
#define M34InpTrigCount_SIZE		 1

#define M35InpTrigCount                  0x3C
#define M35InpTrigCount_WIDTH	         16
#define M35InpTrigCount_SIZE		 1

#define M36InpTrigCount                  0x3D
#define M36InpTrigCount_WIDTH	         16
#define M36InpTrigCount_SIZE		 1

#define M37InpTrigCount                  0x3E
#define M37InpTrigCount_WIDTH	         16
#define M37InpTrigCount_SIZE		 1

#define M38InpTrigCount                  0x3F
#define M38InpTrigCount_WIDTH	         16
#define M38InpTrigCount_SIZE		 1

#define DbgRdout4Low                     0x40
#define DbgRdout4Low_WIDTH	         16
#define DbgRdout4Low_SIZE		 1

#define DbgRdout4High                    0x41
#define DbgRdout4High_WIDTH	         16
#define DbgRdout4High_SIZE		 1

#define MaxPhos32                        0x42
#define MaxPhos32_WIDTH	                 16
#define MaxPhos32_SIZE		         1

#define MaxPhos3                         0x43
#define MaxPhos3_WIDTH	                 16
#define MaxPhos3_SIZE		         1

#define MaxPhos123                       0x44
#define MaxPhos123_WIDTH	         16
#define MaxPhos123_SIZE		         1
 
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

#endif //__TRIGGEROR_HPP

