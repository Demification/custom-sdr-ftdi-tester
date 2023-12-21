#pragma once
#include <memory>
#include <ftd2xx.h>

class Afe77xxFtdiAccessor
{
public:
    using UPtr = std::unique_ptr<Afe77xxFtdiAccessor>;

    Afe77xxFtdiAccessor();
    ~Afe77xxFtdiAccessor();

    bool readRegisters(uint16_t address, unsigned char *buffer, unsigned int len);
    bool writeRegisters(uint16_t address, unsigned char *buffer, unsigned int len);

private: 
    bool m_inited = false;
    FT_HANDLE m_handle = nullptr;

    bool waitSpiAccess();
    bool writeRegister(uint16_t address, unsigned char value);
    bool readRegister(uint16_t address, unsigned char& value);
};

