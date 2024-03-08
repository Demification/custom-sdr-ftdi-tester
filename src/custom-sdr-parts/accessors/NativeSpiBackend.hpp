#pragma once
#include <memory>
#include <linux/spi/spidev.h>

#include "AbstractSpiBackend.hpp"

class NativeSpiBackend: public AbstractSpiBackend
{
public:
    NativeSpiBackend();
    virtual ~NativeSpiBackend();

    bool isInited() override;
    SpiBackendType type() const override;

    bool write(uint8_t* data, unsigned length) override;
    bool writeAndRead(uint8_t* data, uint8_t* result, unsigned length) override;

private:
    int m_fd = -1;
    bool m_isInited = false;
    std::string m_device = "/dev/spidev1.0";

    uint8_t m_bits = 8;
    uint32_t m_speed = 20000000;
    uint32_t m_mode = SPI_MODE_0;
};

