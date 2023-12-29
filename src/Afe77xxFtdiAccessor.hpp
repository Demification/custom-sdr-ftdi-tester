#pragma once
#include <memory>

#include "FtdiSpiMemoryAccessor.hpp"

class Afe77xxFtdiAccessor: protected FtdiSpiMemoryAccessor
{
public:
    using UPtr = std::unique_ptr<Afe77xxFtdiAccessor>;

    Afe77xxFtdiAccessor(FtdiDeviceInfoList::Ptr infoList);
    virtual ~Afe77xxFtdiAccessor() = default;

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

private: 
    bool writeRegister(uint16_t address, uint8_t value);
    bool readRegister(uint16_t address, uint8_t& value);
};

