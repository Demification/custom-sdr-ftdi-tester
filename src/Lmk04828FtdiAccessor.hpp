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

    bool init(double frequency = -1.0);
    
    bool writeRegister(uint16_t address, uint8_t value);
    bool readRegister(uint16_t address, uint8_t& value);

private:
    void initPll1();
    void initPll2();

    bool parseInitRegs(const std::string& text, 
                       std::map<RegisterAddr, RegisterValue>& registers);

    bool computeInitRegsByFrequency(double frequency, 
                                    std::map<RegisterAddr, RegisterValue>& registers);
    
    void writeRegisters(std::map<RegisterAddr, RegisterValue>& registers);
};