
/***Base class for all devices/interfaces. This class is used in the bank to read/write registers
All interface classes are expected to be derived from DeviceBase. They are expected to have a readReg and a writeReg method. 
***/
#ifndef DEVICE_BASE_H
#define DEVICE_BASE_H
class DeviceBase
{
public:
	virtual int readReg(int addr) = 0;
	virtual int writeReg(int addr, int data) = 0;
	virtual int set(void* handle) = 0;
};

#endif