#pragma once
#include <memory>
#include "AbstractAccessor.hpp"
#include "FtdiSpiMemoryAccessor.hpp"

class Afe7769FtdiAccessor: public AbstractAccessor, 
                           protected FtdiSpiMemoryAccessor
{
public:
    Afe7769FtdiAccessor(FtdiDeviceInfoList::Ptr infoList);
    AccessorType type() const override;

    bool init();
    void* handle();

    bool readRegisters(uint16_t address, 
                       uint8_t* values, 
                       uint32_t length);

    bool writeRegisters(uint16_t address, 
                        uint8_t* values, 
                        uint32_t length);

    bool readRegistersBurst(uint16_t address, 
                            uint8_t* values, 
                            uint32_t length);
    
    bool writeRegistersBurst(uint16_t address, 
                             uint8_t* values, 
                             uint32_t length);

    bool writeRegister(uint16_t address, uint8_t value);
    bool readRegister(uint16_t address, uint8_t& value);

private: 
};

