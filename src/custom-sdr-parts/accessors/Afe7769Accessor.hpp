#pragma once
#include <memory>

#include "Afe7769RficTypes.hpp"
#include "RficSystemDeviceParam.hpp"

#include "AbstractAccessor.hpp"
#include "AbstractSpiBackend.hpp"

#include "tiAfe77_afeGlobalConstants.h"

class Afe7769Accessor: public AbstractAccessor, public afe77InstDeviceInfoDef
{
public:
    Afe7769Accessor(const AbstractSpiBackend::Ptr& backend);
    AccessorType type() const override;

    bool write(uint16_t address, const uint8_t value);
    bool read(uint16_t address, uint8_t* value);

    bool burstRead(uint16_t address, uint8_t* values, uint32_t length);
    bool burstWrite(uint16_t address, const uint8_t* values, uint32_t length);

    void wait(int msec);

    bool readWrite(uint16_t address, const uint8_t value, uint8_t lsb, uint8_t msb);
    bool writeProperty(const std::string& name, uint16_t address, uint32_t value, uint8_t lsb, uint8_t msb);

    void afe77SystemSpiInterfaceSetup(afe77InstDeviceInfoDef* inst);

    void init();

private:
    AbstractSpiBackend::Ptr m_backend;

    static Afe7769Accessor* m_instance;
};

