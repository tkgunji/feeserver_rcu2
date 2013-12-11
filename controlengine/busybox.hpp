#ifndef __BUSYBOX_HPP
#define __BUSYBOX_HPP

#include <vector>
#include "ctr.hpp"
#include "issuehandler.hpp"
#include "dev.hpp"
#include "ctr.hpp"

/******************************************************************************/

class CtrBb: public Ctr{
public:
  CtrBb();
  virtual ~CtrBb();
protected:
  //BB sub device
  Dev* fpBb;
};

/******************************************************************************/

class DevMsgbuffer;

class DevBb: public Dev{
public:
  DevBb(int id, CEStateMachine* pParent, DevMsgbuffer* msgbuffer);
  virtual int issue(uint32_t cmd, uint32_t parameter, const char* pData, int iDataSize, std::vector<uint32_t>& rb);
  virtual int HighLevelHandler(const char* pCommand, std::vector<uint32_t>& rb);
  static int updateServiceStatus(Ser::TceServiceData* pData, int major, int minor, void* parameter);
protected:
  virtual int ReSynchronizeLocal();
  virtual int WriteReg(uint32_t cmd,  uint32_t address, uint32_t value, int iDataSize);
  virtual int ArmorLocal();
  virtual int GetGroupId();
  uint32_t fFeeServerBbFpgas;
  DevMsgbuffer* fpMsgbuffer;
};

/******************************************************************************/

/*****************************************
 **** Busybox fw registers          ******
 *****************************************/

#define FPGAOffset             0x8000

#define BbTXmodule             0x0001
#define BbTXmodule_WIDTH       16
#define BbTXmodule_SIZE        1

#define BbRXmemp               0x2000
#define BbRXmemp_WIDTH         14
#define BbRXmemp_SIZE          1

#define BbEventFIFOCNT         0x2001
#define BbEventFIFOCNT_WIDTH   4
#define BbEVENTFIFOCNT_SIZE    1

#define BbCurEventID1          0x2002
#define BbCurEventID1_WIDTH    3
#define BbCurEventID1_SIZE     1

#define BbCurEventID2          0x2003
#define BbCurEventID2_WIDTH    16
#define BbCurEventID2_SIZE     1

#define BbCurEventID3          0x2004
#define BbCurEventID3_WIDTH    16
#define BbCurEventID3_SIZE     1

#define BbNewEventID1          0x2005
#define BbNewEventID1_WIDTH    3
#define BbNewEventID1_SIZE     1

#define BbNewEventID2          0x2006
#define BbNewEventID2_WIDTH    16
#define BbNewEventID2_SIZE     1

#define BbNewEventID3          0x2007
#define BbNewEventID3_WIDTH    16
#define BbNewEventID3_SIZE     1

#define BbL0TTO                0x2008
#define BbL0TTO_WIDTH          16
#define BbL0TTO_SIZE           1

#define BbBusyCond             0x2009
#define BbBusyCond_WIDTH       16
#define BbBusyCond_SIZE        1

#define BbHaltFSMmatch         0x200A
#define BbHaltFSMmatch_WIDTH   1
#define BbHaltFSMmatch_SIZE    1

#define BbForcematch           0x200B
#define BbForcematch_WIDTH     1
#define BbForcematch_SIZE      1

#define BbReReqTimeout         0x200C
#define BbReReqTimeout_WIDTH   16
#define BbReReqTimeout_SIZE    1

#define BbCurRequestID         0x200D
#define BbCurRequestID_WIDTH   4
#define BbCurRequestID_SIZE    1

#define BbRetryCount           0x200E
#define BbRetryCount_WIDTH     16
#define BbRetryCount_SIZE      1

#define BbBusytimeH            0x2010
#define BbBusytimeH_WIDTH      16
#define BbBusytimeH_SIZE       1

#define BbBusytimeL            0x2011
#define BbBusytimeL_WIDTH      16
#define BbBusytimeL_SIZE       1

#define BbRXmemfilter          0x2012
#define BbRXmemfilter_WIDTH    16
#define BbRXmemfilter_SIZE     1

#define BbNumOfChan            0x2014
#define BbNumOfChan_WIDTH      16
#define BbNumOfChan_SIZE       1

#define BbFwVersion            0x2015
#define BbFwVersion_WIDTH      16
#define BbFwVersion_SIZE       1

#define BbStressEn             0x2016
#define BbStressEn_WIDTH       1
#define BbStressEn_SIZE        1

#define BbBurstSize            0x2017
#define BbBurstSize_WIDTH      6
#define BbBurstSize_SIZE       1

#define BbBurstIntLTime        0x2018
#define BbBurstIntLTime_WIDTH  16
#define BbBurstIntLTime_SIZE   1

#define BbTrigMode             0x2019
#define BbTrigMode_WIDTH       3
#define BbTrigMode_SIZE        1

#define BbCurTrigEvtInf        0x2050
#define BbCurTrigEvtInf_WIDTH  13
#define BbCurTrigEvtInf_SIZE   1

#define BbCurTrigEvtErr1       0x2051
#define BbCurTrigEvtErr1_WIDTH 9
#define BbCurTrigEvtErr1_SIZE  1

#define BbCurTrigEvtErr2       0x2052
#define BbCurTrigEvtErr2_WIDTH 16
#define BbCurTrigEvtErr2_SIZE  1

#define BbNewTrigEvtInf        0x2054
#define BbNewTrigEvtInf_WIDTH  13
#define BbNewTrigEvtInf_SIZE   1

#define BbNewTrigEvtErr1       0x2055
#define BbNewTrigEvtErr1_WIDTH 9
#define BbNewTrigEvtErr1_SIZE  1

#define BbNewTrigEvtErr2       0x2056
#define BbNewTrigEvtErr2_WIDTH 16
#define BbNewTrigEvtErr2_SIZE  1

/*****************************************
 **** Trigger Interface registers   ******
 *****************************************/


// Control register1
#define BbTrmCTR1          0x3000
#define BbTrmCTR1_WIDTH    16
#define BbTrmCTR1_SIZE     1
#define BbTrmCTR1_MSK_SERB 0x0001 // SerialB enable
#define BbTrmCTR1_MSK_DEM  0x0002 // Disable error masking
#define BbTrmCTR1_MSK_ROI  0x0004 // Region of Interest enable
#define BbTrmCTR1_MSK_L0   0x0008 // L0 support enable
#define BbTrmCTR1_MSK_L2A  0x0100 // Level 2 Accept storage mask
#define BbTrmCTR1_MSK_L2R  0x0200 // Level 2 Reject storage mask
#define BbTrmCTR1_MSK_L2TO 0x0400 // Level 2 Timeout storage mask
#define BbTrmCTR1_MSK_L1M  0x0800 // Level 1 Message mask
#define BbTrmCTR1_MSK_IM   0x1000 // Input mask

// Control register2
#define BbTrmCTR2          0x3001
#define BbTrmCTR2_WIDTH    16
#define BbTrmCTR2_SIZE     1
#define BbTrmCTR2_MSK_BCOF 0x0001 // BunchCounter overflow
#define BbTrmCTR2_MSK_RA   0x0002 // Run Active
#define BbTrmCTR2_MSK_BUSY 0x0004 // Busy Receiving message
#define BbTrmCTR2_MSK_CDHV 0x00F0 // CDH Version
#define BbTrmCTR2_MSK_TRMV 0xFF00 // Trigger Receiver Module Version

// Reset trigger module
#define BbTrmMRST          0x3002
#define BbTrmMRST_WIDTH    1
#define BbTrmMRST_SIZE     1
#define BbTrmMRST_MSK      0x0001

// Reset all counters
#define BbTrmCNTRST        0x3008
#define BbTrmCNTRST_WIDTH  1
#define BbTrmCNTRST_SIZE   1
#define BbTrmCNTRST_MSK    0x0001

// Issue testmode
#define BbTrmTSTM          0x300A
#define BbTrmTSTM_WIDTH    1
#define BbTrmTSTM_SIZE     1
#define BbTrmTSTM_MSK      0x0001

// L1 Latency
#define BbTrmL1LAT          0x300C
#define BbTrmL1LAT_WIDTH    16
#define BbTrmL1LAT_SIZE     1
#define BbTrmL1LAT_MSK_DLT  0xF000 // delta, uncertainty region
#define BbTrmL1LAT_MSK_LAT  0x0FFF // Latency (25 ns cycles)

// L2 max latency
#define BbTrmL2LATMAX          0x300E
#define BbTrmL2LATMAX_WIDTH    16
#define BbTrmL2LATMAX_SIZE     1

// L2 min latency
#define BbTrmL2LATMIN          0x300F
#define BbTrmL2LATMIN_WIDTH    16
#define BbTrmL2LATMIN_SIZE     1

// L1 message max latency
#define BbTrmL1LATMAX          0x3014
#define BbTrmL1LATMAX_WIDTH    16
#define BbTrmL1LATMAX_SIZE     1

// L1 message min latency
#define BbTrmL1LATMIN          0x3015
#define BbTrmL1LATMIN_WIDTH    16
#define BbTrmL1LATMIN_SIZE     1

// Pre-pulse counter
#define BbTrmPPCNT         0x3016
#define BbTrmPPCNT_WIDTH   1
#define BbTrmPPCNT_SIZE    1

// BCID local
#define BbTrmBCIDL         0x3018
#define BbTrmBCIDL_WIDTH   12
#define BbTrmBCIDL_SIZE    1
#define BbTrmBCIDL_MSK     0x0FFF

// L0 Counter
#define BbTrmL0CNT         0x301A
#define BbTrmL0CNT_WIDTH   16
#define BbTrmL0CNT_SIZE    1

// L1 Counter
#define BbTrmL1CNT         0x301C
#define BbTrmL1CNT_WIDTH   16
#define BbTrmL1CNT_SIZE    1

// L1 Message Counter
#define BbTrmL1MCNT        0x301E
#define BbTrmL1MCNT_WIDTH  16
#define BbTrmL1MCNT_SIZE   1

// L2 Accept Counter
#define BbTrmL2ACNT        0x3020
#define BbTrmL2ACNT_WIDTH  16
#define BbTrmL2ACNT_SIZE   1

// L2 Reject Counter
#define BbTrmL2RCNT        0x3022
#define BbTrmL2RCNT_WIDTH  16
#define BbTrmL2RCNT_SIZE   1

// BunchCounter
#define BbTrmBCNT          0x3026
#define BbTrmBCNT_WIDTH    11
#define BbTrmBCNT_SIZE     1

// Single Hamming error count (SHE)
#define BbTrmSHECNT        0x302C
#define BbTrmSHECNT_WIDTH  16
#define BbTrmSHECNT_SIZE   1

// Double Hamming error count (DHE)
#define BbTrmDHECNT        0x302D
#define BbTrmDHECNT_WIDTH  16
#define BbTrmDHECNT_SIZE   1

// Message decoding error count (MDE)
#define BbTrmMDECNT        0x302E
#define BbTrmMDECNT_WIDTH  16
#define BbTrmMDECNT_SIZE   1

// Sequence/Timeout error count (STE)
#define BbTrmSTECNT        0x302F
#define BbTrmSTECNT_WIDTH  16
#define BbTrmSTECNT_SIZE   1

// Buffered events count
#define BbTrmBECNT          0x3040
#define BbTrmBECNT_WIDTH    5
#define BbTrmBECNT_SIZE     1
#define BbTrmBECNT_MSK      0x001F

// Event Error Reg 1
#define BbTrmEER1           0x3052
#define BbTrmEER1_WIDTH     16
#define BbTrmEER1_SIZE      1

// Event Error Reg1 Masks
#define BbTrmEER1_MSK_SBE   0x0001 //( 0) Stop Bit Error
#define BbTrmEER1_MSK_SHEA  0x0002 //( 1) Single Hamming Error - Addressed Message
#define BbTrmEER1_MSK_DHEA  0x0004 //( 2) Double Hamming Error - Addressed Message
#define BbTrmEER1_MSK_SHEB  0x0008 //( 3) Single Hamming Error - Broadcast Message
#define BbTrmEER1_MSK_DHEB  0x0010 //( 4) Double Hamming Error - Broadcast Message
#define BbTrmEER1_MSK_UA    0x0020 //( 5) Unknown TTC Address
#define BbTrmEER1_MSK_IL1M  0x0040 //( 6) Incomplete Level 1 Message
#define BbTrmEER1_MSK_IL2M  0x0080 //( 7) Incomplete Level 2 Message
// Not used                   0x0100   ( 8)
#define BbTrmEER1_MSK_TTCAE 0x0200 //( 9) TTCrx Address Error
#define BbTrmEER1_MSK_SL0   0x0400 //(10) Spurious L0
#define BbTrmEER1_MSK_ML0   0x0800 //(11) Missing L0
#define BbTrmEER1_MSK_SL1   0x1000 //(12) Spurious L1
#define BbTrmEER1_MSK_BL1   0x2000 //(13) Boundary L1
#define BbTrmEER1_MSK_ML1   0x4000 //(14) Missing L1
#define BbTrmEER1_MSK_EL1   0x4000 //(15) Erronous L1 - outside legal timeslot

// Event Error Reg 2
#define BbTrmEER2           0x3053
#define BbTrmEER2_WIDTH     9
#define BbTrmEER2_SIZE      1

// Event Error Reg2 Masks
#define BbTrmEER2_MSK_L1MTO 0x0001 //( 0) L1 Message missing/timed out
#define BbTrmEER2_MSK_L2TE  0x0002 //( 1) L2 message outside valid time slot (Timing Error)
#define BbTrmEER2_MSK_L2MTO 0x0004 //( 2) L2 message missing / timeout
// Not Used                   0x0008 //( 3) 
// Not Used                   0x0010 //( 4)
#define BbTrmEER2_MSK_PPE   0x0020 //( 5) Pre-pulse Error
#define BbTrmEER2_MSK_L1MCE 0x0040 //( 6) L1 message Content Error
#define BbTrmEER2_MSK_L2MCE 0x0080 //( 7) L2 message Content Error

#define BbTrmDAQHEADER_01_L         0x3042
#define BbTrmDAQHEADER_01_L_WIDTH   16
#define BbTrmDAQHEADER_01_L_SIZE    1

#define BbTrmDAQHEADER_01_H         0x3043
#define BbTrmDAQHEADER_01_H_WIDTH   16
#define BbTrmDAQHEADER_01_H_SIZE    1

#define BbTrmDAQHEADER_02_L         0x3044
#define BbTrmDAQHEADER_02_L_WIDTH   16
#define BbTrmDAQHEADER_02_L_SIZE    1

#define BbTrmDAQHEADER_02_H         0x3045
#define BbTrmDAQHEADER_02_H_WIDTH   16
#define BbTrmDAQHEADER_02_H_SIZE    1

#define BbTrmDAQHEADER_03_L         0x3046
#define BbTrmDAQHEADER_03_L_WIDTH   16
#define BbTrmDAQHEADER_03_L_SIZE    1

#define BbTrmDAQHEADER_03_H         0x3047
#define BbTrmDAQHEADER_03_H_WIDTH   16
#define BbTrmDAQHEADER_03_H_SIZE    1

#define BbTrmDAQHEADER_04_L         0x3048
#define BbTrmDAQHEADER_04_L_WIDTH   16
#define BbTrmDAQHEADER_04_L_SIZE    1

#define BbTrmDAQHEADER_04_H         0x3049
#define BbTrmDAQHEADER_04_H_WIDTH   16
#define BbTrmDAQHEADER_04_H_SIZE    1

#define BbTrmDAQHEADER_05_L         0x304a
#define BbTrmDAQHEADER_05_L_WIDTH   16
#define BbTrmDAQHEADER_05_L_SIZE    1

#define BbTrmDAQHEADER_05_H         0x304b
#define BbTrmDAQHEADER_05_H_WIDTH   16
#define BbTrmDAQHEADER_05_H_SIZE    1

#define BbTrmDAQHEADER_06_L         0x304c
#define BbTrmDAQHEADER_06_L_WIDTH   16
#define BbTrmDAQHEADER_06_L_SIZE    1

#define BbTrmDAQHEADER_06_H         0x304d
#define BbTrmDAQHEADER_06_H_WIDTH   16
#define BbTrmDAQHEADER_06_H_SIZE    1

#define BbTrmDAQHEADER_07_L         0x304e
#define BbTrmDAQHEADER_07_L_WIDTH   16
#define BbTrmDAQHEADER_07_L_SIZE    1

#define BbTrmDAQHEADER_07_H         0x304f
#define BbTrmDAQHEADER_07_H_WIDTH   16
#define BbTrmDAQHEADER_07_H_SIZE    1

#define BbTrmEVENTINFO              0x3050
#define BbTrmEVENTINFO_WIDTH        13
#define BbTrmEVENTINFO_SIZE         1

#endif //__BUSYBOX_HPP

