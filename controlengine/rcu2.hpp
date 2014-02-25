#ifndef __RCU2_HPP
#define __RCU2_HPP

#include <vector>
#include "ctr.hpp"
#include "dev.hpp"
#include "fec.hpp"

/******************************************************************************/

#define RCU2_FECActiveList       0x8000
#define RCU2_FECActiveList_WIDTH 32
#define RCU2_FECActiveList_SIZE  1

#define RCU2StatusReg 0x0000

/******************************************************************************/

class DevRcu2: public Dev{
public:
  DevRcu2(int id,  CEStateMachine* pParent, DevMsgbuffer* msgbuffer);
  virtual int GetGroupId();
  virtual int HighLevelHandler(const char* pCommand, std::vector<uint32_t>& rb);
  virtual int issue(uint32_t cmd, uint32_t parameter, const char* pData, int iDataSize, std::vector<uint32_t>& rb);
  virtual int SetAfl(uint32_t value);
  virtual int Rcu2AdcConf(uint32_t adc);
  static  int Rcu2AdcRead(Ser::TceServiceData* pData, int32_t adc, int32_t unused, void* parameter);
  virtual int Rcu2RadMonConf(uint32_t adc);
  static  int Rcu2RadMonRead(Ser::TceServiceData* pData, int32_t adc, int32_t unused, void* parameter);

protected:
  virtual int ArmorLocal();
  virtual int ReSynchronizeLocal();
  DevMsgbuffer* fpMsgbuffer;
  std::map<uint32_t,uint32_t> fAltroInstr;
  std::map<uint32_t,uint32_t> fRegCfg;
  static std::string imemFileName;
  static std::string regCfgFileName;

};


/******************************************************************************/

class CtrRcu2: public Ctr{
public:
  CtrRcu2();
  virtual ~CtrRcu2();
  static int CtrRcu2GO_IDLE(CEStateMachine* callbackObject);
  //RCU2 sub device
  DevRcu2* fpRcu2;
protected:
  /** The FEC access */
  DevFecaccess* fpFecaccess;
  /** The FECs */
  DevFec* fpFecs[32];
};

/******************************************************************************/

//Class for access via rcu2
class DevFecaccessRcu2: public DevFecaccess{
public:
  DevFecaccessRcu2(DevMsgbuffer* msgbuffer);
  virtual int AccessFecReg(uint32_t &data, uint32_t fec, uint32_t reg, uint32_t rnw);
};

/******************************************************************************/

class DevMsgbufferRcu2: public DevMsgbuffer {
public:
  DevMsgbufferRcu2();
  virtual ~DevMsgbufferRcu2();
  virtual int SingleWrite(uint32_t address, uint32_t data, uint32_t mode=1);
  virtual int SingleRead(uint32_t address, uint32_t* pData, uint32_t mode=1);
  virtual int MultipleWrite(uint32_t address, uint32_t* pData, int iSize, int iDataSize=4, uint32_t mode=1);
  virtual int MultipleRead(uint32_t address, int iSize,uint32_t* pData, uint32_t mode=1);
  virtual int FlashErase(int startSec, int stopSec);
  virtual int DriverReload();
  virtual int DriverUnload();
  virtual int DriverLoad();
};


/*************************************                                                                
   DCS register inferface 
*************************************/
#define BASE_FPGA_FABRIC_FIC0   0x30000000
#define BASE_FPGA_FABRIC_FIC1   0x50000000
#define BASE_SPI_0              0x40001000
#define BASE_I2C_0              0x40002000
#define BASE_RADMON_JTAG        0x40012000

/*************************************                                                                
   RCU FW2 Registers and Memories                                                                  
*************************************/

/** Thoese lines are copy from rcu **/
#define V2_ALTROInstMEM          0x0000    // 22bit x 4096
#define V2_ALTROInstMEM_WIDTH    22
#define V2_ALTROInstMEM_SIZE     4096

#define V2_ALTROPatternMEM       0x0000    // 10bit x 1024
#define V2_ALTROPatternMEM_WIDTH 22
#define V2_ALTROPatternMEM_SIZE  4096

#define V2_ALTROACL              0x1000    // 32bit x 128
#define V2_ALTROACL_WIDTH        12
#define V2_ALTROACL_SIZE         4096

#define V2_ALTROResultMEM        0x2000    // 25bit x 8192
#define V2_ALTROResultMEM_WIDTH  25
#define V2_ALTROResultMEM_SIZE   8192

//Hit List Memory (Active Channel List from Board Controllers)
#define V2_ALTROHlMEM            0x5000    // 32bit x 128
#define V2_ALTROHlMEM_WIDTH      32
#define V2_ALTROHlMEM_SIZE       128

#define V2_FECActiveList         (0x5100 | BASE_FPGA_FABRIC_FIC1)
#define V2_FECActiveList_WIDTH   32
#define V2_FECActiveList_SIZE    1

#define V2_FECRDOList            (0x5100 | BASE_FPGA_FABRIC_FIC1)
#define V2_FECRDOList_WIDTH      32
#define V2_FECRDOList_SIZE       1

#define V2_FECActiveList_RO      (0x8000 | BASE_FPGA_FABRIC_FIC0)
#define V2_FECActiveList_RO_WIDTH 32
#define V2_FECActiveList_RO_SIZE  1

#define V2_FECRDOList_RO         (0x8000 | BASE_FPGA_FABRIC_FIC0)
#define V2_FECRDOList_RO_WIDTH   32
#define V2_FECRDOList_RO_SIZE    1


//rcufw2 FECERR_A and FECERR_B registers
//#define V2_FECErrA               0x5110 
//#define V2_FECErrA_WIDTH         32
//#define V2_FECErrA_SIZE          1

//#define V2_FECErrB               0x5111
//#define V2_FECErrB_WIDTH         32
//#define V2_FECErrB_SIZE          1

//#define V2_FECErrA_RO            0x800A
//#define V2_FECErrA_RO_WIDTH      32
//#define V2_FECErrA_RO_SIZE       1

//#define V2_FECErrB_RO            0x800B
//#define V2_FECErrB_RO_WIDTH      32
//#define V2_FECErrB_RO_SIZE       1

#define V2_FECErrAI_RO            (0x8006 | BASE_FPGA_FABRIC_FIC0)
#define V2_FECErrAI_RO_WIDTH      21     
#define V2_FECErrAI_RO_SIZE       1       

#define V2_FECErrAO_RO            (0x8007 | BASE_FPGA_FABRIC_FIC0)
#define V2_FECErrAO_RO_WIDTH      21     
#define V2_FECErrAO_RO_SIZE       1       

#define V2_FECErrBI_RO            (0x8008 | BASE_FPGA_FABRIC_FIC0)
#define V2_FECErrBI_RO_WIDTH      21     
#define V2_FECErrBI_RO_SIZE       1       

#define V2_FECErrBO_RO            (0x8009 | BASE_FPGA_FABRIC_FIC0)
#define V2_FECErrBO_RO_WIDTH      21     
#define V2_FECErrBO_RO_SIZE       1       


#define V2_AltroIf               (0x5101 | BASE_FPGA_FABRIC_FIC1)
#define V2_AltroIf_WIDTH         16
#define V2_AltroIf_SIZE          1

#define V2_AltroTrCfg            (0x5102 | BASE_FPGA_FABRIC_FIC1)
#define V2_AltroTrCfg_WIDTH      16
#define V2_AltroTrCfg_SIZE       1

#define V2_RDOMod                (0x5103 | BASE_FPGA_FABRIC_FIC1)
#define V2_RDOMod_WIDTH          4
#define V2_RDOMod_SIZE           1

#define V2_AltroCfg1             (0x5104 | BASE_FPGA_FABRIC_FIC1)
#define V2_AltroCfg1_WIDTH       16
#define V2_AltroCfg1_SIZE        1

#define V2_AltroCfg2             (0x5105 | BASE_FPGA_FABRIC_FIC1)
#define V2_AltroCfg2_WIDTH       16
#define V2_AltroCfg2_SIZE        1

#define V2_RCU_Version           (0x5106 | BASE_FPGA_FABRIC_FIC1)
#define V2_RCU_Version_WIDTH     1
#define V2_RCU_Version_SIZE      1

#define V2_RCU_BPVersion         (0x5107 | BASE_FPGA_FABRIC_FIC1)
#define V2_RCU_BPVersion_WIDTH   1
#define V2_RCU_BPVersion_SIZE    1

#define V2_RCUId                 (0x5108 | BASE_FPGA_FABRIC_FIC1)
#define V2_RCUId_WIDTH           9
#define V2_RCUId_SIZE            1

#define V2_BUSBSY                (0x5116 | BASE_FPGA_FABRIC_FIC1)
#define V2_BUSBSY_WIDTH          1
#define V2_BUSBSY_SIZE           1

#define V2_CMDRESET              (0x5300 | BASE_FPGA_FABRIC_FIC1)    // 1bit x 1
//Front End Card reset
#define V2_CMDRESETFEC           (0x5301 | BASE_FPGA_FABRIC_FIC1)    // 1bit x 1
//RCU reset
#define V2_CMDRESETRCU           (0x5302 | BASE_FPGA_FABRIC_FIC1)   // 1bit x 1
#define CONFGFEC                 (0x5303 | BASE_FPGA_FABRIC_FIC1)    // 1bit x 1
#define V2_CMDExecALTRO          (0x5304 | BASE_FPGA_FABRIC_FIC1)   // 1bit x 1
#define V2_CMDAbortALTRO         (0x5305 | BASE_FPGA_FABRIC_FIC1)   // 1bit x 1
#define V2_CMDSwTrig             (0x5306 | BASE_FPGA_FABRIC_FIC1)    // 1bit x 1
#define V2_CMDResAltroErrSt      (0x5307 | BASE_FPGA_FABRIC_FIC1)    // 1bit x 1
#define V2_CMDResRDRXReg         (0x5308 | BASE_FPGA_FABRIC_FIC1)    // 1bit x 1
#define V2_CMDResCounters        (0x5309 | BASE_FPGA_FABRIC_FIC1)    // 1bit x 1
#define V2_CMDDCSBusIRQ          (0x5310 | BASE_FPGA_FABRIC_FIC1)    // 1bit x 1
#define V2_CMDSCLKsync           (0x530a | BASE_FPGA_FABRIC_FIC1)   // 1bit x 1


/*************************************
   RCU FW1 Registers and Memories
*************************************/

// lsbs of memory locations with a smaller bit width than the system are ignored 
#define ALTROResultMEM        0x6000    // 20bit x 128
#define ALTROResultMEM_WIDTH  20
#define ALTROResultMEM_SIZE   128

// Altro Active Channel list
#define ALTROACL              0x6400    // 16bit x 256
#define ALTROACL_WIDTH        16
#define ALTROACL_SIZE         256

#define ALTROPatternMEM       0x6800    // 10bit x 1024
#define ALTROPatternMEM_WIDTH 10
#define ALTROPatternMEM_SIZE  1024

//ALTRO Instruction memory
#define ALTROInstMEM          0x7000    // 23bit x 256
#define ALTROInstMEM_WIDTH    23
#define ALTROInstMEM_SIZE     256

#define DataHEADER            0x4001    // 32bit x 6
#define DataHEADER_WIDTH      32
#define DataHEADER_SIZE       6

/* new define further down */
/* #define ALTROStatusREG        0x7800    // {16'h(fec_acl),12'h0,4'h(status)}	 */
/* #define ALTROStatusREG_WIDTH  32 */
/* #define ALTROStatusREG_SIZE   1 */

/* these registers never got used (SC design of 2004) */ 
/* #define SCResultMEM           0x8000    // 16bit x 16 */
/* #define SCResultMEM_WIDTH     16 */
/* #define SCResultMEM_SIZE      16 */

/* #define SCInstMEM             0x9000    // 11bit x 256 */
/* #define SCInstMEM_WIDTH       11 */
/* #define SCInstMEM_SIZE        256 */

/* #define DataAssemblerDataMEM        0x4000    // 32bit x 1024 */
/* #define DataAssemblerDataMEM_WIDTH  32 */
/* #define DataAssemblerDataMEM_SIZE   1024 */

#define FECActiveList         (0x8000 | BASE_FPGA_FABRIC_FIC0)
#define FECActiveList_WIDTH   32
#define FECActiveList_SIZE    1

//#define FECRDOList            0x8001
//#define FECRDOList_WIDTH      32
//#define FECRDOList_SIZE       1

#define FECResultREG          (0x8002 | BASE_FPGA_FABRIC_FIC0) 
#define FECResultREG_WIDTH    21
#define FECResultREG_SIZE     1

#define FECErrReg             (0x8003 | BASE_FPGA_FABRIC_FIC0)
#define FECErrReg_WIDTH       32
#define FECErrReg_SIZE        1


#define FECINTmode            0x510b
#define FECINTmode_WIDTH      2
#define FECINTmode_SIZE       1

#define FECSCAddREG           (0x8005 | BASE_FPGA_FABRIC_FIC0)
#define FECSCAddREG_WIDTH     32
#define FECSCAddREG_SIZE      1

#define FECSCDataREG          (0x8006 | BASE_FPGA_FABRIC_FIC0)
#define FECSCDataREG_WIDTH    32
#define FECSCDataREG_SIZE     1

/* a few FW version from May 06 use 0x8006 as FW version register */
#define RCUFwVersionOld       (0x8001 | BASE_FPGA_FABRIC_FIC0)
#define RCUFwVersionOld_WIDTH 24
#define RCUFwVersionOld_SIZE  1

#define RCUFwVersion          (0x8001 | BASE_FPGA_FABRIC_FIC0)
#define RCUFwVersion_WIDTH    24
#define RCUFwVersion_SIZE     1

//#define FECSWREG              0x8009
//#define FECSWREG_WIDTH        1
//#define FECSWREG_SIZE         1

#define V2_FECSWREG           (0x5107 | BASE_FPGA_FABRIC_FIC1)
#define V2_FECSWREG_WIDTH     1
#define V2_FECSWREG_SIZE      1

#define FECSCCmdREG           (0x8010 | BASE_FPGA_FABRIC_FIC0)
#define FECSCCmdREG_WIDTH     32
#define FECSCCmdREG_SIZE      1

#ifdef  ENABLE_ANCIENT_05
// Reset FEC Error Register
#define FECResetErrReg        (0x8012 | BASE_FPGA_FABRIC_FIC0)
#else   // !ENABLE_ANCIENT_05
#define FECResetErrReg        (0x8011 | BASE_FPGA_FABRIC_FIC0)
#endif  // ENABLE_ANCIENT_05
#define FECResetErrReg_WIDTH  1
#define FECResetErrReg_SIZE   1

// Reset FEC Result Rregister
#define FECResetResReg        (0x8012 | BASE_FPGA_FABRIC_FIC0)
#define FECResetResReg_WIDTH  1
#define FECResetResReg_SIZE   1

#define FECStatusMemory       0x8100
#define FECStatusMemory_WIDTH 16
#define FECStatusMemory_Size  32

#define StatusReg             0xA000    // {busySC,errorSC,busyAM,errorAM}	
#define StatusReg_WIDTH       32
#define StatusReg_SIZE        1

#define FECCommands           0xC
#define FECCommands_WIDTH     32
#define FECCommands_SIZE      4096

/** Altro ERRST register */
#define AltroErrSt            0x7800
/** bit width of Altro ERRST register */
#define AltroErrSt_WIDTH      32
/** size of Altro ERRST register block */
#define AltroErrSt_SIZE       1 
/** error in pattern memory comparission */
#define PATTERN_MISSMATCH     0x01
/** the rcu sequencer was aborted */
#define SEQ_ABORT             0x02
/** timeout during FEC access */
#define FEC_TIMEOUT           0x04
/** error bit of the Altro bus set during operation */
#define ALTRO_ERROR           0x08
/** channel address missmatch */
#define ALTRO_HWADD_ERROR     0x10
/** 
 * RCU Hardware address mask
 * the RCU Hardware address bits are RW and encode sector [8:3] and partition [2:0]
 */
#define RCU_HWADDR_MASK       0x001ff000
/** RCU Hardware address partition mask [2:0] */
#define RCU_HWADDR_PART_MASK  0x00007000
/** RCU Hardware address partition bit width */
#define RCU_HWADDR_PART_WIDTH 3
/** RCU Hardware address sector mask [8:3] */
#define RCU_HWADDR_SEC_MASK   0x001f8000
/** RCU Hardware address sector bit width */
#define RCU_HWADDR_SEC_WIDTH  6
/** RCU Hardware address width */
#define RCU_HWADDR_WIDTH      RCU_HWADDR_SEC_WIDTH+RCU_HWADDR_PART_WIDTH
/** the sequencer is busy */
#define RCU_SEQ_BUSY          0x80000000

#define AltroTrCfg            0x7801
#define AltroTrCfg_WIDTH      32
#define AltroTrCfg_SIZE       1

#define AltroTrCnt            0x7802
#define AltroTrCnt_WIDTH      32
#define AltroTrCnt_SIZE       1

#define AltroLwAdd            0x7803
#define AltroLwAdd_WIDTH      18
#define AltroLwAdd_SIZE       1

#define AltroIrAdd            0x7804
#define AltroIrAdd_WIDTH      20
#define AltroIrAdd_SIZE       1

#define AltroIrDat            0x7805
#define AltroIrDat_WIDTH      20
#define AltroIrDat_SIZE       1

#define AltroPmCfg            0x7806
#define AltroPmCfg_WIDTH      20
#define AltroPmCfg_SIZE       1

#define AltroChAdd            0x7807
#define AltroChAdd_WIDTH      20
#define AltroChadd_SIZE       1

#define FECCSR0               0x11
#define FECCSR1               0x12
#define FECCSR2               0x13
#define FECCRR3               0x14

// the following commands are executed if there is an access to the specified address
#define CMDExecALTRO          0x0000    // 1bit x 1
#define CMDAbortALTRO         0x0800    // 1bit x 1
#define CMDExecSC             0x1000    // 1bit x 1
#define CMDAbortSC            0x1800    // 1bit x 1
#define CMDRESET              0x2000    // 1bit x 1
#define CMDRESETFEC           0x2001    // 1bit x 1
#define CMDRESETRCU           0x2002    // 1bit x 1
#define CMDResAltroErrSt      0x6c01    // 1bit x 1
#define CMDDCS_ON             0xE000    // 1bit x 1
#define CMDSIU_ON             0xF000    // 1bit x 1
#define CMDL1TTC              0xE800    // 1bit x 1
#define CMDL1I2C              0xF800    // 1bit x 1
#define CMDL1CMD              0xD800    // 1bit x 1
#define CMDL1                 0xD000    // 1bit x 1
#define CMDSCLKsync           0x8020    // 1bit x 1

// Defines for Bitshift commands to address BC registers
// for the 5 bit version of the slow control module
#define MSMCommand5_base        12
#define MSMCommand5_rnw         11
#define MSMCommand5_bcast       10
#define MSMCommand5_branch       9
#define MSMCommand5_FECAdr       5
#define MSMCommand5_BCRegAdr     0
// for the 8 bit version of the slow control module
#define MSMCommand8_base        15
#define MSMCommand8_rnw         14
#define MSMCommand8_bcast       13
#define MSMCommand8_branch      12
#define MSMCommand8_FECAdr       8
#define MSMCommand8_BCRegAdr     0

// Defines for the Trigger Receiver Module
#define TTCControl		(0x4000 | BASE_FPGA_FABRIC_FIC1)
#define TTCControl_WIDTH        24
#define TTCControl_SIZE         1

#define TTCROIConfig1	        (0x4002 | BASE_FPGA_FABRIC_FIC1)
#define TTCROIConfig1_WIDTH     18
#define TTCROIConfig1_SIZE      1

#define TTCROIConfig2	        (0x4003 | BASE_FPGA_FABRIC_FIC1)
#define TTCROIConfig2_WIDTH     18
#define TTCROIConfig2_SIZE      1

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

#define TTCDAQHeader	        (0x4021 | BASE_FPGA_FABRIC_FIC1)
#define TTCDAQHeader_WIDTH      32
#define TTCDAQHeader_SIZE       1

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


//Defines for the Actel Device

/** Actel Command Register **/
#define ACTELCommandReg         0xB000
#define ACTELCommandReg_SIZE    1
#define ACTELCommandReg_WIDTH   32

/** Actel Selectmap Command Register **/
#define ACTELSmCommandReg       0xB002
#define ACTELSmCommandReg_SIZE  1
#define ACTELSmCommandReg_WIDTH 16

/** Actel Status Register **/
#define ACTELStatusReg          0xB100    
#define ACTELStatusReg_SIZE     1
#define ACTELStatusReg_WIDTH    12

/** Actel Error Register **/
#define ACTELErrorReg           0xB101
#define ACTELErrorReg_SIZE      1
#define ACTELErrorReg_WIDTH     8

/** Firmware Version Register**/
#define ACTELFwVersion          0xB102
#define ACTELFwVersion_SIZE     1
#define ACTELFwVersion_WIDTH    16

/** Actel Data Register **/
#define ACTELDataReg            0xB103    //
#define ACTELDataReg_SIZE       1
#define ACTELDataReg_WIDTH      32

/** Register holds the number of Errors encountered during RBAV **/
#define ACTELReadbackErrorCount          0xB104
#define ACTELReadbackErrorCount_SIZE     1
#define ACTELReadbackErrorCount_WIDTH    16

/** Register holds the number of the last frame with error **/
#define ACTELLastErrorFrameNumber        0xB105
#define ACTELLastErrorFrameNumber_SIZE   1
#define ACTELLastErrorFrameNumber_WIDTH  12

/** Number of Last frame beeing verified **/
#define ACTELLastFrameNumber             0xB106
#define ACTELLastFrameNumber_SIZE        1
#define ACTELLastFrameNumber_WIDTH       12

/** Number of times a complete readback and verification was done **/
#define ACTELReadbackCycleCounter        0xB107
#define ACTELReadbackCycleCounter_SIZE   1
#define ACTELReadbackCycleCounter_WIDTH  16

/** Register to stop automatic configuration **/
#define ACTELStopPowerUpCodeReg          0xB112
#define ACTELStopPowerUpCodeReg_SIZE     1
#define ACTELStopPowerUpCodeReg_WIDTH    32

/** Resets the Actel when written to **/
#define ACTELReset               0xB003
#define ACTELReset_SIZE          1
#define ACTELReset_WIDTH         1

#define ACTELFwVersion102        0x0102
#define ACTELFwVersion103        0x0103
#define ACTELFwVersion104        0x0104
#define ACTELCurrentFwVersion    ACTELFwVersion104

//Defines for the Flash memory addresses defined in the Actel Usermanual, see IFT wiki

#define RCUFLASHInitAddr         0x0
#define RCUFLASHInitAddr_SIZE    1
#define RCUFLASHInitAddr_WIDTH   32

#define RCUFLASHScrubAddr        0x1000
#define RCUFLASHScrubAddr_SIZE   1
#define RCUFLASHScrubAddr_WIDTH  32

#define RCUFLASHFramesAddr        0x2000
#define RCUFLASHFramesAddr_SIZE    1
#define RCUFLASHFramesAddr_WIDTH   32
// Defines for the DCS board

/** Defines the "old" mode to connect to the RCU Firmware **/
#define DCSOldMode               0xBF02
#define DCSOldMode_SIZE          1
#define DCSOldMode_WIDTH         16

#define DCSOldModeSet            0x01D
#define DCSOldModeUnset          0x0

#define DCSFwVersion             0xBF00
#define DCSFwVersion_SIZE        1
#define DCSFwVersion_WIDTH       32

#endif //__RCU2_HPP

