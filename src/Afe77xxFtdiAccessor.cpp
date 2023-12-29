#include "Afe77xxFtdiAccessor.hpp"
#include <string.h>

#include <thread>
#include <chrono>
#include <memory>
#include <functional>

#include "spi.h"
#include "afe77xx.h"
#include "Debug.hpp"

Afe77xxFtdiAccessor::Afe77xxFtdiAccessor(FtdiDeviceInfoList::Ptr infoList) 
    : FtdiSpiMemoryAccessor(ChannelConfig_t {.ClockRate = 20000000,
                                           .LatencyTimer = 0,
                                           .configOptions = SPI_CONFIG_OPTION_MODE0 | 
                                                            SPI_CONFIG_OPTION_CS_DBUS3 | 
                                                            SPI_CONFIG_OPTION_CS_ACTIVELOW, 
                                           .Pin = 0 }, 
                            infoList->getByIndex(0), 0) 
{
    if(isInitedMpsseMode())
        __DEBUG_INFO__("Afe77xx ftdi accessor info:" + deviceInfo()->string());
}

bool Afe77xxFtdiAccessor::init() {
    if(!isInitedMpsseMode()) return false;

    writeRegister(0x00, 0x30);
    writeRegister(0x01, 0x00);

    return true;
}

void *Afe77xxFtdiAccessor::handle() {
    return FtdiSpiMemoryAccessor::handle();
}

bool Afe77xxFtdiAccessor::readRegisters(uint16_t address, 
                                        uint8_t *values, 
                                        uint32_t length) 
{
    if(!isInitedMpsseMode()) return false;
    
    for (size_t i = 0; i < length; i++) {
        if(!mpsseWaitIsBusy() || !readRegister(address + i, values[i])) {
             __DEBUG_ERROR__("Can`t write register, "
                "address:" + std::to_string(address + i));
            
            continue;
        }
    }
    return true;
}

bool Afe77xxFtdiAccessor::writeRegisters(uint16_t address, 
                                         uint8_t *values, 
                                         uint32_t length) 
{
    if(!isInitedMpsseMode()) return false;

    for (size_t i = 0; i < length; i++) {
        if(!mpsseWaitIsBusy() || !writeRegister(address + i, values[i])) {
             __DEBUG_ERROR__("Can`t write register, "
                "address:" + std::to_string(address + i));
            
            continue;
        }
    }
    return true;
}

bool Afe77xxFtdiAccessor::readRegistersBurst(uint16_t address, 
                                             uint8_t *values, 
                                             uint32_t length)
{
    if(!isInitedMpsseMode()) return false;

    auto size = length + 2;
    auto data = std::unique_ptr<uint8_t[]>(new uint8_t[size]);

    auto paddr = ((uint8_t*)&address) + 1;
    data[0] = *paddr | 0x80;
    data[1] = *(--paddr);

    memset(&data[2], 0, length);

    if(!mpsseWaitIsBusy() || 
        !mpsseWriteAndRead(data.get(), data.get(), size, 
                        SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |  
                        SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE)) 
    {
         __DEBUG_ERROR__("Can`t write register, "
                "address:" + std::to_string(address));

        return false;
    }

    memcpy(values, &data[2], length);
    return true;
}

bool Afe77xxFtdiAccessor::writeRegistersBurst(uint16_t address, 
                                              uint8_t *values, 
                                              uint32_t length)
{
    if(!isInitedMpsseMode()) return false;

    auto size = length + 2;
    auto data = std::unique_ptr<uint8_t[]>(new uint8_t[size]);

    auto paddr = ((uint8_t*)&address) + 1;
    data[0] = *paddr;
    data[1] = *(--paddr);

    memcpy(&data[2], values, length);

    if(!mpsseWaitIsBusy() || 
        !mpsseWrite(data.get(), size, 
                        SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |  
                        SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE)) 
    {
         __DEBUG_ERROR__("Can`t write register, "
                "address:" + std::to_string(address));

        return false;
    }
    return true;
}

bool Afe77xxFtdiAccessor::writeRegister(uint16_t address,
                                        uint8_t value)
{
    auto paddr = ((uint8_t*)&address) + 1;
    uint8_t packet[3] = {*paddr, *(--paddr), value};

    return mpsseWrite(packet, 3, 
                        SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |  
                        SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
}

bool Afe77xxFtdiAccessor::readRegister(uint16_t address, 
                                       uint8_t &value)
{
    uint8_t read_packet[3];

    auto paddr = ((uint8_t*)&address) + 1;
    uint8_t write_packet[3] = {*paddr, *(--paddr), 0xff};
    write_packet[0] |= 0x80;

    if(!mpsseWriteAndRead(write_packet, read_packet, 3, 
                                SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |  
                                SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE))
    {
        return false;
    }

    value = read_packet[2];
    return true;
}
