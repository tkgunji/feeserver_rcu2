#ifndef __RCU_HPP
#define __RCU_HPP

#include <vector>
#include <map>
#include "ctr.hpp"
#include "dev.hpp"
#include "fec.hpp"

/******************************************************************************/

class DevActel;
class DevRcu;

class CtrRcu: public Ctr{
public:
  CtrRcu();
  virtual ~CtrRcu();
  static int CtrRcuGO_IDLE(CEStateMachine* callbackObject);
  //RCU sub device
  DevRcu* fpRcu;
protected:
  //ACTEL sub device
  DevActel* fpActel;
  /** The FEC access */
  DevFecaccess* fpFecaccess;
  /** The FECs */
  DevFec* fpFecs[32];
};

/******************************************************************************/

class DevRcu: public Dev{
public:
  DevRcu(int id,  CEStateMachine* pParent, DevMsgbuffer* msgbuffer);
  virtual int GetGroupId();
  virtual int HighLevelHandler(const char* pCommand, std::vector<uint32_t>& rb);
  virtual int issue(uint32_t cmd, uint32_t parameter, const char* pData, int iDataSize, std::vector<uint32_t>& rb);
  virtual int SetAfl(uint32_t value);
  virtual int RcuAdcConf(uint32_t adc);
  static  int RcuAdcRead(Ser::TceServiceData* pData, int32_t adc, int32_t unused, void* parameter);
protected:
  virtual int ArmorLocal();
  virtual int ReSynchronizeLocal();
  DevMsgbuffer* fpMsgbuffer;
  std::map<uint32_t,uint32_t> fAltroInstr;
  std::map<uint32_t,uint32_t> fRegCfg;
  static std::string imemFileName;
  static std::string regCfgFileName;
};

//Class for access via i2c (slow control) bus
class DevFecaccessRcuI2c: public DevFecaccess{
public:
  DevFecaccessRcuI2c(DevMsgbuffer* msgbuffer);
  virtual int AccessFecReg(uint32_t &data, uint32_t fec, uint32_t reg, uint32_t rnw);
};
//Class for access via gtl (altro) bus
class DevFecaccessRcuGtl: public DevFecaccess{
public:
  DevFecaccessRcuGtl(DevMsgbuffer* msgbuffer);
  virtual int AccessFecReg(uint32_t &data, uint32_t fec, uint32_t reg, uint32_t rnw);
};

/******************************************************************************/

class DevMsgbuffer;

/**
 * @class CEactel
 *
 * \b TOC
 * - @ref CEactel_states
 * - @ref CEactel_transitions
 *
 * @section CEactel_states States
 * <br>
 * <b> SCRUBBING </b>
 * - performs continous scrubbing (overwriting of all frames, no matter if the contain errors or not)
 * - Number of cycles can be monitored with the cyclecounter
 * 
 * 
 * <br>
 *
 * <b> READBACK </b>
 * - executes continous Frame by Frame Readback and Verification 
 * - number of cycles can be monitored with the CycleCounter
 * - number of occurred errors can be monitored with the FrameErrorCount
 *
 *
 * <br>
 *
 * <b> OFF / idle </b>
 * - No checks done
 *
 * <br>
 *
 * <b> ON / standby</b>
 * - Checks successfully performed, everything ready
 * 
 * <br>
 *
 *
 * <pre>
 *
 *
 *              +-------->---------+---------------->----------------------------+
 *              |                  |                                             |
 *              ^                  ^                                             |
 *              |                  |                                             V
 *       -------------       ------------                                     -----------
 *      |  SCRUBBING  |     |  READBACK  |            +- GO_STANDBY/GO_IDLE -| ERROR (13)|
 *       -------------       ------------             |                          -----------
 *         |        |          |       |              |           can be reached by any state
 *         |        ^          |       |              |           or result of transition	      
 *         |        |          |       |              |           left by action 'go_standby'
 *         |    GO_SCRUBBING   |       |              |           or 'go idle'
 *         V        |          ^       |              V
 *         |        |          |       V              |
 *         |        |     GO_READBACK  |              |
 *         |        |          |       |              |
 *         |        |          |       |              |
 *         |        |          |       |              |
 *         |   ------------------      |              |
 *         +->| ON / configured  |<----+--GO_STANDBY--+
 *             ------------------                     |
 *                   | |                              |
 *                   ^ |                              |
 *                   | |                              |
 *            START  | v  STOP                        |
 *                   | |                              |
 *              -------------                         |
 *             | OFF / idle |<------GO_IDLE-----------+
 *              -------------
 * </pre>
 * @section CEactel_transitions Transitions
 * <b>GO_IDLE</b>
 *
 * <b>GO_STANDBY</b>
 *
 * <b>CONFIGURE</b>
 *
 * <b>START</b>
 *
 * <b>STOP</b>
 *
 * <b>GO_READBACK</b>
 *
 * <b>QUIT_READBACK</b>
 *
 * <b>GO_SCRUBBING</b>
 *
 * <b>QUIT_SCRUBBING</b>
 *
 * @ingroup rcuce
 */
class DevActel: public Dev{
public:
  DevActel(int id,  CEStateMachine* pParent, DevMsgbuffer* msgbuffer);

public:
  
   /**
   * Evaluate the state of the hardware.
   * The function does not change any internal data, it performes a number of evaluation
   * operations.
   * @return             state
   */
  int ReSynchronizeLocal();  
  
   /**
   * Internal function called during the @ref CEStateMachine::Armor procedure.
   * The function is called from the @ref CEStateMachine base class to carry out
   * device specific start-up tasks.
   * 
   *@return result value
   */
  int ArmorLocal();

  /******************************************************************************
   ********************                                     *********************
   ********************    state and transition handlers    *********************
   ********************                                     *********************
   *****************************************************************************/

  /**
   * @return            neg. error code if failed
   */
  int LeaveStateIDLE();
  
  /**
   * @return            neg. error code if failed
   */
  int LeaveStateRUNNING();


  /**
   * The Flash is checked for the Readback pointer, if it is present the 
   * Actel is switched to Readback mode. Error mode is entered and Message sent
   * to upper layers in case Flash pointer is not set up.
   * @return            neg. error code if failed
   */
  int EnterStateREADBACK();
  
  /**
   * @return            neg. error code if failed
   */
  int LeaveStateREADBACK();

  /**
   * The Flash is checked for the Scrubbing pointer, if present the Actel
   * is switched to Scrubbing mode. Error mode is entered and Message sent to upper
   * layers in case Flash pointer is not set up.
   * @return            neg. error code if failed
   */
  int EnterStateSCRUBBING();  

  /**
   * @return            neg. error code if failed
   */
  int LeaveStateSCRUBBING();
  
  /**
   * The handler for the <i>reset</i> action.
   * @return             >=0 success, neg error code if failed
   */
  static int DevActelGO_STANDBY(CEStateMachine* callbackObject);
  
public:

  /** the instance of the message buffer interface */
  DevMsgbuffer* fpMsgbuffer;

  /**
   * Command handling for the Actel command group.
   * @return          number of bytes of the payload buffer which have been processed
   *                  neg. error code if failed
   */
  int TranslateActelCommand(uint32_t cmd, uint32_t parameter, const char* pData, int iDataSize, std::vector<uint32_t>& rb);

  /**
   * Handler for high level commands.
   * The CEactel device provides the following high-level commands:
   * - ACTEL_INIT_CONFIG
   * @see CEIssueHandler::HighLevelHandler.
   * @param command   command string
   * @param rb        result buffer
   * @return 1 if processed, 0 if not, neg. error code if failled 
   */
  int HighLevelHandler(const char* pCommand, std::vector<uint32_t>& rb);

  int RcuEraseFlash();
  
/*********************************
 ***   Internal functions     ***
 ********************************/  
  /**
   * Does a init procedure of the Xilinx from the Flash,
   * which will erase the configuration memory.
   *
   * @return
   **/
  int XilinxInit();
  
  /**
   * Does a startup procedure of the Xilinx
   *
   * @return
   **/
  int XilinxStartup();

  /**
   * Issues an Abort sequence on the Xilinx 
   *
   * @return
   **/
  int XilinxAbort();

  /**
   * Reads the last active code from the status
   * register and returns it in this manner:
   * 
   * 1:Initial Configuration, 2: Scrubbing, 3: Scrubbing continously
   * 4: Readback Verification, 5: Readback Verification continously
   * 
   * @return last active command
   **/
  int LastActiveCommand();

  /**
   * Reads the status register and returns codes for
   * occured errors
   *
   * 1: Smap Interface busy, 4: Configuration controller is busy
   * 8: Flash contains no pointer value
   *
   * @return occured errors
   **/
  int CheckStatusReg();

  /**
   * Reads the error register and returns codes for
   * occured errors
   *
   * -64: Parity Error in Selectmap RAM detected
   * -128: Parity Error in Flash RAM detected
   *
   * @return occured errors
   **/
  int CheckErrorReg();
  
  
  /**
   * Read the firmware register
   *
   * @return
   **/
  uint32_t EvaluateFirmwareVersion();

  /**
   * Check the Flash for a Nullpointer
   *
   * @return
   */
  int CheckFlash();

  /**
   * Set the automatic configuration function,
   * 
   * @param the value to be set, 0xA to disable, 0x0 to enable.
   * @return
   **/
  int SetAutomaticConfiguration(uint32_t);
  
  /**
   * Clears the Error register
   *
   * @return 0 if success, EIO in case of failure
   **/
  int ClearErrorReg();

  /**
   * Clears the Error register
   *
   * @return 0 if success, EIO in case of failure
   **/
  int ClearStatusReg();

  /**
   * Trigger the Initial Configuration
   *
   * @return
   **/
  int InitialConfiguration();

  /**
   * Scrub one time
   *
   * @return
   **/
  int Scrub();
  
  /**
   * Scrub a given number of times, 0 for continously
   *
   * @return
   **/
  int Scrub(int times);
  
  /**
   * Readback and Verification one time
   *
   * @return
   **/
  int Readback();
  
  /**
   * Readback and Verification a given number of times, 0 for continously
   *
   * @return
   **/
  int Readback(int times);

  /**
   * Trigger an abort command on the Xilinx
   *
   * @return
   **/
  int Abort();

/*********************************
 ***Issuehandler functions     ***
 ********************************/
  
  /**
   * The commands (e.g. from the feeserver-ctrl arrive
   * here in this function and pass them on to the TranslateActelCommand
   **/
  int issue(uint32_t cmd, uint32_t parameter, const char* pData, int iDataSize, std::vector<uint32_t>& rb);

  /**
   * Get the Group ID for the commands that we serve here
   * 
   * 
   **/
  int GetGroupId();
  
  
  /******************************************
   ***** Functions Operating on the Flash ***
   *****************************************/

  //Write the Frame data to the Flash memory
  int WriteFramesFlash(const char* pData, int NofFrames);

  //Write Initial Configuration or Scrubbing Block to the Flash memory
  int WriteFlashBlock(const char* pDataOrig, unsigned int mode);


/*************************************************
 ***** Helper functions to program the flash *****
 ************************************************/

//returns the upper 16 bit of a 32 bit word
  uint16_t GetUpperAddress(uint32_t u32address);

//returns the lower 16 bit of a 32 bit word
  uint16_t GetLowerAddress(uint32_t u32address);


  int CalculatkActionStopAddress(uint32_t filesize, uint32_t startaddress);


private:
  /******************************************************************************
   ********************                                     *********************
   ********************          internal functions         *********************
   ********************                                     *********************
   *****************************************************************************/

  /**
   * Write a single location (32bit word).
   * Wrapper to dcscMsgBuffer method.
   * Overloaded by the RCU simulation class.
   * @param address   16 bit address in RCU memory space
   * @param data      data word
   * @return
   */
  virtual int SingleWrite(uint32_t address, uint32_t data);

  /**
   * Read a single location (32bit word).
   * Wrapper to dcscMsgBuffer method.
   * Overloaded by the RCU simulation class.
   * @param address   16 bit address in RCU memory space
   * @param pData     buffer to receive the data
   * @return
   */
  virtual int SingleRead(uint32_t address, uint32_t* pData);

  /**
   * Write a number of 32bit words beginning at a location.
   * The function takes care for the size of the MIB and splits the operation if
   * the amount of data to write exceeds the MIB size.
   * The function expects data in little endian byte order.
   * Wrapper to dcscMsgBuffer method.
   * Overloaded by the RCU simulation class.
   * @param address   16 bit address in RCU memory space
   * @param pData     buffer containing the data
   * @param iSize     number of words to write
   * @param iDataSize size of one word in bytes, allowed 1 (8 bit), 2 (16 bit),3 (compressed 10 bit) 
   *                  or 4 (32 bit) -2 and -4 denote 16/32 bit words which get swapped
   * @return
   */
  virtual int MultipleWrite(uint32_t address, uint32_t* pData, int iSize, int iDataSize);

  /**
   * Read a number of 32bit words beginning at a location.
   * Wrapper to dcscMsgBuffer method.
   * Overloaded by the RCU simulation class.
   * @param address   16 bit address in RCU memory space
   * @param iSize     number of words to write
   * @param pData     buffer to receive the data, the function expect it to be of suitable size
   *                  (i.e. iSize x wordsize)
   * @return          number of 32bit words which have been read, neg. error code if failed
   */
  virtual int MultipleRead(uint32_t address, int iSize,uint32_t* pData);


/************************************************************************************
 ************************************************************************************
 * DCSCMsgBuffer_flash_access Flash access
 ************************************************************************************
 ************************************************************************************/
/**
 * Write to the RCU flash. 
 * currently all communication is done through the message buffer
 * Wrapper to dcscMsgBuffer method. 
 * @param address    start location
 * @param pData      buffer containing the data
 * @param iSize      number of words to write
 * @param iDataSize  size of one word in bytes, allowed 1,2
 * @return
 */
  virtual int FlashWrite(uint32_t address, uint32_t* pData, int iSize, int iDataSize);

/**
 * Read a number of 16bit words from the flash memory.
 * currently all communication is done through the message buffer
 * Wrapper to dcscMsgBuffer method.
 * @param address    start location
 * @param iSize      number of words to read
 * @param pData      buffer to receive the data, the function expect it to be of suitable size 
 *                   (i.e. iSize x wordsize)
 * @return number of read 32bit words, neg. error code if failed
 */
  virtual int FlashRead(uint32_t address, int iSize,uint32_t* pData);

/**
 * Erase sectors of the flash.   
 * Wrapper to dcscMsgBuffer method.
 * @param  firstSec   the first sector to erase; if -1 erase all
 * @param  lastSec    the last sector to erase
 * @return 0 success, neg. error code if failed
 */
  virtual int FlashErase(int startSec, int stopSec);
  
};

/******************************************************************************/

/* the memory locations on the rcu card
 */

/*************************************
   RCU FW2 Registers and Memories
*************************************/

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

#define V2_FECActiveList         0x5100
#define V2_FECActiveList_WIDTH   32
#define V2_FECActiveList_SIZE    1

#define V2_FECRDOList            0x5100
#define V2_FECRDOList_WIDTH      32
#define V2_FECRDOList_SIZE       1

#define V2_FECActiveList_RO       0x8000
#define V2_FECActiveList_RO_WIDTH 32
#define V2_FECActiveList_RO_SIZE  1

#define V2_FECRDOList_RO         0x8000
#define V2_FECRDOList_RO_WIDTH   32
#define V2_FECRDOList_RO_SIZE    1

//rcufw2 FECERR_A and FECERR_B registers
#define V2_FECErrA               0x5110
#define V2_FECErrA_WIDTH         32
#define V2_FECErrA_SIZE          1

#define V2_FECErrB               0x5111
#define V2_FECErrB_WIDTH         32
#define V2_FECErrB_SIZE          1

#define V2_FECErrA_RO            0x800A
#define V2_FECErrA_RO_WIDTH      32
#define V2_FECErrA_RO_SIZE       1

#define V2_FECErrB_RO            0x800B
#define V2_FECErrB_RO_WIDTH      32
#define V2_FECErrB_RO_SIZE       1

#define V2_AltroIf               0x5101
#define V2_AltroIf_WIDTH         16
#define V2_AltroIf_SIZE          1

#define V2_AltroTrCfg            0x5102
#define V2_AltroTrCfg_WIDTH      16
#define V2_AltroTrCfg_SIZE       1

#define V2_RDOMod                0x5103
#define V2_RDOMod_WIDTH          4
#define V2_RDOMod_SIZE           1

#define V2_AltroCfg1             0x5104
#define V2_AltroCfg1_WIDTH       16
#define V2_AltroCfg1_SIZE        1

#define V2_AltroCfg2             0x5105
#define V2_AltroCfg2_WIDTH       16
#define V2_AltroCfg2_SIZE        1

#define V2_RCU_Version           0x5106
#define V2_RCU_Version_WIDTH     1
#define V2_RCU_Version_SIZE      1

#define V2_RCU_BPVersion         0x5107
#define V2_RCU_BPVersion_WIDTH   1
#define V2_RCU_BPVersion_SIZE    1

#define V2_RCUId                 0x5108
#define V2_RCUId_WIDTH           9
#define V2_RCUId_SIZE            1

#define V2_BUSBSY                0x5116
#define V2_BUSBSY_WIDTH          1
#define V2_BUSBSY_SIZE           1

#define V2_CMDRESET              0x5300    // 1bit x 1

//Front End Card reset
#define V2_CMDRESETFEC           0x5301    // 1bit x 1

//RCU reset
#define V2_CMDRESETRCU           0x5302    // 1bit x 1

#define CONFGFEC                 0x5303    // 1bit x 1

#define V2_CMDExecALTRO          0x5304    // 1bit x 1

#define V2_CMDAbortALTRO         0x5305    // 1bit x 1

#define V2_CMDResAltroErrSt      0x5307    // 1bit x 1

#define V2_CMDSCLKsync           0x530a    // 1bit x 1

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

#define FECActiveList         0x8000
#define FECActiveList_WIDTH   32
#define FECActiveList_SIZE    1

#define FECRDOList            0x8001
#define FECRDOList_WIDTH      32
#define FECRDOList_SIZE       1

#define FECResultREG          0x8002
#define FECResultREG_WIDTH    21
#define FECResultREG_SIZE     1

#define FECErrReg             0x8003
#define FECErrReg_WIDTH       32
#define FECErrReg_SIZE        1

#define FECINTmode            0x510b
#define FECINTmode_WIDTH      2
#define FECINTmode_SIZE       1

#define FECSCAddREG           0x8005
#define FECSCAddREG_WIDTH     32
#define FECSCAddREG_SIZE      1

#define FECSCDataREG          0x8006
#define FECSCDataREG_WIDTH    32
#define FECSCDataREG_SIZE     1

/* a few FW version from May 06 use 0x8006 as FW version register */
#define RCUFwVersionOld       0x8006
#define RCUFwVersionOld_WIDTH 24
#define RCUFwVersionOld_SIZE  1

#define RCUFwVersion          0x8008
#define RCUFwVersion_WIDTH    24
#define RCUFwVersion_SIZE     1

#define FECSWREG              0x8009
#define FECSWREG_WIDTH        1
#define FECSWREG_SIZE         1

#define V2_FECSWREG           0x5107
#define V2_FECSWREG_WIDTH     1
#define V2_FECSWREG_SIZE      1

#define FECSCCmdREG           0x8010
#define FECSCCmdREG_WIDTH     32
#define FECSCCmdREG_SIZE      1

#ifdef  ENABLE_ANCIENT_05
// Reset FEC Error Register
#define FECResetErrReg        0x8012
#else   // !ENABLE_ANCIENT_05
#define FECResetErrReg        0x8011
#endif  // ENABLE_ANCIENT_05
#define FECResetErrReg_WIDTH  1
#define FECResetErrReg_SIZE   1

// Reset FEC Result Rregister
#define FECResetResReg        0x8012
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
#define TTCControl		0x4000
#define TTCControl_WIDTH        24
#define TTCControl_SIZE         1

#define TTCROIConfig1	        0x4002
#define TTCROIConfig1_WIDTH     18
#define TTCROIConfig1_SIZE      1

#define TTCROIConfig2	        0x4003
#define TTCROIConfig2_WIDTH     18
#define TTCROIConfig2_SIZE      1

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

#define TTCDAQHeader	        0x4021
#define TTCDAQHeader_WIDTH      32
#define TTCDAQHeader_SIZE       1

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

#endif //__RCU_HPP

