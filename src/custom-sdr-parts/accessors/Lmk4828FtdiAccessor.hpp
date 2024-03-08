#pragma once
#include <memory>
#include <vector>

#include "AbstractAccessor.hpp"
#include "FtdiSpiAccessProvider.hpp"

typedef uint16_t RegisterAddr;
typedef uint8_t  RegisterValue;

struct customsdr_pll_status {
	uint16_t pll1Lock : 1;
	uint16_t pll1LockLossLatch : 1;
	uint16_t pll2Lock : 1;
	uint16_t pll2LockLossLatch : 1;
	uint16_t clkLOS : 1;  //refclk loss of signal
	uint16_t dacTrackLock : 1;
	uint16_t holdOver : 1;
	uint16_t dac; //VCXCO voltage 0-1023
};

class Lmk4828FtdiAccessor: public AbstractAccessor, 
                           protected FtdiSpiAccessProvider
{
public:
    Lmk4828FtdiAccessor(FtdiDeviceInfoList::Ptr infoList);
    AccessorType type() const override;

    bool init(double refClkFreq);
    
    bool sendSysref();
    bool status(customsdr_pll_status& value);

    bool writeRegister(uint16_t address, const uint8_t value);
    bool readRegister(uint16_t address, uint8_t* value);
    
    bool readRegisters(uint16_t address, uint8_t* values, uint32_t length);
    bool writeRegisters(uint16_t address, const uint8_t* values, uint32_t length);

    void init() override;

protected:
    
    bool write(const uint32_t data);
    bool read(uint32_t data, uint8_t* value);

private:
    void initPll1();
    void initPll2();

    bool getLosStatus();
    bool getPll1Lock();
    bool getPll2Lock();

    bool parseInitRegs(const std::string& text, 
                       std::map<RegisterAddr, RegisterValue>& registers);

    bool setVcxcoTrimRegs(double value, 
                          std::map<RegisterAddr, RegisterValue>& registers);
                          
    bool computeInitRegsByFrequency(double frequency, 
                                    std::map<RegisterAddr, RegisterValue>& registers);
    
    void writeRegisters(std::map<RegisterAddr, RegisterValue>& registers);
};