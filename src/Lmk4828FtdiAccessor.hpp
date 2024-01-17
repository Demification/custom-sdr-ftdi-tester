#pragma once
#include <memory>
#include <vector>

#include "AbstractAccessor.hpp"
#include "FtdiSpiMemoryAccessor.hpp"

typedef uint16_t RegisterAddr;
typedef uint8_t  RegisterValue;

struct Lmk4828Status {
	    uint16_t pll1Lock : 1;
	    uint16_t pll1LockLost : 1;
	    uint16_t pll2Lock : 1;
	    uint16_t pll2LockLost : 1;
	    uint16_t clkLOS : 1;  //refclk loss of signal
	    uint16_t dacTrackLock : 1;
	    uint16_t holdOver : 1;
	    uint16_t dac; //VCXCO voltage 0-1023
};

class Lmk4828FtdiAccessor: public AbstractAccessor, 
                           protected FtdiSpiMemoryAccessor
{
public:
    Lmk4828FtdiAccessor(FtdiDeviceInfoList::Ptr infoList);
    AccessorType type() const override;

    bool write(uint32_t data);
    bool read(uint32_t data, uint8_t& value);

    bool init(double refClkFreq = 0.0);
    
    bool sysref();
    bool status(Lmk4828Status& value);
    
    bool readRegisters(uint16_t address, 
                       uint8_t* values, 
                       uint32_t length);

    bool writeRegisters(uint16_t address, 
                        uint8_t* values, 
                        uint32_t length);

protected:
    bool writeRegister(uint16_t address, uint8_t value);
    bool readRegister(uint16_t address, uint8_t& value);

private:
    void initPll1();
    void initPll2();

    bool getLosStatus();
    bool isLockedPll1();
    bool isLockedPll2();

    bool parseInitRegs(const std::string& text, 
                       std::map<RegisterAddr, RegisterValue>& registers);

    bool setVcxcoTrimRegs(uint16_t value, 
                          std::map<RegisterAddr, RegisterValue>& registers);
                          
    bool computeInitRegsByFrequency(double frequency, 
                                    std::map<RegisterAddr, RegisterValue>& registers);
    
    void writeRegisters(std::map<RegisterAddr, RegisterValue>& registers);
};