#pragma once
#include <memory>
#include <string>
#include <stdint.h>

enum SpiBackendType
{
    Ftdi,
    Native
};

std::string to_string(SpiBackendType type);

class AbstractSpiBackend
{
public:
    using Ptr = std::shared_ptr<AbstractSpiBackend>;
    AbstractSpiBackend() = default;

    virtual bool isInited() = 0;
    virtual SpiBackendType type() const = 0;

    virtual bool write(uint8_t* data, unsigned length) = 0;
    virtual bool writeAndRead(uint8_t* data, uint8_t* result, unsigned length) = 0;
};