#include "FtdiSpiAccessProvider.hpp"
#include <thread>
#include <chrono>

#include "spi.h"
#include "ftdi.h"
#include "Debug.hpp"

FtdiSpiAccessProvider::FtdiSpiAccessProvider(const BitbangConfig &config, 
                                             const FtdiDeviceInfo::Ptr& info)
    : m_bitbangConfig(config),
      m_info(info)
{
    if(info.get() && initBitbangMode(info, config)) {
        m_initedBitbangMode = true;
    }
}

FtdiSpiAccessProvider::FtdiSpiAccessProvider(const ChannelConfig_t &config, 
                                             const FtdiDeviceInfo::Ptr &info, int id)
    : m_info(info)
{
    if(info.get() && initMpsseMode(id, config)) {
        m_initedMpsseMode = true;
    }
}

FtdiSpiAccessProvider::~FtdiSpiAccessProvider() {
    if(m_handle)
        FTD2_Close(m_handle);
}

void *FtdiSpiAccessProvider::handle() const {
    return m_handle;
}

const FtdiDeviceInfo::Ptr &FtdiSpiAccessProvider::deviceInfo() const {
    return m_info;
}

bool FtdiSpiAccessProvider::mpsseWrite(uint8_t *data, unsigned length)
{
    if(!m_initedMpsseMode) return false;

    uint32_t option = SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES | 
                      SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |  
                      SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE;

    unsigned int transfered, result;
    if(FT_OK == (result = SPI_Write(m_handle, data, length, &transfered, option))){
        if(transfered != length) 
            __DEBUG_ERROR__("Transfered bytes not equal size.");
    }
    else __DEBUG_ERROR__("Can`t write spi. Result: " + std::to_string(result));
    return !result;
}

bool FtdiSpiAccessProvider::mpsseWriteAndRead(
    uint8_t* data, uint8_t* result, unsigned length)
{
    if(!m_initedMpsseMode) return false;

    uint32_t option = SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES | 
                      SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |  
                      SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE;

    unsigned int transfered, status;
    if(FT_OK != (status = SPI_ReadWrite(
        m_handle, result, data, length, &transfered, option)))
    {
        __DEBUG_ERROR__("Can`t write spi. Status: " + std::to_string(status));
        return false;
    }

    if(transfered != length) {
        __DEBUG_ERROR__("Transfered bytes not equal size.");
        return false;
    }
            
    return true;
}

bool FtdiSpiAccessProvider::isInitedBitbangMode() const {
    return m_initedBitbangMode;
}

bool FtdiSpiAccessProvider::isInitedMpsseMode() const {
    return m_initedMpsseMode;
}

bool FtdiSpiAccessProvider::bitbangWrite(uint32_t data) {
    if(!m_initedBitbangMode) return false;

    unsigned char d[3];

    for (int k = 0; k < 3; ++k)
        d[2 - k] = data >> (k * 8);

    unsigned int written;
    if(!bitbangWrite(d, 3, &written)) 
        return false;

    if(written != 3) {
        __DEBUG_ERROR__("Bitbang written:" + std::to_string(written)); 
        return false;
    }

    return true;
}

bool FtdiSpiAccessProvider::bitbangWrite(unsigned char *data, 
                                         unsigned length, 
                                         unsigned int *written)
{
    if(!m_initedBitbangMode) return false;
    
    FT_STATUS status;
    if(FT_OK != (status = FTD2_Purge(m_handle, FT_PURGE_RX))) {
        __DEBUG_ERROR__("Can`t purge in bitbang mode. Status:" + std::to_string(status)); 
        return false;
    }

    unsigned char * p = m_bitbangBuff.data();
    *p++ = 0; //nSS down
    unsigned cnt = 0;
    while (cnt < length && cnt < m_maxBurstLen )
    {
        unsigned char byte = *data++;
        for (unsigned k=0; k<8; ++k)  //bit counter
        {
            unsigned char b = 0;
            if (byte & 0x80) b |= m_bitbangConfig.mosi;
            *p++ = b;                       //sck down, change mosi
            *p++ = b | m_bitbangConfig.sck; //sck up, mosi do not shanged
            byte <<= 1;
        }
        cnt++;
    }
    *p++ = m_bitbangConfig.nss | m_bitbangConfig.sck; //nSS up,
    *p = m_bitbangConfig.nss;                  //sck down

    unsigned int wrtn = 0;
    if(FT_OK != (status = FTD2_Write(m_handle, m_bitbangBuff.data(), cnt*16+3, &wrtn))) {
        __DEBUG_ERROR__("Can`t write register in bitbang mode. Status:" + std::to_string(status)); 
        return false;
    }

    if (written)
        *written = wrtn>=3 ? (wrtn-3)/16 : 0;

    return true;
}

bool FtdiSpiAccessProvider::bitbangWriteAndRead(uint32_t data, uint8_t *result) {
    if(!m_initedBitbangMode) return false;
    
    unsigned char d[3];

    for (int k = 0; k < 3; ++k)
        d[2 - k] = data >> (k * 8);

    unsigned int written;
    if(!bitbangWriteAndRead(d, 3, &written)) 
        return false;
    
    if(written != 3) {
        __DEBUG_ERROR__("Bitbang written:" + std::to_string(written)); 
        return false;
    }
    
    if (result) *result = d[2];

    return true;
}

bool FtdiSpiAccessProvider::bitbangWriteAndRead(unsigned char *data, 
                                                unsigned length, 
                                                unsigned int *writen)
{
    if(!m_initedBitbangMode) return false;
    
    FT_STATUS status;
    if(FT_OK != (status = FTD2_Purge(m_handle, FT_PURGE_RX))) {
        __DEBUG_ERROR__("Can`t purge in bitbang mode. Status:" + std::to_string(status)); 
        return false;
    }

    unsigned char * src = data;
    unsigned char * p = m_bitbangBuff.data();
    *p++ = 0; //nSS down
    unsigned cnt = 0;
    while (cnt < length && cnt < m_maxBurstLen )
    {
        unsigned char byte = *src++;
        for (unsigned k=0; k<8; ++k) //bit counter
        {
            unsigned char b = 0;
            if (byte & 0x80) b |= m_bitbangConfig.mosi;
            *p++ = b;                       //sck down, change mosi
            *p++ = b | m_bitbangConfig.sck; //sck up, mosi do not shanged
            byte <<= 1;
        }
        cnt++;
    }
    *p++ = m_bitbangConfig.nss | m_bitbangConfig.sck; //nSS up,
    *p = m_bitbangConfig.nss;                         //sck down

    unsigned int wrtn = 0;
    unsigned int retn= 0;
    if(FT_OK != (status = FTD2_Write(m_handle, m_bitbangBuff.data(), cnt*16+3, &wrtn))) {
        __DEBUG_ERROR__("Can`t write register in bitbang mode. Status:" + std::to_string(status)); 
        return false;
    }
    if(FT_OK != (status = FTD2_Read(m_handle, m_bitbangBuff.data(), wrtn, &retn))) {
        __DEBUG_ERROR__("Can`t read register in bitbang mode. Status:" + std::to_string(status)); 
        return false;
    }

    p = m_bitbangBuff.data() + 2;
    length = retn>=3 ? (retn-3)/16 : 0;
    cnt = 0;
    while (cnt < length && cnt < m_maxBurstLen )
    {
        unsigned char byte = 0;
        for (unsigned k=0;k<8;++k) //bit counter
        {
            byte <<= 1;
            byte |= *p & m_bitbangConfig.miso ? 0x01 : 0;
            p += 2;
        }
        cnt++;
        *data++ = byte;
    }

    if (writen)
        *writen = wrtn>=3 ? (wrtn-3)/16 : 0;

    return true;
}

bool FtdiSpiAccessProvider::initBitbangMode(const FtdiDeviceInfo::Ptr& info, 
                                            const BitbangConfig &config)
{
    FT_STATUS status;

    if(!info) {
        __DEBUG_ERROR__("Info is empty.");
        return false;
    }

    if(FT_OK != (status = FTD2_OpenEx((void*)info->description.c_str(), 
                                       FT_OPEN_BY_DESCRIPTION, &m_handle))) 
    {
        __DEBUG_ERROR__("Can`t open device. Status" + std::to_string(status));
        return false;
    }

    m_bitbangBuff.resize(m_maxBurstLen * 16 + 3);

    if(FT_OK != (status = FTD2_SetLatencyTimer(m_handle, 2))) {
        __DEBUG_ERROR__("Can`t set latency timer. Status:" + std::to_string(status)); 
        return false;
    }

    if(FT_OK != (status = FTD2_SetTimeouts(m_handle, 1000, 100))) {
        __DEBUG_ERROR__("Can`t set timeout timer. Status:" + std::to_string(status)); 
        return false;
    }

    if(FT_OK != (status = FTD2_SetBitmode(m_handle, 
                                          config.nss|config.sck|config.mosi, 
                                          FT_BITMODE_SYNC_BITBANG)))
    {
        __DEBUG_ERROR__("Can`t set bitbang mode. Status:" + std::to_string(status)); 
        return false;
    }

    unsigned int written = 0;
    unsigned char nss = config.nss;
    if (FT_OK != (status = FTD2_Write(m_handle, &nss, 1, &written))) {
        __DEBUG_ERROR__("Can`t write nss. Status:" + std::to_string(status));
        return false;
    }

    if (written != 1) {
        __DEBUG_ERROR__("Written:" + std::to_string(written));
        return false;
    }

    return true;
}

bool FtdiSpiAccessProvider::initMpsseMode(int id, 
                                          const ChannelConfig_t &config) 
{
    unsigned int result;
    if(FT_OK == (result = SPI_OpenChannel(id, &m_handle))) {
        if(FT_OK == (result = SPI_InitChannel(m_handle, const_cast<ChannelConfig_t*>(&config)))) {
            return true;
        }
        else __DEBUG_ERROR__("Can`t init spi channel. Result: " + std::to_string(result));
    }
    else __DEBUG_ERROR__("Can`t open spi channel. Result: " + std::to_string(result));

    return false;
}

bool FtdiSpiAccessProvider::mpsseWaitIsBusy() {
    if(!m_initedMpsseMode) return false;

    unsigned int state = 1, counter = 0;
    while (state) {
        if(FT_OK != SPI_IsBusy(m_handle, &state)){
            __DEBUG_ERROR__("Can`t get spi busy state.");
            return false;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(true));
        ++counter;

        if(counter > 2000) {
            __DEBUG_ERROR__("Spi busy-state stuck.");
            return false;
        }
    }
    return true;
}
