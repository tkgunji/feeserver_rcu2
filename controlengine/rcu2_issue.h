#ifndef __RCU2_ISSUE_H
#define __RCU2_ISSUE_H

/**
 * @defgroup rcu_issue Command interpretation for the RCU2 FeeServer
 * This group describes the command interface to the RCU2-like FeeServer ControlEngine.
 *
 * The <i>issue</i> method of the CE API expects the input either as encoded in 
 * message buffer format or as a ControlEngine command.
 * The FEE API supports currently one DIM command channel of type char array. The RCU2 CE
 * treats the first 4 byte as a 32bit header word.
 * <b>Note:</b> The architecture of the ARM linux is <b>little endian</b>, which means that
 * the least significant byte comes first. 
 * All feeserver commands start with <b>0xf</b> in there most significant bits, anything else
 * will be treated as a BLOB encoded in the message buffer format. 
 * @see dcscMsgBufferInterface/dcscMsgBufferInterface.h for detailed 
 * description of the message buffer interface to the RCU2 motherboard.
 * FEESERVER_CMD is predefined as 0xf0000000 (dcscMsgBufferInterface/dcscMsgBufferInterface.h)
 *
 * @author Matthias Richter
 * @ingroup feesrv_ce
 */

/**
 * @name Layout of FeeServer command
 * The header of a FeeServer command has the following format: <pre>
 * Bit 32 - 28 | 27 - 24 | 23 - 16 | 15 -0 
 *     1 1 1 1   cmd id    sub id    parameter 
 *                                      ^------- 16 bit user parameter passed to the handler function 
 *                            ^-----------------  8 bit command sub id passed to the handler function 
 *                   ^--------------------------  4 bit command id switches to the handler function 
 *        ^-------------------------------------  4 bit code for FeeServer command 
 * </pre>
 *
 * A data block is following, its content depends on the specific command. <br>
 * The whole command is terminated by an end-marker, which also contains the version encoded in the
 * lower 16 bits. The CE issue handler always treats the last 4 bytes of the incoming command block as
 * the 32 bit trailer. The command @ref CE_RELAX_CMD_VERS_CHECK can be used to disable the check.
 * @ingroup rcu_issue
 */

/**
 * CE command ctrl sequence mask
 * @ingroup rcu_issue
 */
#define FEESERVER_CMD_MASK                0xf0000000

/**
 * the command id mask
 * @ingroup rcu_issue
 */
#define FEESERVER_CMD_ID_MASK             0x0f000000

/**
 * the command sub id mask
 * @ingroup rcu_issue
 */
#define FEESERVER_CMD_SUB_MASK            0x00ff0000

/**
 * the command parameter mask
 * @ingroup rcu_issue
 */
#define FEESERVER_CMD_PARAM_MASK          0x0000ffff

/**
 * bit shift of general CE command ctrl sequence
 * @ingroup rcu_issue
 */
#define FEESERVER_CMD_BITSHIFT            28

/**
 * bit shift of the command id
 * @ingroup rcu_issue
 */
#define FEESERVER_CMD_ID_BITSHIFT         24

/**
 * bit shift of the command sub id
 * @ingroup rcu_issue
 */
#define FEESERVER_CMD_SUB_BITSHIFT        16

/**
 * bit shift of the parameter
 * @ingroup rcu_issue
 */
#define FEESERVER_CMD_PARAM_BTSHFT        0

/**
 * end marker mask
 * @ingroup rcu_issue
 */
#define CE_CMD_EM_MASK                    0xffff0000

/**
 * end version mask
 * @ingroup rcu_issue
 */
#define CE_CMD_VS_MASK                    0x0000ffff

/**
 * the end marker
 * @ingroup rcu_issue
 */
#define CE_CMD_ENDMARKER                  0xdd330000

// FEESERVER_CMD is defined in dcscMsgBufferInterface.h if the file is part of a project
// including the message buffer interface. If the latter header file is not available, 
// take the default value
#ifndef FEESERVER_CMD
#define FEESERVER_CMD                     0xf0000000
#endif //FEESERVER_CMD 

/* command defines
 * the number of additional 32 bit data words is given in {}
 */ 

/****************************************************************************************/

/**
 * commandset UNUSED 0.
 * @ingroup rcu_issue
 */
#define FEESVR_CMD_UNUSED0               (0x00000000 | FEESERVER_CMD)

/****************************************************************************************/

/**
 * @name Group 1: Commands for the ControlEngine.
 * @ingroup rcu_issue
 */

/**
 * ID of group 1.
 * 0xf1xxxxxx
 * @ingroup rcu_issue
 */
#define FEESERVER_CE_CMD                 (0x01000000 | FEESERVER_CMD)

/**
 * turn on/off the update of the services.
 * parameter: 0x0 off, otherwise on
 * payload: 0
 * @ingroup rcu_issue
 */
#define CEDBG_EN_SERVICE_UPDATE            (0x030000 | FEESERVER_CE_CMD)

/**
 * Force update of channels
 * parameter: name of channel
 * payload: 0
 * @ingroup rcu_issue
 */
#define CE_FORCE_CH_UPDATE                 (0x040000 | FEESERVER_CE_CMD)

/**
 * set the logging level (@ref rcu_ce_base_logging).
 * parameter: log level <br>
 * payload: 0
 * @ingroup rcu_issue
 */
#define CE_SET_LOGGING_LEVEL               (0x080000 | FEESERVER_CE_CMD)

/**
 * Trigger transition for one of the state machines.
 * parameter: number of bytes in the payload <br>
 * payload: char buffer containing service name belonging to the state
 * machine followed by the action separated by a blank.<br>
 * return: none<br>
 * <b>Note:</b> defined in command set version 2 <br>
 * @ingroup rcu_issue
 */
#define CE_TRIGGER_TRANSITION              (0x0b0000 | FEESERVER_CE_CMD)

/**
 * Terminating the FeeServer
 * parameter: none <br>
 * payload: 0
 * @ingroup rcu_issue
 */
#define CE_TERMINATE                       (0x0c0000 | FEESERVER_CE_CMD)

/**
 * Rebooting the computer
 * parameter: none <br>
 * payload: 0
 * @ingroup rcu_issue
 */
#define CE_REBOOT                          (0x0d0000 | FEESERVER_CE_CMD)

/**
 * Set the sleep time between update cycles.
 * parameter: sleep time <br>
 * payload: 0
 * @ingroup rcu_issue
 */
#define CE_UPDATE_SLEEP                    (0x0e0000 | FEESERVER_CE_CMD)

/**
 * Reconfigure PLD
 * parameter: none <br>
 * payload: 0
 * @ingroup rcu_issue
 */
#define CE_RECONFIGURE_PLD                 (0x0f0000 | FEESERVER_CE_CMD)

/**
 * Test error state
 * parameter: none <br>
 * payload: 0
 * @ingroup rcu_issue
 */
#define CE_TEST_ERROR                      (0x100000 | FEESERVER_CE_CMD)

/**
 * A configure command for the FEE.
 * The command encapsulates and arbitrary sequence of other commands. The 
 * 'Hardware address' (HW address) is a unique ID specifying the target
 * device/partition to receive the configuration command.<br>
 *
 * parameter: number of words inside the block after the checksum, i.e. the
 *            the total number of  (including @ref FEE_VERIFICATION 
 * and @ref FEE_CONFIGURE_END)<br>
 * payload: <br>
 * - 32 bit HW address, related to the ALTRO addressing<br>
 * - 32 bit checksum for this configuration command
 * - first command of the block 
 * - ...
 * - last configuration command of the block
 * - @ref FEE_VERIFICATION command
 * - @ref FEE_CONFIGURE_END command
 *
 * The @ref FEE_VERIFICATION command can be empty but is mandatory.<br>
 * The @ref FEE_CONFIGURE_END command signals the end of the configuration for
 * a device/partition.<br>
 * return: the output of all the sub-commands<br>
 * <b>Note:</b> defined in command set version 2 <br>
 * @ingroup rcu_issue
 */
#define FEE_CONFIGURE                      (0x200000 | FEESERVER_CE_CMD)

/**
 * Device mask for @ref FEE_CONFIGURE command.
 * This is the mask for the device bits of the hardware address of the
 * @ref FEE_CONFIGURE command.
 * @ingroup rcu_issue
 */
#define FEE_CONFIGURE_HW_DEVICE             0xf000

/**
 * Altro address mask for @ref FEE_CONFIGURE command.
 * Mask for the ALTRO address branch part of the of the hardware address of the
 * @ref FEE_CONFIGURE command.
 * @ingroup rcu_issue
 */
#define FEE_CONFIGURE_HW_ALTRO_BRANCH       0x0800

/**
 * Altro address mask for @ref FEE_CONFIGURE command.
 * Mask for the ALTRO address FEC part of the of the hardware address of the
 * @ref FEE_CONFIGURE command.
 * @ingroup rcu_issue
 */
#define FEE_CONFIGURE_HW_ALTRO_FEC          0x0780

/**
 * Altro address mask for @ref FEE_CONFIGURE command.
 * Mask for the ALTRO address number part of the of the hardware address of the
 * @ref FEE_CONFIGURE command.
 * @ingroup rcu_issue
 */
#define FEE_CONFIGURE_HW_ALTRO_NO           0x0070

/**
 * Altro address mask for @ref FEE_CONFIGURE command.
 * Mask for the ALTRO address channel part of the of the hardware address of the
 * @ref FEE_CONFIGURE command.
 * @ingroup rcu_issue
 */
#define FEE_CONFIGURE_HW_ALTRO_CHANNEL      0x000f

/**
 * Signal the end of the configuration for a device specified by the HW address.
 * The end of a configuration for a device/partition will cause it to change its state. <br>
 * parameter: ignored <br>
 * payload: 32 bit HW address as for the @ref FEE_CONFIGURE command<br>
 * result: none <br>
 *
 * <b>Note:</b> defined in command set version 2 <br>
 * @ingroup rcu_issue
 */
#define FEE_CONFIGURE_END                  (0x210000 | FEESERVER_CE_CMD)

/**
 * Specify a verification sequence.
 * The command can only occur inside a @ref FEE_CONFIGURE command and will be connected to
 * this. The verification daemon periodically checks the configuration with the help of
 * verification sequences. The daemon doesn't know about the operations of the verification 
 * and neither the result. 
 * It treats the sub-commands as normal CE commands and calculates the checksum from the
 * result.<br>
 * <b>Note:</b> This implies a modulation of the hardware when preparing the
 * verification command block.<br>
 * <br>
 * parameter: number of payload words excluding the 2 checksum words. If 0, no checksum words
 *            are expected <br>
 * payload: <br>
 * - 32 bit checksum for this verification command block
 * - 32 bit checksum for the result of the verification sequence
 * - first command of the block 
 * - ...
 * - last command of the block
 *
 * <b>Note:</b> An empty verification command (parameter zero) does not have any payload, not even
 * the checksums.<br>
 * <br>
 * return: the output of all the sub-commands<br>
 *
 * <b>Note:</b> defined in command set version 2 <br>
 * @ingroup rcu_issue
 */
#define FEE_VERIFICATION                   (0x220000 | FEESERVER_CE_CMD)

/**
 * Signal the need for external configuration data.
 * The command can only occur inside a @ref FEE_CONFIGURE command and will abort the interpretation
 * of the configuration sequence. The device will go into state @ref CONF_EXTERNAL and wait for a
 * <i>closeConfExternal</i> action. After this action the device is considered to be completely
 * configured and it wil go to state @ref STBY_CONFIGURED. <br>
 * <br>
 * parameter: lower 16 bit of the device HW address described in @ref FEE_CONFIGURE <br>
 * payload: none <br>
 * return: none<br>
 *
 * <b>Note:</b> defined in command set version 2 <br>
 * @ingroup rcu_issue
 */
#define FEE_EXTERNAL_CONFIGURATION         (0x230000 | FEESERVER_CE_CMD)

/**
 * Same as the @ref FEE_CONFIGURE command, but size of block is stored in first
 * word of data block rather than in parameter. To be used if more than 16 bit
 * payload size is needed.
 * @ingroup rcu_issue
 */
#define FEE_CONFIGURE_32                   (0x240000 | FEESERVER_CE_CMD)

/**
 * Same as the @ref FEE_CONFIGURE command, but will not trigger any change of state.
 * @ingroup rcu_issue
 */
#define FEE_CONFIGURE_NOSTATE              (0x250000 | FEESERVER_CE_CMD)

/**
 * Same as the @ref FEE_CONFIGURE_END command, but will not trigger any change of state.
 * @ingroup rcu_issue
 */
#define FEE_CONFIGURE_END_NOSTATE          (0x260000 | FEESERVER_CE_CMD)

/**
 * Same as the @ref FEE_CONFIGURE_32 command, but will not trigger any change of state.
 * @ingroup rcu_issue
 */
#define FEE_CONFIGURE_32_NOSTATE           (0x270000 | FEESERVER_CE_CMD)
/****************************************************************************************/

/**
 * commandset UNUSED 2.
 * @ingroup rcu_issue
 */
#define FEESVR_CMD_UNUSED2               (0x02000000 | FEESERVER_CMD)

/****************************************************************************************/

/**
 * @name Group 3: command set for the RCU
 * @ingroup rcu_issue
 */

/**
 * ID of group 3
 * 0xf3xxxxxx
 * @ingroup rcu_issue
 */
#define FEESVR_CMD_RCU2                  (0x03000000 | FEESERVER_CMD)

/**
 * send the execution command to run the sequence written to rcu instruction memory.
 * parameter: 8 bit start address in the instruction memory <br>
 * payload: 0
 * <b>Note:</b> The start address is available from command set version 2, in version 1 the parameter is just ignored <br>
 * @ingroup rcu_issue
 */
#define RCU2_EXEC                           (0x010000 | FEESVR_CMD_RCU2)

/**
 * stop the execution of the RCU2 sequencer.
 * parameter: ignored <br>
 * payload: 0
 * @ingroup rcu_issue
 */
#define RCU2_STOP_EXEC                      (0x020000 | FEESVR_CMD_RCU2)

// the following commands allow to write a block of data to rcu instruction or pattern memory
// the number of 32 bit words is given by the command parameter

/**
 * write to rcu instruction memory.
 * parameter: number of 32 bit words in the payload <br>
 * payload: 32 bit data words
 * @ingroup rcu_issue
 */
#define RCU2_WRITE_INSTRUCTION              (0x030000 | FEESVR_CMD_RCU2)

/**
 * write to rcu instruction memory and send the execution command.
 * parameter: number of 32 bit words in the payload <br>
 * payload: 32 bit data words
 * @ingroup rcu_issue
 */
#define RCU2_EXEC_INSTRUCTION               (0x040000 | FEESVR_CMD_RCU2)

//No longer supported
//#define RCU2_WRITE_PATTERN8               (0x050000 | FEESVR_CMD_RCU2)
//#define RCU2_WRITE_PATTERN16              (0x060000 | FEESVR_CMD_RCU2)
//#define RCU2_WRITE_PATTERN32              (0x070000 | FEESVR_CMD_RCU2)
//#define RCU2_WRITE_PATTERN10              (0x080000 | FEESVR_CMD_RCU2)

/**
 * read from rcu instruction memory.
 * parameter: number of 32 bit words to read <br>
 * payload: 0
 * @ingroup rcu_issue
 */
#define RCU2_READ_INSTRUCTION               (0x090000 | FEESVR_CMD_RCU2)

//No longer supported
//#define RCU2_READ_PATTERN                 (0x0a0000 | FEESVR_CMD_RCU2)

// the following commands allow to read and write from/to an rcu memory location
// parameter is a 16 bit address

/**
 * read from rcu memory location.
 * parameter: 16 bit address <br>
 * payload: 0
 * @ingroup rcu_issue
 */
#define RCU2_READ_MEMORY                    (0x0b0000 | FEESVR_CMD_RCU2)

/**
 * write to rcu memory location.
 * parameter: 16 bit address <br>
 * payload: 1 32 bit word
 * @ingroup rcu_issue
 */
#define RCU2_WRITE_MEMORY                   (0x0c0000 | FEESVR_CMD_RCU2)

/**
 * write to rcu result memory.
 * parameter: number of 32 bit words in the payload <br>
 * payload: 32 bit data words
 * @ingroup rcu_issue
 */
#define RCU2_WRITE_RESULT                   (0x0d0000 | FEESVR_CMD_RCU2)

/**
 * read from rcu result memory.
 * parameter: number of 32 bit words to read <br>
 * payload: 0
 * @ingroup rcu_issue
 */
#define RCU2_READ_RESULT                    (0x0e0000 | FEESVR_CMD_RCU2)

/**
 * write block to rcu memory.
 * parameter: number of 32 bit words to write <br>
 * payload: the first 32 bit word of the payload is the address to write to, the 32 bit data follows
 * @ingroup rcu_issue
 */
#define RCU2_WRITE_MEMBLOCK                 (0x100000 | FEESVR_CMD_RCU2)

/**
 * read block from rcu memory.
 * parameter: number of 32 bit words to read <br>
 * payload: address
 * @ingroup rcu_issue
 */
#define RCU2_READ_MEMBLOCK                  (0x110000 | FEESVR_CMD_RCU2)

//No longer supported
//#define RCU2_READ_ERRST                   (0x120000 | FEESVR_CMD_RCU2)

/**
 * write the TRGCFG register of the RCU2.
 * parameter: ignored <br>
 * payload: 32bit data (only bit 0 to 28 valid) <br>
 * result: none <br>
 * <b>Note:</b> defined in command set version 2 <br>
 * @ingroup rcu_issue
 */
#define RCU2_WRITE_TRGCFG                   (0x130000 | FEESVR_CMD_RCU2)

/**
 * read the TRGCFG register of the RCU2.
 * parameter: ignored <br>
 * payload: none <br>
 * result: 32 bit register content <br>
 * <b>Note:</b> defined in command set version 2 <br>
 * @ingroup rcu_issue
 */
#define RCU2_READ_TRGCFG                    (0x140000 | FEESVR_CMD_RCU2)

/**
 * write the PMCFG register of the RCU2.
 * <b>Note:</b>The PMCFG register is 20 bit wide.
 * parameter: ignored <br>
 * payload: 32bit data <br>
 * result: none <br>
 * <b>Note:</b> defined in command set version 2 <br>
 * @ingroup rcu_issue
 */
#define RCU2_WRITE_PMCFG                    (0x150000 | FEESVR_CMD_RCU2)

/**
 * read the PMCFG register of the RCU2.
 * <b>Note:</b>The PMCFG register is 20 bit wide.
 * parameter: ignored <br>
 * payload: none <br>
 * result: 32 bit register content <br>
 * <b>Note:</b> defined in command set version 2 <br>
 * @ingroup rcu_issue
 */
#define RCU2_READ_PMCFG                     (0x160000 | FEESVR_CMD_RCU2)

/**
 * read the TRCNT register of the RCU2.
 * parameter: ignored <br>
 * payload: none <br>
 * result: 32 bit register content <br> 
 * <b>Note:</b> defined in command set version 3 <br>
 * @ingroup rcu_issue
 */
#define RCU2_READ_TRCNT                     (0x170000 | FEESVR_CMD_RCU2)

/**
 * write the AFL register of the RCU2.
 * parameter: ignored <br>
 * payload: 32bit data <br>
 * result: none <br>
 * <b>Note:</b> defined in command set version 3 <br>
 * @ingroup rcu_issue
 */
#define RCU2_WRITE_AFL                      (0x180000 | FEESVR_CMD_RCU2)

/**
 * read the AFL register of the RCU2.
 * parameter: ignored <br>
 * payload: none <br>
 * result: 32 bit register content <br>
 * <b>Note:</b> defined in command set version 3 <br>
 * @ingroup rcu_issue
 */
#define RCU2_READ_AFL                       (0x190000 | FEESVR_CMD_RCU2)

/**
 * write to rcu ACL memory.
 * parameter: number of 32 bit words in the payload <br>
 * payload: 32 bit data words <br>
 * <b>Note:</b> defined in command set version 3 <br>
 * @ingroup rcu_issue
 */
#define RCU2_WRITE_ACL                      (0x1a0000 | FEESVR_CMD_RCU2)

/**
 * read from rcu ACL memory.
 * parameter: number of 32 bit words to read <br>
 * payload: 0 <br>
 * <b>Note:</b> defined in command set version 3 <br>
 * @ingroup rcu_issue
 */
#define RCU2_READ_ACL                       (0x1b0000 | FEESVR_CMD_RCU2)

/**
 * write to rcu HEADER memory.
 * parameter: number of 32 bit words in the payload <br>
 * payload: 32 bit data words <br>
 * <b>Note:</b> defined in command set version 3 <br>
 * @ingroup rcu_issue
 */
#define RCU2_WRITE_HEADER                   (0x1c0000 | FEESVR_CMD_RCU2)

/**
 * read from rcu HEADER memory.
 * parameter: number of 32 bit words to read <br>
 * payload: 0 <br>
 * <b>Note:</b> defined in command set version 3 <br>
 * @ingroup rcu_issue
 */
#define RCU2_READ_HEADER                    (0x1d0000 | FEESVR_CMD_RCU2)

//No longer supported
//#define RCU2_CHECK_ERROR                  (0x200000 | FEESVR_CMD_RCU2)
//#define RCU2_EN_AUTO_CHECK                (0x210000 | FEESVR_CMD_RCU2)
//#define RCU2_SET_ABM                      (0x220000 | FEESVR_CMD_RCU2)
//#define RCU2_CHECK_VERSION                (0x230000 | FEESVR_CMD_RCU2)

/**
 * Reset the FEE
 * parameter: 1 = only FECs, 2 = only RCU2, global otherwise <br>
 * payload: 0 <br>
 * result: none <br>
 * <b>Note:</b> defined in command set version 3 <br>
 * <b>Note 2:</b> this command serves the GLB_RESET, FEC_RESET and
 * RCU2_RESET operations of the RCU2 firmware <br>
 * @ingroup rcu_issue
 */
#define RCU2_RESET                          (0x240000 | FEESVR_CMD_RCU2)

/**
 * Enable trigger source
 * parameter: 1 = L1_TTC, 2 = L1_I2C, L1_CMD otherwise <br>
 * payload: 0 <br>
 * result: none <br>
 * <b>Note:</b> defined in command set version 3 <br>
 * <b>Note 2:</b> this command serves the L1_CMD, L1_TTC and
 * L1_I2C operations of the RCU2 firmware <br>
 * @ingroup rcu_issue
 */
#define RCU2_L1TRG_SELECT                   (0x250000 | FEESVR_CMD_RCU2)

/**
 * Send an L1 trigger (L1 operation of RCU2 firmware)
 * parameter: ignored <br>
 * payload: 0 <br>
 * result: none <br>
 * <b>Note:</b> defined in command set version 3 <br>
 * @ingroup rcu_issue
 */
#define RCU2_SEND_L1_TRIGGER                (0x260000 | FEESVR_CMD_RCU2)

/**
 * Read the RCU2 firmware version.
 * parameter: ignored <br>
 * payload: 0 <br>
 * result: version id, 0 if no version info available <br>
 * <b>Note:</b> defined in command set version 4 <br>
 * @ingroup rcu_issue
 */
#define RCU2_READ_FW_VERSION                (0x270000 | FEESVR_CMD_RCU2)

/**
 * Re-synchronize the SCLK with the next L1TTC trigger.
 * parameter: ignored <br>
 * payload: 0 <br>
 * result: none <br>
 * <b>Note:</b> defined in command set version 4 <br>
 * @ingroup rcu_issue
 */
#define RCU2_SYNC_SCLK_L1TTC                (0x280000 | FEESVR_CMD_RCU2)

  //new in rcufw2

/**
 * write the TTC L1 Latency register.
 * <b>Note:</b>The register is 16 bit wide.
 * parameter: ignored <br>
 * payload: 32bit data <br>
 * result: none <br>
 * <b>Note:</b> defined in command set version 2 <br>
 * @ingroup rcu_issue
 */
#define WRITE_TTC_L1_LATENCY               (0x290000 | FEESVR_CMD_RCU2)

/**
 * write the TTC L1 message latency register.
 * <b>Note:</b>The register is 32 bit wide.
 * parameter: ignored <br>
 * payload: 32bit data <br>
 * result: none <br>
 * <b>Note:</b> defined in command set version 2 <br>
 * @ingroup rcu_issue
 */
#define WRITE_TTC_L1_MSG_LATENCY           (0x2A0000 | FEESVR_CMD_RCU2)

/**
 * write the ALTRO IF register.
 * <b>Note:</b>The register is 16 bit wide.
 * parameter: ignored <br>
 * payload: 32bit data <br>
 * result: none <br>
 * <b>Note:</b> defined in command set version 2 <br>
 * @ingroup rcu_issue
 */
#define WRITE_ALTROIF                      (0x2B0000 | FEESVR_CMD_RCU2)

/**
 * write the RCU2 Readout mode register.
 * <b>Note:</b>The register is 4 bit wide.
 * parameter: ignored <br>
 * payload: 32bit data <br>
 * result: none <br>
 * <b>Note:</b> defined in command set version 2 <br>
 * @ingroup rcu_issue
 */
#define WRITE_RDO_MOD                      (0x2C0000 | FEESVR_CMD_RCU2)

/**
 * read the TTC L1 Latency register.
 * parameter: ignored <br>
 * payload: none <br>
 * result: 32 bit register content <br>
 * <b>Note:</b> defined in command set version 7 <br>
 * @ingroup rcu_issue
 */
#define READ_TTC_L1_LATENCY                (0x2D0000 | FEESVR_CMD_RCU2)

/**
 * read the TTC L1 Message Latency register.
 * parameter: ignored <br>
 * payload: none <br>
 * result: 32 bit register content <br>
 * <b>Note:</b> defined in command set version 7 <br>
 * @ingroup rcu_issue
 */
#define READ_TTC_L1_MSG_LATENCY            (0x2E0000 | FEESVR_CMD_RCU2)

/**
 * read the ALTRO IF register.
 * parameter: ignored <br>
 * payload: none <br>
 * result: 32 bit register content <br>
 * <b>Note:</b> defined in command set version 7 <br>
 * @ingroup rcu_issue
 */
#define READ_ALTROIF                       (0x2F0000 | FEESVR_CMD_RCU2)

/**
 * read the RCU2 Readout Module register.
 * parameter: ignored <br>
 * payload: none <br>
 * result: 32 bit register content <br>
 * <b>Note:</b> defined in command set version 7 <br>
 * @ingroup rcu_issue
 */
#define READ_RDO_MOD                       (0x300000 | FEESVR_CMD_RCU2)

/**
 * write the TTC Control register.
 * <b>Note:</b>The ID register is 24 bit wide.
 * parameter: ignored <br>
 * payload: 32bit data <br>
 * result: none <br>
 * <b>Note:</b> defined in command set version 2 <br>
 * @ingroup rcu_issue
 */
#define WRITE_TTCCONTROL                   (0x310000 | FEESVR_CMD_RCU2)

/**
 * read the TTC Control register.
 * parameter: ignored <br>
 * payload: none <br>
 * result: 32 bit register content <br>
 * <b>Note:</b> defined in command set version 7 <br>
 * @ingroup rcu_issue
 */
#define READ_TTCCONTROL                    (0x320000 | FEESVR_CMD_RCU2)

/**
 * write the TTC ROI Config1 register.
 * <b>Note:</b>The ID register is 18 bit wide.
 * parameter: ignored <br>
 * payload: 32bit data <br>
 * result: none <br>
 * <b>Note:</b> defined in command set version 2 <br>
 * @ingroup rcu_issue
 */
#define WRITE_TTC_ROICONFIG1               (0x330000 | FEESVR_CMD_RCU2)

/**
 * read the TTC ROI Config1 register.
 * parameter: ignored <br>
 * payload: none <br>
 * result: 32 bit register content <br>
 * <b>Note:</b> defined in command set version 7 <br>
 * @ingroup rcu_issue
 */
#define READ_TTC_ROICONFIG1                (0x340000 | FEESVR_CMD_RCU2)

/**
 * write the TTC ROI Config2 register.
 * <b>Note:</b>The ID register is 18 bit wide.
 * parameter: ignored <br>
 * payload: 32bit data <br>
 * result: none <br>
 * <b>Note:</b> defined in command set version 2 <br>
 * @ingroup rcu_issue
 */
#define WRITE_TTC_ROICONFIG2               (0x350000 | FEESVR_CMD_RCU2)

/**
 * read the TTC ROI Config2 register.
 * parameter: ignored <br>
 * payload: none <br>
 * result: 32 bit register content <br>
 * <b>Note:</b> defined in command set version 7 <br>
 * @ingroup rcu_issue
 */
#define READ_TTC_ROICONFIG2                (0x360000 | FEESVR_CMD_RCU2)

/**
 * write the TTC L2 Latency register.
 * <b>Note:</b>The ID register is 32 bit wide.
 * parameter: ignored <br>
 * payload: 32bit data <br>
 * result: none <br>
 * <b>Note:</b> defined in command set version 2 <br>
 * @ingroup rcu_issue
 */
#define WRITE_TTC_L2_LATENCY               (0x370000 | FEESVR_CMD_RCU2)

/**
 * read the TTC L2 Latency register.
 * parameter: ignored <br>
 * payload: none <br>
 * result: 32 bit register content <br>
 * <b>Note:</b> defined in command set version 7 <br>
 * @ingroup rcu_issue
 */
#define READ_TTC_L2_LATENCY                (0x380000 | FEESVR_CMD_RCU2)

/**
 * write the TTC ROI Latency register.
 * <b>Note:</b>The ID register is 32 bit wide.
 * parameter: ignored <br>
 * payload: 32bit data <br>
 * result: none <br>
 * <b>Note:</b> defined in command set version 2 <br>
 * @ingroup rcu_issue
 */
#define WRITE_TTC_ROILATENCY               (0x390000 | FEESVR_CMD_RCU2)

/**
 * read the TTC ROI Latency register.
 * parameter: ignored <br>
 * payload: none <br>
 * result: 32 bit register content <br>
 * <b>Note:</b> defined in command set version 7 <br>
 * @ingroup rcu_issue
 */
#define READ_TTC_ROILATENCY                (0x400000 | FEESVR_CMD_RCU2)

/**
 * read the TTC PrePulseCounter register.
 * parameter: ignored <br>
 * payload: none <br>
 * result: 32 bit register content <br>
 * <b>Note:</b> defined in command set version 7 <br>
 * @ingroup rcu_issue
 */
#define READ_TTC_PREPULSECOUNTER           (0x410000 | FEESVR_CMD_RCU2)

/**
 * read the TTC BCID register.
 * parameter: ignored <br>
 * payload: none <br>
 * result: 32 bit register content <br>
 * <b>Note:</b> defined in command set version 7 <br>
 * @ingroup rcu_issue
 */
#define READ_TTC_BCIDLOCAL                 (0x420000 | FEESVR_CMD_RCU2)

/**
 * read the TTC L0 Counter register.
 * parameter: ignored <br>
 * payload: none <br>
 * result: 32 bit register content <br>
 * <b>Note:</b> defined in command set version 7 <br>
 * @ingroup rcu_issue
 */
#define READ_TTC_L0COUNTER                 (0x430000 | FEESVR_CMD_RCU2)

/**
 * read the TTC L1 Counter register.
 * parameter: ignored <br>
 * payload: none <br>
 * result: 32 bit register content <br>
 * <b>Note:</b> defined in command set version 7 <br>
 * @ingroup rcu_issue
 */
#define READ_TTC_L1COUNTER                 (0x440000 | FEESVR_CMD_RCU2)

/**
 * read the TTC L1 MsgCounter register.
 * parameter: ignored <br>
 * payload: none <br>
 * result: 32 bit register content <br>
 * <b>Note:</b> defined in command set version 7 <br>
 * @ingroup rcu_issue
 */
#define READ_TTC_L1MSGCOUNTER              (0x450000 | FEESVR_CMD_RCU2)

/**
 * read the TTC L2 accept register.
 * parameter: ignored <br>
 * payload: none <br>
 * result: 32 bit register content <br>
 * <b>Note:</b> defined in command set version 7 <br>
 * @ingroup rcu_issue
 */
#define READ_TTC_L2ACOUNTER                (0x460000 | FEESVR_CMD_RCU2)

/**
 * read the TTC L2 reject register.
 * parameter: ignored <br>
 * payload: none <br>
 * result: 32 bit register content <br>
 * <b>Note:</b> defined in command set version 7 <br>
 * @ingroup rcu_issue
 */
#define READ_TTC_L2RCOUNTER                (0x470000 | FEESVR_CMD_RCU2)

/**
 * read the TTC module ROI register.
 * parameter: ignored <br>
 * payload: none <br>
 * result: 32 bit register content <br>
 * <b>Note:</b> defined in command set version 7 <br>
 * @ingroup rcu_issue
 */
#define READ_TTC_ROICOUNTER                (0x480000 | FEESVR_CMD_RCU2)

/**
 * read the ALTRO CFG1 register.
 * parameter: ignored <br>
 * payload: none <br>
 * result: 32 bit register content <br>
 * <b>Note:</b> defined in command set version 7 <br>
 * @ingroup rcu_issue
 */
#define READ_ALTROCFG1                     (0x490000 | FEESVR_CMD_RCU2)

/**
 * write the ALTRO CFG1 register of the RCU2.
 * <b>Note:</b>The ID register is 16 bit wide.
 * parameter: ignored <br>
 * payload: 32bit data <br>
 * result: none <br>
 * <b>Note:</b> defined in command set version 2 <br>
 * @ingroup rcu_issue
 */
#define WRITE_ALTROCFG1                    (0x4A0000 | FEESVR_CMD_RCU2)

/**
 * read the ALTRO CFG2 register.
 * parameter: ignored <br>
 * payload: none <br>
 * result: 32 bit register content <br>
 * <b>Note:</b> defined in command set version 7 <br>
 * @ingroup rcu_issue
 */
#define READ_ALTROCFG2                     (0x4B0000 | FEESVR_CMD_RCU2)

/**
 * write the ALTRO CFG2 register of the RCU2.
 * <b>Note:</b>The ID register is 16 bit wide.
 * parameter: ignored <br>
 * payload: 32bit data <br>
 * result: none <br>
 * <b>Note:</b> defined in command set version 2 <br>
 * @ingroup rcu_issue
 */
#define WRITE_ALTROCFG2                    (0x4C0000 | FEESVR_CMD_RCU2)

/**
 * read the RCU2 ID register.
 * parameter: ignored <br>
 * payload: none <br>
 * result: 32 bit register content <br>
 * <b>Note:</b> defined in command set version 7 <br>
 * @ingroup rcu_issue
 */
#define READ_RCU2ID                         (0x4D0000 | FEESVR_CMD_RCU2)

/**
 * write the ID register of the RCU2.
 * <b>Note:</b>The ID register is 9 bit wide.
 * parameter: ignored <br>
 * payload: 32bit data <br>
 * result: none <br>
 * <b>Note:</b> defined in command set version 2 <br>
 * @ingroup rcu_issue
 */
#define WRITE_RCU2ID                        (0x4E0000 | FEESVR_CMD_RCU2)

/**
 * read from ALTRO Hitlist memory.
 * parameter: number of 32 bit words to read <br>
 * payload: 0 <br>
 * <b>Note:</b> defined in command set version 7 <br>
 * @ingroup rcu_issue
 */
#define READ_ALTRO_HLMEM                   (0x4F0000 | FEESVR_CMD_RCU2)

/**
 * write to ALTRO Hitlist memory.
 * parameter: number of 32 bit words in the payload <br>
 * payload: 32 bit data words <br>
 * <b>Note:</b> defined in command set version 7 <br>
 * @ingroup rcu_issue
 */
#define WRITE_ALTRO_HLMEM                  (0x500000 | FEESVR_CMD_RCU2)

/**
 * write to ALTRO pedestal memory.
 * parameter: channel address (as in ALTRO manual) <br>
 * payload: 1024 10-bit values truncated into 342 32-bit word) (3 in each word)<br>
 * @ingroup rcu_issue
 */
#define RCU2_WRITE_ALTRO_PEDMEM             (0x510000 | FEESVR_CMD_RCU2)

//No longer supported
//#define INIT_BUSY_BOX                    (0xF00000 | FEESVR_CMD_RCU2)

/**
 * Store Altro instructions in FeeServer memory. Use RCU2_ALTRO_INSTRUCTION_EXECUTE
 * to actually write to Altros. Command assume payload is in pairs of 32-bit words,
 * where first word is <address> and second is  <data>.
 * parameter: number of 32 bit words in the payload (should be even number!)<br>
 * payload: 32 bit word address/data pairs <br>
 * @ingroup rcu_issue
 */
#define RCU2_ALTRO_INSTRUCTION_STORE        (0x520000 | FEESVR_CMD_RCU2)

/**
 * Write Altro instructions stored by RCU2_ALTRO_INSTRUCTION_STORE to Altros.
 * parameter: none <br>
 * payload: none <br>
 * @ingroup rcu_issue
 */
#define RCU2_ALTRO_INSTRUCTION_EXECUTE      (0x530000 | FEESVR_CMD_RCU2)

/**
 * Clear Altro instructions stored by RCU2_ALTRO_INSTRUCTION_STORE.
 * parameter: none <br>
 * payload: none <br>
 * @ingroup rcu_issue
 */
#define RCU2_ALTRO_INSTRUCTION_CLEAR        (0x540000 | FEESVR_CMD_RCU2)

/**
 * Copy Altro instructions stored by RCU2_ALTRO_INSTRUCTION_STORE to file.
 * parameter: none <br>
 * payload: none <br>
 * @ingroup rcu_issue
 */
#define RCU2_ALTRO_INSTRUCTION_WRITE_FILE   (0x550000 | FEESVR_CMD_RCU2)

/**
 * Copy Altro instructions stored in file by RCU2_ALTRO_INSTRUCTION_WRITE_FILE back to map.
 * parameter: none <br>
 * payload: none <br>
 * @ingroup rcu_issue
 */
#define RCU2_ALTRO_INSTRUCTION_READ_FILE    (0x560000 | FEESVR_CMD_RCU2)

/**
 * Turns off the front end cards for 4.0uSec. After the 4.0uSec the front end cards will be turned on same as the original configuration of active front end card list.
 * parameter: none <br>
 * payload: none <br>
 * @ingroup rcu_issue
 */
#define RCU2_CONFGFEC                       (0x570000 | FEESVR_CMD_RCU2)

/**
 * Enables the RCU2 to switch off the FEC branch in case the interrupt signals
 * parameter: ignored <br>
 * payload: value to be written to register, i.e. 0, 1, 2 or 3 <br>
 * @ingroup rcu_issue
 */
#define RCU2_WRITE_EN_INT_BA                (0x580000 | FEESVR_CMD_RCU2)

/**
 * Reads the register that configures the RCU2 to switch off the FEC branch in case the interrupt signals
 * parameter: ignored <br>
 * payload: none <br>
 * @ingroup rcu_issue
 */
#define RCU2_READ_EN_INT_BA                 (0x590000 | FEESVR_CMD_RCU2)

/**
 * Store register configuration in FeeServer memory. Use REG_CFG_EXECUTE
 * to actually write to hardware. Command assume payload is in pairs of 32-bit words,
 * where first word is <address> and second is  <data>.
 * parameter: number of 32 bit words in the payload (should be even number!)<br>
 * payload: 32 bit word address/data pairs <br>
 * @ingroup rcu_issue
 */
#define REG_CFG_STORE                      (0x5A0000 | FEESVR_CMD_RCU2)

/**
 * Write register configuration stored by REG_CFG_STORE to hardware.
 * parameter: none <br>
 * payload: none <br>
 * @ingroup rcu_issue
 */
#define REG_CFG_EXECUTE                    (0x5B0000 | FEESVR_CMD_RCU2)

/**
 * Write register configuration stored by REG_CFG_STORE to hardware for single register.
 * parameter: none <br>
 * payload: 32 bit word register address to execute <br>
 * @ingroup rcu_issue
 */
#define REG_CFG_EXECUTE_SINGLE             (0x5C0000 | FEESVR_CMD_RCU2)

/**
 * Clear register configuration stored by REG_CFG_STORE.
 * parameter: none <br>
 * payload: none <br>
 * @ingroup rcu_issue
 */
#define REG_CFG_CLEAR                      (0x5D0000 | FEESVR_CMD_RCU2)

/**
 * Copy register confiuration stored by REG_CFG_STORE to file.
 * parameter: none <br>
 * payload: none <br>
 * @ingroup rcu_issue
 */
#define REG_CFG_WRITE_FILE                 (0x5E0000 | FEESVR_CMD_RCU2)

/**
 * Copy register configuration stored in file by REG_CFG_WRITE_FILE back to map.
 * parameter: none <br>
 * payload: none <br>
 * @ingroup rcu_issue
 */
#define REG_CFG_READ_FILE                  (0x5F0000 | FEESVR_CMD_RCU2)

/**
 * Set the file name to be used for storing IMEM instructions.
 * parameter:  number of 32-bit words. <br>
 * payload: 0-terminated string, has to be <parameter> 32-bit words. <br>
 * @ingroup rcu_issue
 */
#define RCU2_ALTRO_INSTRUCTION_FILE_NAME    (0x600000 | FEESVR_CMD_RCU2)

/**
 * Set the file name to be used for storing REG_CFG instructions.
 * parameter:  number of 32-bit words. <br>
 * payload: 0-terminated string, has to be <parameter> 32-bit words. <br>
 * @ingroup rcu_issue
 */
#define REG_CFG_FILE_NAME                  (0x610000 | FEESVR_CMD_RCU2)

/**
 * Reloads the RCU2 driver.
 * parameter:  none. <br>
 * payload: none. <br>
 * @ingroup rcu_issue
 */
#define RCU2_DRIVER_RELOAD                  (0x620000 | FEESVR_CMD_RCU2)

/**
 * Unloads the RCU2 driver.
 * parameter:  none. <br>
 * payload: none. <br>
 * @ingroup rcu_issue
 */
#define RCU2_DRIVER_UNLOAD                  (0x630000 | FEESVR_CMD_RCU2)

/**
 * Loads the RCU2 driver.
 * parameter:  none. <br>
 * payload: none. <br>
 * @ingroup rcu_issue
 */
#define RCU2_DRIVER_LOAD                    (0x640000 | FEESVR_CMD_RCU2)

  /**
   * Rcu testing error state
   * @ingroup rcu_issue
   */
#define RCU2_TEST_ERROR                     (0x650000 | FEESVR_CMD_RCU2CONF)

/****************************************************************************************/

/**
 * @name Group 4: command set for the RCU2 configuration.
 * The command group is implemented by DEV_ACTEL.
 * @ingroup rcu_issue
 */

/**
 * ID of group 4.
 * 0xf4xxxxxx
 * @ingroup rcu_issue
 */
#define FEESVR_CMD_RCU2CONF               (0x04000000 | FEESERVER_CMD)

/**
 * write a configuration to the RCU2 FPGA
 * @ingroup rcu_issue
 */
#define RCU2_WRITE_FPGA_CONF                (0x010000 | FEESVR_CMD_RCU2CONF)

/**
 * read the configuration of the RCU2 FPGA
 * @ingroup rcu_issue
 */
#define RCU2_READ_FPGA_CONF                 (0x020000 | FEESVR_CMD_RCU2CONF)

/**
 * write the configuration data to the Flash
 * @ingroup rcu_issue
 */
#define RCU2_WRITE_FLASH                    (0x030000 | FEESVR_CMD_RCU2CONF)

/**
 * Let the Actel program the Xilinx FPGA from the Flash memory.
 * @ingroup rcu_issue
 */
#define RCU2_INIT_CONF                      (0x040000 | FEESVR_CMD_RCU2CONF)

/**
 * erase the Flash memory
 * @ingroup rcu_issue
 */
#define RCU2_ERASE_FLASH                    (0x050000 | FEESVR_CMD_RCU2CONF)

/**
 * Do an initial configuration with the given data
 * @ingroup rcu_issue
 */
#define RCU2_WRITE_INITCONF                 (0x060000 | FEESVR_CMD_RCU2CONF)

/**
 * Do a scrubbing with the provided scrubbing data
 * @ingroup rcu_issue
 */
#define RCU2_WRITE_SCRUBBING                (0x070000 | FEESVR_CMD_RCU2CONF)

  /**
   * Enable Scrubing
   * @ingroup rcu_issue
   */
#define RCU2_WRITE_FRAMES                   (0x080000 | FEESVR_CMD_RCU2CONF)

  /**
   * Enable the ACTEL automatic reconfiguration feature
   * parameter: The value to enable/disable automatic power up configuration (0x0 to set or 0x1to remove )
   * @ingroup rcu_issue
   */
#define RCU2_SET_AUTOCONF                   (0x0A0000 | FEESVR_CMD_RCU2CONF)

  /**
   * Erase the Xilinx FPGA, if an old RCU2 FW is used (xxxx06),
   * this is necessary to access the Flash.
   * Sending this command also stops the automatic power up configuration.
   * @ingroup rcu_issue
   */
#define RCU2_DELETE_XILINX                  (0x0B0000 | FEESVR_CMD_RCU2CONF)

  /**
   * Set the OldMode register (0xBF02) to "0x01D" if the parameter 0x1 is given,
   * 0x0 as parameter removes the OldMode.
   * Has to be done when an old RCU2 firmware is used (xxxx06).
   * @ingroup rcu_issue
   */
#define RCU2_SET_OLDMODE                    (0x0C0000 | FEESVR_CMD_RCU2CONF)

  /**
   * Clear the Actel Errorregister 0xb101
   * @ingroup rcu_issue
   */
#define ACTEL_CLEAR_ERRORREG               (0x0D0000 | FEESVR_CMD_RCU2CONF)

  /**
   * Clear the Actel Statusregisters
   * Registers cleared are: Status (0xb100), ErrorCount (0xb104), ErrorFrameNr (0xb105), FrameNr (0xb106), Cycles (0xb107)
   * @ingroup rcu_issue
   */
#define ACTEL_CLEAR_STATUSREGS             (0x0E0000 | FEESVR_CMD_RCU2CONF)

#define ACTEL_RESET                        (0x0F0000 | FEESVR_CMD_RCU2CONF)

  /**
   * Actel testing error state
   * @ingroup rcu_issue
   */
#define ACTEL_TEST_ERROR                   (0x100000 | FEESVR_CMD_RCU2CONF)

/****************************************************************************************/

/**
 * commandset RCU22.
 * @ingroup rcu_issue
 */
#define FEESVR_CMD_RCU2_2                  (0x05000000 | FEESERVER_CMD)
#define EXAMPLE_COMMAND_1                (0x010000 | FEESVR_CMD_RCU2_2)
#define EXAMPLE_COMMAND_2                (0x020000 | FEESVR_CMD_RCU2_2)
#define RCU2_READ_STATUS_REG             (0x030000 | FEESVR_CMD_RCU2_2)
#define RCU2_WRITE_TO_REG                (0x040000 | FEESVR_CMD_RCU2_2)

/****************************************************************************************/

/**
 * commandset UNUSED 6.
 * @ingroup rcu_issue
 */
#define FEESVR_CMD_UNUSED6               (0x06000000 | FEESERVER_CMD)

/****************************************************************************************/

/**
 * @name Group 7: shell execution.
 * 
 * @ingroup rcu_issue
 */

/**
 * ID of group 7.
 * 0xf7xxxxxx
 * @ingroup rcu_issue
 */
#define FEESVR_CMD_SHELL                 (0x07000000 | FEESERVER_CMD)

/**
 * Execute a script/program on the DCS board.
 * parameter: length of the payload, rounded up to multiple of 4 bytes.<br>
 * payload: char buffer containing shell command and arguments.<br>
 * <br>
 * @ingroup rcu_issue
 */
#define FEESRV_EXECUTE_PGM                 (0x010000 | FEESVR_CMD_SHELL)

/**
 * Send a shell script/binary down and execute it.
 * The payload will be treated as shell script/binary and executed.
 * parameter: length of the payload, rounded up to multiple of 4 bytes.<br>
 * payload: script/binary to be executed.<br>
 * <br>
 * @ingroup rcu_issue
 */
#define FEESRV_EXECUTE_SCRIPT              (0x020000 | FEESVR_CMD_SHELL)
#define FEESRV_BINARY_PGM                  (0x030000 | FEESVR_CMD_SHELL)

/**
 * Send an rcu-sh script down and execute it through rcu-sh interpreter.
 * parameter: length of the payload, rounded up to multiple of 4 bytes.<br>
 * payload: char buffer containing the rcu-sh script.<br>
 * @ingroup rcu_issue
 */
#define FEESRV_RCU2SH_SCRIPT                (0x040000 | FEESVR_CMD_SHELL)

/****************************************************************************************/

/**
 * commandset UNUSED 8.
 * @ingroup rcu_issue
 */
#define FEESVR_CMD_UNUSED8               (0x08000000 | FEESERVER_CMD)

/****************************************************************************************/

/**
 * command set for the CALIBRATION PULSER CE.
 * @ingroup rcu_issue
 */
#define FEESVR_CMD_CPULSER               (0x09000000 | FEESERVER_CMD)

#define CPULSER_WRITE_TTC_CONTROL          (0x010000 | FEESVR_CMD_CPULSER)
#define CPULSER_TOGGLE_TTC_RESET           (0x020000 | FEESVR_CMD_CPULSER)
#define CPULSER_WRITE_TTC_ROICONFIG1       (0x030000 | FEESVR_CMD_CPULSER)
#define CPULSER_WRITE_TTC_ROICONFIG2       (0x040000 | FEESVR_CMD_CPULSER)
#define CPULSER_TOGGLE_TTC_RESETCOUNTER    (0x050000 | FEESVR_CMD_CPULSER)
#define CPULSER_TOGGLE_TTC_ISSUETESTMODE   (0x060000 | FEESVR_CMD_CPULSER)
#define CPULSER_WRITE_TTC_L1LATENCY        (0x070000 | FEESVR_CMD_CPULSER)
#define CPULSER_WRITE_TTC_L2LATENCY        (0x080000 | FEESVR_CMD_CPULSER)
#define CPULSER_WRITE_TTC_ROILATENCY       (0x090000 | FEESVR_CMD_CPULSER)
#define CPULSER_WRITE_TTC_L1MSGLATENCY     (0x0A0000 | FEESVR_CMD_CPULSER)

#define CPULSER_WRITE_CONTROL              (0x0B0000 | FEESVR_CMD_CPULSER)
#define CPULSER_WRITE_FINEDELAY            (0x0C0000 | FEESVR_CMD_CPULSER)
#define CPULSER_WRITE_COARSEDELAY          (0x0D0000 | FEESVR_CMD_CPULSER)
#define CPULSER_WRITE_CHANNEL0             (0x0E0000 | FEESVR_CMD_CPULSER)
#define CPULSER_WRITE_CHANNEL1             (0x0F0000 | FEESVR_CMD_CPULSER)
#define CPULSER_WRITE_AMPLITUDE            (0x100000 | FEESVR_CMD_CPULSER)
#define CPULSER_WRITE_PULSESHAPE           (0x110000 | FEESVR_CMD_CPULSER)
#define CPULSER_WRITE_PULSERCONF           (0x120000 | FEESVR_CMD_CPULSER)
#define CPULSER_WRITE_INSTRUCTIONREG       (0x130000 | FEESVR_CMD_CPULSER)
#define CPULSER_TOGGLE_REMOTERESET         (0x140000 | FEESVR_CMD_CPULSER)
#define CPULSER_TOGGLE_ALLCHANNELSON       (0x150000 | FEESVR_CMD_CPULSER)
#define CPULSER_TOGGLE_ALLCHANNELSOFF      (0x160000 | FEESVR_CMD_CPULSER)
#define CPULSER_TOGGLE_REMOTE              (0x170000 | FEESVR_CMD_CPULSER)
#define CPULSER_TOGGLE_LOCAL               (0x180000 | FEESVR_CMD_CPULSER)
#define CPULSER_TOGGLE_GLOBALRESET         (0x190000 | FEESVR_CMD_CPULSER)
#define CPULSER_TOGGLE_TESTTRIGGER         (0x1A0000 | FEESVR_CMD_CPULSER)
#define CPULSER_ARM_SYNC                   (0x1B0000 | FEESVR_CMD_CPULSER)



/****************************************************************************************/

/**
 * commandset UNUSED A.
 * @ingroup rcu_issue
 */
#define FEESVR_CMD_UNUSEDA               (0x0a000000 | FEESERVER_CMD)

/****************************************************************************************/

/**
 * command set for the PHOS CE.
 * @ingroup rcu_issue
 */
#define FEESVR_CMD_PHOS                  (0x0b000000 | FEESERVER_CMD)

/**
 * Set APD values for FEC.
 * parameter: FEC number 0-31 <br>
 * payload: 32 __32 values contning APD values.
 * Note: Values must be encoded with Hamming code.<br>
 * <br>
 * @ingroup rcu_issue
 */
#define PHOS_APD_INIT                      (0x010000 | FEESVR_CMD_PHOS)

/****************************************************************************************/

/**
 * command set for the BUSYBOX CE.
 * @ingroup rcu_issue
 */
#define FEESVR_CMD_BB                    (0x0c000000 | FEESERVER_CMD)

#define BB_INIT                            (0x010000 | FEESVR_CMD_BB)
#define INIT_BUSY_BOX                      (0x010000 | FEESVR_CMD_BB) //Depricated
#define BB_WRITE_STATUS                    (0x020000 | FEESVR_CMD_BB)
#define BB_WRITE_RXMF                      (0x030000 | FEESVR_CMD_BB)
#define BB_WRITE_RRTO                      (0x040000 | FEESVR_CMD_BB)
#define BB_WRITE_FM                        (0x050000 | FEESVR_CMD_BB)
#define BB_WRITE_FSMH                      (0x060000 | FEESVR_CMD_BB)
#define BB_WRITE_FEEBA                     (0x070000 | FEESVR_CMD_BB)
#define BB_WRITE_L0TTO                     (0x080000 | FEESVR_CMD_BB)
#define BB_WRITE_L0_TIMEOUT                (0x080000 | FEESVR_CMD_BB) //Depricated
#define BB_WRITE_TX                        (0x090000 | FEESVR_CMD_BB)
#define BB_WRITE_TTC_CONTROL0              (0x0a0000 | FEESVR_CMD_BB)
#define BB_WRITE_TTC_CONTROL1              (0x0b0000 | FEESVR_CMD_BB)
#define BB_WRITE_TTC_TMR                   (0x0c0000 | FEESVR_CMD_BB)
#define BB_WRITE_TTC_L0L                   (0x0d0000 | FEESVR_CMD_BB)
#define BB_WRITE_TTC_BCIDO                 (0x0e0000 | FEESVR_CMD_BB)
#define BB_FIRMWARE_RESET                  (0x0f0000 | FEESVR_CMD_BB)
#define BB_WRITE_TTC_L1L                   (0x100000 | FEESVR_CMD_BB)
#define BB_WRITE_TRIGMODE                  (0x110000 | FEESVR_CMD_BB)

/****************************************************************************************/

/**
 * command set for the LASER CE.
 * @ingroup rcu_issue
 */
#define FEESVR_CMD_LASER                 (0x0D000000 | FEESERVER_CMD)

#define LASER_WRITE_MODEFREQVAL            (0x010000 | FEESVR_CMD_LASER)
#define LASER_WRITE_1_FLASHSTARTTIME       (0x020000 | FEESVR_CMD_LASER)
#define LASER_WRITE_1_FLASHENDTIME         (0x030000 | FEESVR_CMD_LASER)
#define LASER_WRITE_2_FLASHSTARTTIME       (0x040000 | FEESVR_CMD_LASER)
#define LASER_WRITE_2_FLASHENDTIME         (0x050000 | FEESVR_CMD_LASER)
#define LASER_WRITE_L0REQUESTTIME          (0x070000 | FEESVR_CMD_LASER)
#define LASER_WRITE_L0RETURNWINDOWSTART    (0x080000 | FEESVR_CMD_LASER)
#define LASER_WRITE_L0RETURNWINDOWSTOP     (0x090000 | FEESVR_CMD_LASER)
#define LASER_WRITE_1_QSWITCHSTART         (0x0A0000 | FEESVR_CMD_LASER)
#define LASER_WRITE_1_QSWITCHSTOP          (0x0B0000 | FEESVR_CMD_LASER)
#define LASER_WRITE_2_QSWITCHSTART         (0x330000 | FEESVR_CMD_LASER)
#define LASER_WRITE_2_QSWITCHSTOP          (0x340000 | FEESVR_CMD_LASER)
#define LASER_WRITE_RUNMODE                (0x0C0000 | FEESVR_CMD_LASER)
#define LASER_WRITE_SAMPLECLOCKDIVIDER     (0x0D0000 | FEESVR_CMD_LASER)
#define LASER_WRITE_TRIGGERCONFIGURATION1  (0x0E0000 | FEESVR_CMD_LASER)
#define LASER_WRITE_TRIGGERCONFIGURATION2  (0x0F0000 | FEESVR_CMD_LASER)
#define LASER_WRITE_TTC_CONTROL            (0x110000 | FEESVR_CMD_LASER)
#define LASER_TOGGLE_TTC_RESET             (0x120000 | FEESVR_CMD_LASER)
#define LASER_WRITE_TTC_ROICONFIG1         (0x130000 | FEESVR_CMD_LASER)
#define LASER_WRITE_TTC_ROICONFIG2         (0x140000 | FEESVR_CMD_LASER)
#define LASER_TOGGLE_TTC_RESETCOUNTER      (0x150000 | FEESVR_CMD_LASER)
#define LASER_TOGGLE_TTC_ISSUETESTMODE     (0x160000 | FEESVR_CMD_LASER)
#define LASER_WRITE_TTC_L1LATENCY          (0x170000 | FEESVR_CMD_LASER)
#define LASER_WRITE_TTC_L2LATENCY          (0x180000 | FEESVR_CMD_LASER)
#define LASER_WRITE_TTC_ROILATENCY         (0x190000 | FEESVR_CMD_LASER)
#define LASER_WRITE_TTC_L1MSGLATENCY       (0x1A0000 | FEESVR_CMD_LASER)

#define LASER_SET_1_FLASHSTARTTIME         (0x1B0000 | FEESVR_CMD_LASER)
#define LASER_SET_1_FLASHENDTIME           (0x1C0000 | FEESVR_CMD_LASER)
#define LASER_SET_2_FLASHSTARTTIME         (0x1D0000 | FEESVR_CMD_LASER)
#define LASER_SET_2_FLASHENDTIME           (0x1E0000 | FEESVR_CMD_LASER)
#define LASER_SET_L0REQUESTTIME            (0x200000 | FEESVR_CMD_LASER)
#define LASER_SET_L0RETURNWINDOWSTART      (0x210000 | FEESVR_CMD_LASER)
#define LASER_SET_L0RETURNWINDOWEND        (0x220000 | FEESVR_CMD_LASER)
#define LASER_SET_1_QSWITCHSTART           (0x230000 | FEESVR_CMD_LASER)
#define LASER_SET_1_QSWITCHEND             (0x240000 | FEESVR_CMD_LASER)
#define LASER_SET_2_QSWITCHSTART           (0x350000 | FEESVR_CMD_LASER)
#define LASER_SET_2_QSWITCHEND       (0x360000 | FEESVR_CMD_LASER)
#define LASER_CLEAR_COUNTERS               (0x250000 | FEESVR_CMD_LASER)

#define LASER_ENABLE_WARMUP                (0x260000 | FEESVR_CMD_LASER)
#define LASER_DISABLE_WARMUP               (0x270000 | FEESVR_CMD_LASER)
#define LASER_ENABLE_2                     (0x280000 | FEESVR_CMD_LASER)
#define LASER_DISABLE_2                    (0x290000 | FEESVR_CMD_LASER)
#define LASER_ENABLE_1                     (0x2A0000 | FEESVR_CMD_LASER)
#define LASER_DISABLE_1                    (0x2B0000 | FEESVR_CMD_LASER)
#define LASER_CLEAR_SOD                    (0x2C0000 | FEESVR_CMD_LASER)
#define LASER_ARM_SYNC                     (0x2D0000 | FEESVR_CMD_LASER)

#define LASER_WRITE_MAXNUMBEROFFLASHEVENTS (0x300000 | FEESVR_CMD_LASER)
#define LASER_WRITE_NUMBEROFEVENTSINBURST  (0x310000 | FEESVR_CMD_LASER)
#define LASER_WRITE_ENABLES                (0x320000 | FEESVR_CMD_LASER)

/****************************************************************************************/

/**
 * command set for the TOR CE.
 * @ingroup rcu_issue
 */
#define FEESVR_CMD_TOR                   (0x0e000000 | FEESERVER_CMD)

#define WRITE_TRIG0_PROGDELAY              (0x010000 | FEESVR_CMD_TOR)
#define WRITE_TRIG0_PROGRATEHIGH           (0x020000 | FEESVR_CMD_TOR)
#define WRITE_TRIG0_PROGRATELOW            (0x030000 | FEESVR_CMD_TOR)
#define WRITE_TRIG0_SIGNATURE              (0x040000 | FEESVR_CMD_TOR)
#define WRITE_TRIG0_OPTIONCODE             (0x050000 | FEESVR_CMD_TOR)
#define WRITE_TRIG0_MESSAGEHEADER          (0x060000 | FEESVR_CMD_TOR)

#define WRITE_TRIG1_PROGDELAY              (0x070000 | FEESVR_CMD_TOR)
#define WRITE_TRIG1_PROGRATEHIGH           (0x080000 | FEESVR_CMD_TOR)
#define WRITE_TRIG1_PROGRATELOW            (0x090000 | FEESVR_CMD_TOR)
#define WRITE_TRIG1_SIGNATURE              (0x0A0000 | FEESVR_CMD_TOR)
#define WRITE_TRIG1_OPTIONCODE             (0x0B0000 | FEESVR_CMD_TOR)
#define WRITE_TRIG1_MESSAGEHEADER          (0x0C0000 | FEESVR_CMD_TOR)

#define WRITE_TRIG1M_PROGDELAY             (0x0D0000 | FEESVR_CMD_TOR)
#define WRITE_TRIG1M_PROGRATEHIGH          (0x0E0000 | FEESVR_CMD_TOR)
#define WRITE_TRIG1M_PROGRATELOW           (0x0F0000 | FEESVR_CMD_TOR)
#define WRITE_TRIG1M_SIGNATURE             (0x100000 | FEESVR_CMD_TOR)
#define WRITE_TRIG1M_OPTIONCODE            (0x110000 | FEESVR_CMD_TOR)
#define WRITE_TRIG1M_MESSAGEHEADER         (0x120000 | FEESVR_CMD_TOR)

#define WRITE_TRIG1H_PROGDELAY             (0x130000 | FEESVR_CMD_TOR)
#define WRITE_TRIG1H_PROGRATEHIGH          (0x140000 | FEESVR_CMD_TOR)
#define WRITE_TRIG1H_PROGRATELOW           (0x150000 | FEESVR_CMD_TOR)
#define WRITE_TRIG1H_SIGNATURE             (0x160000 | FEESVR_CMD_TOR)
#define WRITE_TRIG1H_OPTIONCODE            (0x170000 | FEESVR_CMD_TOR)
#define WRITE_TRIG1H_MESSAGEHEADER         (0x180000 | FEESVR_CMD_TOR)

#define WRITE_L1L_THRESHOLD                (0x190000 | FEESVR_CMD_TOR)
#define WRITE_L1M_THRESHOLD                (0x1A0000 | FEESVR_CMD_TOR)
#define WRITE_L1H_THRESHOLD                (0x1B0000 | FEESVR_CMD_TOR)
#define WRITE_L0TRIGGERMASK_12             (0x1C0000 | FEESVR_CMD_TOR)
#define WRITE_L0TRIGGERMASK_34             (0x1D0000 | FEESVR_CMD_TOR)
#define WRITE_L0TRIGGERMASK_5              (0x1E0000 | FEESVR_CMD_TOR)
#define WRITE_CTRL_RESERVE_R               (0x1F0000 | FEESVR_CMD_TOR)

/****************************************************************************************/

/**
 * command set for the GATE PULSER CE.
 * @ingroup rcu_issue
 */
#define FEESVR_CMD_GPULSER               (0x0f000000 | FEESERVER_CMD)

#define GPULSER_WRITE_TTC_CONTROL          (0x010000 | FEESVR_CMD_GPULSER)
#define GPULSER_TOGGLE_TTC_RESET           (0x020000 | FEESVR_CMD_GPULSER)
#define GPULSER_WRITE_TTC_ROICONFIG1       (0x030000 | FEESVR_CMD_GPULSER)
#define GPULSER_WRITE_TTC_ROICONFIG2       (0x040000 | FEESVR_CMD_GPULSER)
#define GPULSER_TOGGLE_TTC_RESETCOUNTER    (0x050000 | FEESVR_CMD_GPULSER)
#define GPULSER_TOGGLE_TTC_ISSUETESTMODE   (0x060000 | FEESVR_CMD_GPULSER)
#define GPULSER_WRITE_TTC_L1LATENCY        (0x070000 | FEESVR_CMD_GPULSER)
#define GPULSER_WRITE_TTC_L2LATENCY        (0x080000 | FEESVR_CMD_GPULSER)
#define GPULSER_WRITE_TTC_ROILATENCY       (0x090000 | FEESVR_CMD_GPULSER)
#define GPULSER_WRITE_TTC_L1MSGLATENCY     (0x0A0000 | FEESVR_CMD_GPULSER)
#define GPULSER_TOGGLE_RESET               (0x0B0000 | FEESVR_CMD_GPULSER)
#define GPULSER_TOGGLE_GLOBAL_RESET        (0x0C0000 | FEESVR_CMD_GPULSER)
#define GPULSER_WRITE_CONFREG              (0x0D0000 | FEESVR_CMD_GPULSER)
#define GPULSER_ARM_SYNC                   (0x0E0000 | FEESVR_CMD_GPULSER)
#define GPULSER_WRITE_DIVRATIO             (0x0F0000 | FEESVR_CMD_GPULSER)
#define GPULSER_WRITE_MODE                 (0x100000 | FEESVR_CMD_GPULSER)
#define GPULSER_WRITE_PULSEWIDTH           (0x110000 | FEESVR_CMD_GPULSER)
#define GPULSER_WRITE_MASKCRATE1           (0x120000 | FEESVR_CMD_GPULSER)
#define GPULSER_WRITE_MASKCRATE2           (0x130000 | FEESVR_CMD_GPULSER)
#define GPULSER_WRITE_MASKCRATE3           (0x140000 | FEESVR_CMD_GPULSER)
#define GPULSER_WRITE_MASKCRATE4           (0x150000 | FEESVR_CMD_GPULSER)
#define GPULSER_WRITE_MASKCRATE5           (0x160000 | FEESVR_CMD_GPULSER)
#define GPULSER_WRITE_MASKCRATE6           (0x170000 | FEESVR_CMD_GPULSER)

/*******************************************************************************************/

#endif //__RCU2_ISSUE_H

