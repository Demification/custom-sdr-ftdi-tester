#include "Afe77xxFtdiAccessor.hpp"
#include <thread>
#include <chrono>
#include <functional>

#include "spi.h"
#include "afe77xx.h"

#include "Debug.hpp"
#include "MemoryPartTypes.hpp"

Afe77xxFtdiAccessor::Afe77xxFtdiAccessor() {
    unsigned int result;
    if(FT_OK == (result = SPI_OpenChannel(0, &m_handle))) {
        ChannelConfig config = {.ClockRate = 20000000,
                                .LatencyTimer = 0,
                                .configOptions = SPI_CONFIG_OPTION_MODE0 | 
                                                 SPI_CONFIG_OPTION_CS_DBUS3 | 
                                                 SPI_CONFIG_OPTION_CS_ACTIVELOW, 
                                .Pin = 0 };

        if(FT_OK == (result = SPI_InitChannel(m_handle, &config))) {
            m_inited = true;
        }
        else __DEBUG_ERROR__("Can`t init spi channel. Result: " + std::to_string(result));
    }
    else __DEBUG_ERROR__("Can`t open spi channel. Result: " + std::to_string(result));
}

Afe77xxFtdiAccessor::~Afe77xxFtdiAccessor() {
    if(m_handle)
        SPI_CloseChannel(m_handle);
}

bool Afe77xxFtdiAccessor::setup() {
    if(!m_inited) return false;

    writeRegister(0x00, 0x30);
    writeRegister(0x01, 0x00);

    return true;
}

FT_HANDLE Afe77xxFtdiAccessor::handle() const {
    return m_handle;
}

bool Afe77xxFtdiAccessor::readRegisters(uint16_t address, 
                                        unsigned char *buffer, 
                                        unsigned int len) {
    if(!m_inited) return false;
    
    for (size_t i = 0; i < len; i++) {
        if(!waitSpiAccess()){
            __DEBUG_ERROR__("Spi busy-state stuck.");
            return false;
        }

        if(!readRegister(address + i, buffer[i])) {
             __DEBUG_ERROR__("Can`t write register, "
                "address:" + std::to_string(address + i));
            return false;
        }
    }
    return true;
}

bool Afe77xxFtdiAccessor::writeRegisters(uint16_t address, 
                                         unsigned char *buffer, 
                                         unsigned int len) 
{
    if(!m_inited) return false;

    for (size_t i = 0; i < len; i++) {
        if(!waitSpiAccess()){
            __DEBUG_ERROR__("Spi busy-state stuck.");
            return false;
        }

        if(!writeRegister(address + i, buffer[i])) {
             __DEBUG_ERROR__("Can`t write register, "
                "address:" + std::to_string(address + i));
            return false;
        }
    }
    return true;
}

bool Afe77xxFtdiAccessor::readRegisters(uint16_t address, unsigned int &buffer)
{
    HeaderStreamingPacket read_packet, write_packet;

    write_packet.rw_bit = 1;
    write_packet.address = address;
    write_packet.value = 0xffffffff;

    unsigned option = SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES | 
                      SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | 
                      SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE;

    auto size = sizeof(Packet);
    unsigned int transfered, result;
    if(FT_OK == (result = SPI_ReadWrite(m_handle, 
                                        read_packet.data, 
                                        write_packet.data, 
                                        size, 
                                        &transfered, 
                                        option))){
        if(transfered != size) 
            __DEBUG_ERROR__("Transfered bytes not equal size.");
        else
            buffer = read_packet.value;
    }
    else __DEBUG_ERROR__("Can`t write spi. Result: " + std::to_string(result));
    return !result;
}

bool Afe77xxFtdiAccessor::writeRegisters(uint16_t address, unsigned int buffer)
{
    HeaderStreamingPacket packet;

    packet.rw_bit = 0;
    packet.address = address;
    packet.value = buffer;

    unsigned option = SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES | 
                      SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | 
                      SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE;

    auto size = sizeof(HeaderStreamingPacket);
    unsigned int transfered, result;
    if(FT_OK == (result = SPI_Write(m_handle, packet.data, size, &transfered, option))){
        if(transfered != size) 
            __DEBUG_ERROR__("Transfered bytes not equal size.");
    }
    else __DEBUG_ERROR__("Can`t write spi. Result: " + std::to_string(result));
    return !result;
}

bool Afe77xxFtdiAccessor::waitSpiAccess() {
    unsigned int state = 1, counter = 0;
    while (state) {
        if(FT_OK != SPI_IsBusy(m_handle, &state)){
            __DEBUG_ERROR__("Can`t get spi buzy state.");
            return false;
        }

        std::this_thread::sleep_for (std::chrono::milliseconds(true));
        ++counter;

        if(counter > 2000) 
            return false;
    }
    return true;
}

bool Afe77xxFtdiAccessor::writeRegister(uint16_t address,
                                            unsigned char value)
{
    Packet packet;

    packet.rw_bit = 0;
    packet.address = address;
    packet.value = value;

    unsigned option = SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES | 
                      SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | 
                      SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE;

    auto size = sizeof(Packet);
    unsigned int transfered, result;
    if(FT_OK == (result = SPI_Write(m_handle, packet.data, size, &transfered, option))){
        if(transfered != size) 
            __DEBUG_ERROR__("Transfered bytes not equal size.");
    }
    else __DEBUG_ERROR__("Can`t write spi. Result: " + std::to_string(result));
    return !result;
}

bool Afe77xxFtdiAccessor::readRegister(uint16_t address, unsigned char &value)
{
    Packet read_packet, write_packet;

    write_packet.rw_bit = 1;
    write_packet.address = address;
    write_packet.value = 0xff;

    unsigned option = SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES | 
                      SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | 
                      SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE;

    auto size = sizeof(Packet);
    unsigned int transfered, result;
    if(FT_OK == (result = SPI_ReadWrite(m_handle, 
                                        read_packet.data, 
                                        write_packet.data, 
                                        size, 
                                        &transfered, 
                                        option))){
        if(transfered != size) 
            __DEBUG_ERROR__("Transfered bytes not equal size.");
        else
            value = read_packet.value;
    }
    else __DEBUG_ERROR__("Can`t write spi. Result: " + std::to_string(result));
    return !result;
}
