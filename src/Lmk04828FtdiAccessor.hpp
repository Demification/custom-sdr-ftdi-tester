#pragma once
#include <memory>
#include <vector>
#include <ftd2xx.h>

class Lmk04828FtdiAccessor
{
public:
    Lmk04828FtdiAccessor();
    ~Lmk04828FtdiAccessor();

    bool setup();

    bool writeRegister(uint16_t address, uint8_t value);
    double setPllRefClk(double frequency);

private:
    bool m_inited = false;
    FT_HANDLE m_handle = nullptr;

    std::vector<FT_DEVICE_LIST_INFO_NODE> m_info;

    bool initInfo();
    bool initDevice();

   struct BitbangConfig {
        unsigned char sck;
        unsigned char mosi;
        unsigned char miso;
        unsigned char nss;
    };

    const unsigned m_maxBurstLen = 3;
    std::vector<unsigned char> m_bitbangBuff;

    const BitbangConfig m_config;

    bool initBitbangMode(const BitbangConfig& config);
    bool bitbangWrite(const BitbangConfig& config,
                      unsigned char * data, 
                      unsigned len, 
                      unsigned int *written);
};