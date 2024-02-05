#pragma once
#include <string>
#include <vector>
#include <memory>

#include "FtdiDeviceInfoList.hpp"

struct BitbangConfig {
    uint8_t sck;
    uint8_t mosi;
    uint8_t miso;
    uint8_t nss;
};

struct ChannelConfig_t;
class FtdiSpiAccessProvider
{
public:
    using UPtr = std::unique_ptr<FtdiSpiAccessProvider>;

    FtdiSpiAccessProvider(const BitbangConfig& config, 
                          const FtdiDeviceInfo::Ptr& info);

    FtdiSpiAccessProvider(const ChannelConfig_t& config, 
                          const FtdiDeviceInfo::Ptr& info, int id);

    virtual ~FtdiSpiAccessProvider();

    void* handle() const;
    const FtdiDeviceInfo::Ptr& deviceInfo() const;

    bool isInitedBitbangMode() const;
    bool isInitedMpsseMode() const;

    bool mpsseWaitIsBusy();
    bool mpsseWrite(uint8_t* data, unsigned length);
    bool mpsseWriteAndRead(uint8_t* data, uint8_t* result, unsigned length);
    
    /**
    * @brief FtdiSpiAccessProvider::bitbangWrite
    * write 3 bytes: 2 byte address and 1 byte of data
    * sendibg starts from 2nd byte from MSB (1st MSB is ignored)
    * @param data packed 3 byte
    * @return bool
    */
    bool bitbangWrite(uint32_t data);

    /**
    * @brief FtdiSpiAccessProvider::bitbangWrite
    *  cteate 5MHz SCK and write bytes bit by bit (MSB first)
    * @param data - bytes to write
    * @param length - data length in bytes
    * @param written - in bytes
    * @return bool
    */
    bool bitbangWrite(unsigned char * data, 
                      unsigned length, 
                      unsigned int *written);

    /**
    * @brief FtdiSpiAccessProvider::bitbangWriteAndRead
    * write 2 bytes: 2 byte address
    * sendibg starts from 2nd byte from MSB (1st MSB is ignored)
    * @param data packed 2 byte
    * @param value
    * @return bool
    */
    bool bitbangWriteAndRead(uint32_t data, uint8_t *result);

    /**
    * @brief FtdiSpiAccessProvider::bitbangWriteAndRead
    * sync mode must be ON
    * @param data - bytes to write and read
    * @param len - data length in bytes
    * @param writen
    * @return bool
    */
    bool bitbangWriteAndRead(unsigned char * data, 
                             unsigned length, 
                             unsigned int *writen);

private:
    void* m_handle = nullptr;
    bool m_initedBitbangMode = false;
    bool m_initedMpsseMode = false;
    
    BitbangConfig m_bitbangConfig;
    FtdiDeviceInfo::Ptr m_info;

    const unsigned m_maxBurstLen = 3;
    std::vector<unsigned char> m_bitbangBuff;

    bool initBitbangMode(const FtdiDeviceInfo::Ptr& info, 
                         const BitbangConfig& config);

    bool initMpsseMode(int id, 
                       const ChannelConfig_t& config);
};