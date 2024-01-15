#pragma once
#include <memory>
#include <vector>

#include "FtdiSpiMemoryAccessor.hpp"

typedef uint16_t RegisterAddr;
typedef uint8_t  RegisterValue;

class Lmk04828FtdiAccessor: protected FtdiSpiMemoryAccessor
{
public:
    Lmk04828FtdiAccessor(FtdiDeviceInfoList::Ptr infoList);
    virtual ~Lmk04828FtdiAccessor() = default;

    bool write(uint32_t data);
    bool read(uint32_t data, uint8_t& value);

    bool init(double refClkFreq = 0.0);
    bool sendSysref();
    
    bool getLosStatus();
    bool isLockedPll1();
    bool isLockedPll2();

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

    bool parseInitRegs(const std::string& text, 
                       std::map<RegisterAddr, RegisterValue>& registers);

    bool setVcxcoTrimRegs(uint16_t value, 
                          std::map<RegisterAddr, RegisterValue>& registers);
                          
    bool computeInitRegsByFrequency(double frequency, 
                                    std::map<RegisterAddr, RegisterValue>& registers);
    
    void writeRegisters(std::map<RegisterAddr, RegisterValue>& registers);
};