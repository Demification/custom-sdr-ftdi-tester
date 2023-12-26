#ifndef TI_FTDI_WRAPPER
#define TI_FTDI_WRAPPER

#include <ftd2xx.h>
#include <string>
#include <ostream>
#include<iostream>
#include <array>
#include "helperFunctions.h"
#include <bitset>
#include<algorithm>
#include"DeviceBase.h"


class FTDIRegProgrammer: public DeviceBase
{
private:
	int clkBit, dataBit, enableBit, addressLen, packetLen, dataFirst, msbFirst, enableHigh, clkEdge, dataOutBit, readClkEdge, readOutMode, packetOrder;
	std::array<bool, 8> maskArray;
	std::array<bool, 8> valueArray;
public:
	FT_HANDLE ftHandle;
	FT_STATUS ftStatus;;

	virtual int set(FT_HANDLE handle) override;

	int writeToFTDI(std::string data);
	std::string readFromFTDI(std::string data);
	int reset();
	virtual int readReg(int addr) override;
	virtual int writeReg(int addr, int val) override;
	int purge();
	int purgeRX();
	int purgeTX();
	FTDIRegProgrammer(void* handle,int *status);
	~FTDIRegProgrammer();
	int close();
	int setPin(int index, int val);
	int setMask();
	int writeValue();

	int setWritePacket(int val);
	int setWritePacketString(std::string data);

	int rawWritePins(int data);
	void setAddressLen(int value);
	void setPacketLen(int value);
	void setPacketOrder(int value);
	void setMsbFirst(int value);
	void setEnableType(int value);
	void setClkEdge(int value);
	std::string readSerialData(int size);
	int setBaudRate(uint32_t baudRate);
	int setDivisor(uint8_t divisor);

};

#endif