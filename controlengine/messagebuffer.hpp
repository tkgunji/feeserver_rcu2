#ifndef __MESSAGEBUFFER_HPP
#define __MESSAGEBUFFER_HPP
#include <map>
#include <stdint.h>

class DevMsgbuffer{
public:
  DevMsgbuffer();
  virtual ~DevMsgbuffer();
  /**
   * Write a single location (32bit word).
   * Wrapper to dcscMsgBuffer method.
   * Overloaded by the simulation class.
   * @param address   16 bit address in memory space
   * @param data      data word
   * @return
   */
  virtual int SingleI2CWrite(uint32_t base, uint32_t address, uint32_t pData, uint32_t mode=1)=0;
  virtual int SingleWrite(uint32_t address, uint32_t data, uint32_t mode=1)=0;
  /**
   * Read a single location (32bit word).
   * Wrapper to dcscMsgBuffer method.|
   * Overloaded by the simulation class.
   * @param address   16 bit address in memory space
   * @param pData     buffer to receive the data
   * @return
   */
  virtual int SingleI2CRead(uint32_t base, uint32_t address, uint32_t* pData, uint32_t mode=1)=0;
  virtual int SingleRead(uint32_t address, uint32_t* pData, uint32_t mode=1)=0;
  /**
   * Write a number of 32bit words beginning at a location.
   * The function takes care for the size of the MIB and splits the operation if
   * the amount of data to write exceeds the MIB size.
   * The function expects data in little endian byte order.
   * Wrapper to dcscMsgBuffer method.
   * Overloaded by the simulation class.
   * @param address   16 bit address in memory space
   * @param pData     buffer containing the data
   * @param iSize     number of words to write
   * @param iDataSize size of one word in bytes, allowed 1 (8 bit), 2 (16 bit),3 (compressed 10 bit) 
   *                  or 4 (32 bit) -2 and -4 denote 16/32 bit words which get swapped
   * @return
   */
  virtual int MultipleWrite(uint32_t address, uint32_t* pData, int iSize, int iDataSize=4, uint32_t mode=1)=0;
  /**
   * Read a number of 32bit words beginning at a location.
   * Wrapper to dcscMsgBuffer method.
   * Overloaded by the simulation class.
   * @param address   16 bit address in memory space
   * @param iSize     number of words to read
   * @param pData     buffer to receive the data, the function expect it to be of suitable size
   *                  (i.e. iSize x wordsize)
   * @return          number of 32bit words which have been read, neg. error code if failed
   */
  virtual int MultipleRead(uint32_t address, int iSize,uint32_t* pData, uint32_t mode=1)=0;

  /**
  * Erase sectors of the RCU Flash memory
  * Wrapper to dcscMsgBuffer method.
  * @param startSec  the first sector to erase; if -1 erase all
  * @param stopSec   the last sector to erase
  * @return          0 success, neg. error code if failed
  */
  virtual int FlashErase(int startSec, int stopSec)=0;
  virtual int DriverReload()=0;
  virtual int DriverUnload()=0;
  virtual int DriverLoad()=0;
};

class DevMsgbufferDummy: public DevMsgbuffer{
public:
  DevMsgbufferDummy();
  virtual ~DevMsgbufferDummy();
  virtual int SingleWrite(uint32_t address, uint32_t data, uint32_t mode=1);
  virtual int SingleRead(uint32_t address, uint32_t* pData, uint32_t mode=1);
  virtual int SingleI2CWrite(uint32_t base, uint32_t address, uint32_t pData, uint32_t mode=1);
  virtual int SingleI2CRead(uint32_t base, uint32_t address, uint32_t* pData, uint32_t mode=1);
  virtual int MultipleWrite(uint32_t address, uint32_t* pData, int iSize, int iDataSize=4, uint32_t mode=1);
  virtual int MultipleRead(uint32_t address, int iSize,uint32_t* pData, uint32_t mode=1);
  virtual int FlashErase(int startSec, int stopSec);
  virtual int DriverReload();
  virtual int DriverUnload();
  virtual int DriverLoad();
protected:
  std::map<uint32_t,uint32_t> fMemory;
};
#endif //__MESSAGEBUFFER_HPP

