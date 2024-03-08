#include "Afe7769Accessor.hpp"
#include <string.h>
#include <memory>
#include <thread>
#include <memory>
#include <functional>

#include "Debug.hpp"

#include "tiAfe77_init.h"
#include "tiAfe77_controls.h"
#include "tiAfe77_jesd.h"
#include "tiAfe77_macro.h"
#include "tiAfe77_macroInterface.h"

#define DEBUG_AFE_SPI

Afe7769Accessor* Afe7769Accessor::m_instance = nullptr;

Afe7769Accessor::Afe7769Accessor(const AbstractSpiBackend::Ptr& backend) 
    : m_backend(backend) 
{ m_instance = this; }

AccessorType Afe7769Accessor::type() const {
    return AccessorType::AFE7769;
}

void Afe7769Accessor::init() {
    afe77SystemSpiInterfaceSetup(this);

    if(!m_backend->isInited() || !bringup(this)) {
        __DEBUG_INFO__("Afe7769 backend not inited.");
        return;
    }

    uint32_t chipId;
    afe77ChipInfoStruct chipInfo;

    if(getChipInfo(this, &chipInfo) && 
       systemCommandMacro(this, GetChipIdOpValue, &chipId) && 
       chipId == chipInfo.chip_id)
    {
        __DEBUG_INFO__(std::string("Afe7769 info:") 
            + " chip_id: " + std::to_string(chipInfo.chip_id)
            + ", chip_version: "  + std::to_string(chipInfo.chip_version));
    }
    else __DEBUG_ERROR__("Fail macro execution.");
}

bool Afe7769Accessor::write(
    uint16_t address, const uint8_t value)
{
    #ifdef DEBUG_AFE_SPI
    std::cout << "write " << std::hex 
              << "0x" << int(address) << " 0x" << int(value)
              << std::dec << std::endl;
    #endif

    uint8_t packet[3] = {(uint8_t)(address>>8), (uint8_t)address, value};
    return m_backend->write(packet, 3);
}

bool Afe7769Accessor::read(
    uint16_t address, uint8_t* value)
{
    uint8_t read_packet[3] = {0};
    uint8_t lsbaddr = ((uint8_t)(address>>8) | 0x80);
    uint8_t write_packet[3] = {lsbaddr, (uint8_t)address, 0x00};

    if(!m_backend->writeAndRead(write_packet, read_packet, 3))
    {
        return false;
    }

    if (value)
        *value = read_packet[2];
    return true;
}

bool Afe7769Accessor::burstRead(
    uint16_t address, uint8_t *values, uint32_t length)
{
    std::vector<uint8_t> data(length + 2, 0);
    data[0] = (address>>8) | 0x80;
    data[1] = address;

    if(!m_backend->writeAndRead(data.data(), data.data(), length + 2))
    {
         __DEBUG_ERROR__("Can`t write registers, "
                "address:" + std::to_string(address));

        return false;
    }
    std::copy(data.begin()+2, data.end(), values);
    return true;
}

bool Afe7769Accessor::burstWrite(
    uint16_t address, const uint8_t *values, uint32_t length)
{
    #ifdef DEBUG_AFE_SPI
    std::cout << "burst write " << std::hex << "0x" << address << " {" ;
    for (size_t i = 0; i < length; i++)
        std::cout << "0x" << (int)values[i] << " ";
    std::cout << "}" << std::dec << std::endl;
    #endif
    
    std::vector<uint8_t> data(length + 2);
    data[0] = address >> 8;
    data[1] = address;
    std::copy(values, values+length, data.begin()+2);

    if(!m_backend->write(data.data(), length + 2))
    {
         __DEBUG_ERROR__("Can`t write registers, "
                "address:" + std::to_string(address));

        return false;
    }
    return true;
}

void Afe7769Accessor::wait(int msec)
{
    std::this_thread::sleep_for(
        std::chrono::milliseconds(msec));
}

bool Afe7769Accessor::readWrite(uint16_t address, const uint8_t value, uint8_t lsb, uint8_t msb)
{
    #ifdef DEBUG_AFE_SPI
    std::cout << "read-write 0x" << std::hex 
              << (int)address << " 0x" << (int)value 
              << std::dec << " lsb=" << (int)lsb << " msb=" << (int)msb 
              << std::endl;
    #endif

	uint8_t temp = 0;
	if(read(address, &temp)) {
	    for(int i = lsb; i <= msb; i = i + 1)
		    temp = (temp & (0xff - (1 << i))) | (value & (0x00 + (1 << i)));
    }
    else return false;
	return write(address,temp);	
}

bool Afe7769Accessor::writeProperty(const std::string& name, uint16_t address, uint32_t value, uint8_t lsb, uint8_t msb)
{
    #ifdef DEBUG_AFE_SPI
    std::cout << "\033[35m" << "property " << name 
              << " addr: 0x" << std::hex << int(address)
              << " val: 0x" << value;
    #endif

	uint32_t temp = 0;
	if(burstRead(address, (uint8_t*)&temp, 4)) {
        
        #ifdef DEBUG_AFE_SPI
        std::cout << " prev: 0x" << temp;
        #endif

        value = value << lsb;
	    for(int i = lsb; i <= msb; i = i + 1) 
            temp = (temp & (0xffffffffu - (1u << i))) | 
                (value & (0x00000000u + (1 << (i))));
    }
    else return false;

    #ifdef DEBUG_AFE_SPI
    std::cout << " new: 0x" << temp 
              << std::dec << " lsb=" << int(lsb) << " msb=" << int(msb)
              << "\033[0m" << std::endl;
    #endif

    for (size_t i = lsb / 8; i < msb/8 + (msb % 8 ? 1 : 0); i++)
        if(!write(address + i, ((uint8_t*)&temp)[i])) return false;
    
    return true;
}

void Afe7769Accessor::afe77SystemSpiInterfaceSetup(afe77InstDeviceInfoDef *inst)
{
    auto interface = const_cast<afe77SystemSpiInterface*>(&inst->spi);

    interface->write = [](uint16_t address, const uint8_t value) -> uint8_t {
        return m_instance->write(address, value) ? 1 : 0; };

    interface->read = [](uint16_t address, uint8_t* value) -> uint8_t {
        return m_instance->read(address, value) ? *value : 0; };

    interface->burstRead = [](uint16_t address, uint8_t* values, uint32_t length) -> uint8_t {
        return m_instance->burstRead(address, values, length) ? length : 0; };

    interface->burstWrite = [](uint16_t address, const uint8_t* values, uint32_t length) -> uint8_t {
        return m_instance->burstWrite(address, values, length) ? length : 0; };

    interface->readWrite = [](uint16_t address, const uint8_t value, uint8_t lsb, uint8_t msb) -> uint8_t {
        return m_instance->readWrite(address, value, lsb, msb) ? 1 : 0; };

    interface->writeProperty = [](const char* name, uint16_t address, uint32_t value, uint8_t lsb, uint8_t msb) -> uint8_t {
        return m_instance->writeProperty(name, address, value, lsb, msb) ? 1 : 0; };

    interface->wait = [](int msec) -> void { m_instance->wait(msec); };
};