#include "Afe7769FtdiAccessor.hpp"
#include <string.h>

#include <thread>
#include <chrono>
#include <memory>
#include <functional>

#include "spi.h"
#include "afe77xx.h"
#include "Debug.hpp"

Afe7769FtdiAccessor::Afe7769FtdiAccessor(FtdiDeviceInfoList::Ptr infoList) 
    : FtdiSpiMemoryAccessor(ChannelConfig_t {.ClockRate = 20000000,
                                             .LatencyTimer = 0,
                                             .configOptions = SPI_CONFIG_OPTION_MODE0 | 
                                                              SPI_CONFIG_OPTION_CS_DBUS3 | 
                                                              SPI_CONFIG_OPTION_CS_ACTIVELOW, 
                                             .Pin = 0x0BUL | (0x08UL << 8) | (0x0BUL<<16) | (0x08UL << 24),
                                             .currentPinState = 0x0BUL | (0x08UL << 8) }, 
                            infoList->getByIndex(0), 0) 
{
    if(!isInitedMpsseMode())
        __DEBUG_INFO__("Afe7769 ftdi accessor not inited.");
}

AccessorType Afe7769FtdiAccessor::type() const {
    return AccessorType::AFE7769;
}

bool Afe7769FtdiAccessor::init() {
    if(!isInitedMpsseMode()) return false;

    if(!writeRegister(0x00, 0x30) || 
            !writeRegister(0x01, 0x00))
        return false;

    return true;
}

void *Afe7769FtdiAccessor::handle() {
    return FtdiSpiMemoryAccessor::handle();
}

bool Afe7769FtdiAccessor::readRegisters(uint16_t address, 
                                        uint8_t *values, 
                                        uint32_t length) 
{
    if(!isInitedMpsseMode()) return false;
    
    for (size_t i = 0; i < length; i++) {
        if(!readRegister(address + i, values[i])) {
             __DEBUG_ERROR__("Can`t write register, "
                "address:" + std::to_string(address + i));
            
            continue;
        }
    }
    return true;
}

bool Afe7769FtdiAccessor::writeRegisters(uint16_t address, 
                                         uint8_t *values, 
                                         uint32_t length) 
{
    if(!isInitedMpsseMode()) return false;

    for (size_t i = 0; i < length; i++) {
        if(!writeRegister(address + i, values[i])) {
             __DEBUG_ERROR__("Can`t write register, "
                "address:" + std::to_string(address + i));
            
            continue;
        }
    }
    return true;
}

bool Afe7769FtdiAccessor::readRegistersBurst(uint16_t address, 
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
        !mpsseWriteAndRead(data.get(), data.get(), size))
    {
         __DEBUG_ERROR__("Can`t write register, "
                "address:" + std::to_string(address));

        return false;
    }

    memcpy(values, &data[2], length);
    return true;
}

bool Afe7769FtdiAccessor::writeRegistersBurst(uint16_t address, 
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
        !mpsseWrite(data.get(), size))
    {
         __DEBUG_ERROR__("Can`t write register, "
                "address:" + std::to_string(address));

        return false;
    }
    return true;
}

bool Afe7769FtdiAccessor::writeRegister(uint16_t address,
                                        uint8_t value)
{
    if(!mpsseWaitIsBusy()) return false;

    auto paddr = ((uint8_t*)&address) + 1;
    uint8_t packet[3] = {*paddr, *(--paddr), value};

    return mpsseWrite(packet, 3);
}

bool Afe7769FtdiAccessor::readRegister(uint16_t address, 
                                       uint8_t &value)
{
    if(!mpsseWaitIsBusy()) return false;

    uint8_t read_packet[3] = {0};

    auto paddr = ((uint8_t*)&address) + 1;
    uint8_t write_packet[3] = {*paddr, *(--paddr), 0x00};
    write_packet[0] |= 0x80;

    if(!mpsseWriteAndRead(write_packet, read_packet, 3)) {
        return false;
    }

    value = read_packet[2];
    return true;
}
