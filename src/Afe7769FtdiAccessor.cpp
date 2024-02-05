#include "Afe7769FtdiAccessor.hpp"
#include <string.h>

#include <thread>
#include <chrono>
#include <memory>
#include <functional>

#include "spi.h"
#include "afe77xx.h"
#include "Debug.hpp"

#define CHECK_RESULT(X) if(!X) {__DEBUG_ERROR__("Fail result."); return false; }
#define MASK_BYTE(lsb, msb) (uint8_t)(((1 << ((msb) - (lsb) + 1)) - 1) << lsb)
#define MASK_SHORT(lsb, msb) (uint16_t)(((1 << ((msb) - (lsb) + 1)) - 1) << lsb)

#define AFE_MACRO_PAGE_REG_ADDR          0x13
#define AFE_MACRO_PAGE_SEL_VAL           0x10
#define AFE_MACRO_OPCODE_REG_ADDR        0x193
#define AFE_MACRO_STATUS_REG_ADDR        0xf0
#define AFE_MACRO_OPERAND_START_REG_ADDR 0xa0
#define AFE_MACRO_RESULT_START_REG_ADDR  0xf8 // 0x104

Afe7769FtdiAccessor::Afe7769FtdiAccessor(FtdiDeviceInfoList::Ptr infoList) 
    : FtdiSpiAccessProvider(ChannelConfig_t {.ClockRate = 10000000,
                                             .LatencyTimer = 0,
                                             .configOptions = SPI_CONFIG_OPTION_MODE0 | 
                                                              SPI_CONFIG_OPTION_CS_DBUS3 | 
                                                              SPI_CONFIG_OPTION_CS_ACTIVELOW, 
                                             .Pin = 0x0BUL | (0x08UL << 8) | (0x0BUL << 16) | (0x08UL << 24),
                                             .currentPinState = 0x0BUL | (0x08UL << 8) }, 
                            infoList->getByIndex(0), 0) 
{
    if(!isInitedMpsseMode()) {
        __DEBUG_INFO__("Afe7769 ftdi accessor not inited.");
        return;
    }
}

AccessorType Afe7769FtdiAccessor::type() const {
    return AccessorType::AFE7769;
}

bool Afe7769FtdiAccessor::init() {
    if(!isInitedMpsseMode()) return false;

    CHECK_RESULT(write(0x000, 0x80));
    CHECK_RESULT(write(0x000, 0x30));
    CHECK_RESULT(write(0x001, 0x00));

    auto info = getChipInfo();
    __DEBUG_INFO__(std::string("Inited afe7769 device: ") + deviceInfo()->description + ".\n"
        + " chip_type: "     + std::to_string(info->chip_type)
        + ", chip_id: "      + std::to_string(info->chip_id)
        + ", chip_version: " + std::to_string(info->chip_version)
        + ", vendor_id: "    + std::to_string(info->vendor_id));
    
    /*CHECK_RESULT(write(0x16, 0x10));
    for (size_t i = 0x40; i < 0x40 + 32; i++)
    {
        uint8_t value;
        CHECK_RESULT(read(i, value));
        std::cout << std::hex << "addr: 0x" << i << "  value: 0x" << (int)value << std::endl;
    }
    CHECK_RESULT(write(0x15, 0x00));*/

    /*auto rxDsaPhaseRead = [&](uint32_t ch, uint32_t dsaLimit, int* phaseRead){
	    int i;
	    uint8_t temp;
	
	    write(0x0016, (0x10<<(ch>>1)));	
	    for(i=0;i<dsaLimit; i++){
            int t = (0x03B0+(0x360*(ch&0x1))+4*i);
		    read((0x03B0+(0x360*(ch&0x1))+4*i),temp);
		    phaseRead[i] = temp;
		    read((0x03B1+(0x360*(ch&0x1))+4*i),temp);
		    phaseRead[i] = phaseRead[i] + (((uint32_t)temp)<<8);
		    printf("Phase read for DSA %d is %d. \n", i, phaseRead[i]);
	    }
	    write(0x0016, 0x00);
    };

    int rxDsaPhaseReadValues[128];
    rxDsaPhaseRead(0, 128, rxDsaPhaseReadValues);*/

    return true;
}

void *Afe7769FtdiAccessor::handle() {
    return FtdiSpiAccessProvider::handle();
}

bool Afe7769FtdiAccessor::setRficSampleRate(
    customsdr_rfic_channel ch, customsdr_rfic_sample_rate rate) 
{ 
    return false; 
};

bool Afe7769FtdiAccessor::getRficSampleRate(
    customsdr_rfic_channel ch, customsdr_rfic_sample_rate& samplerate) 
{ 
    return false; 
};

bool Afe7769FtdiAccessor::setRficBandwidth(
    customsdr_rfic_channel ch, customsdr_rfic_bandwidth bandwidth) 
{ 
    return false; 
};

bool Afe7769FtdiAccessor::getRficBandwidth(
    customsdr_rfic_channel ch, customsdr_rfic_bandwidth& bandwidth) 
{ 
    return false; 
};

bool Afe7769FtdiAccessor::setRficFrequency(
    customsdr_rfic_channel ch, double frequency) 
{ 
    return false; 
};

bool Afe7769FtdiAccessor::getRficFrequency(
    customsdr_rfic_channel ch, double& frequency) 
{ 
    return false; 
};

bool Afe7769FtdiAccessor::getRficFrequencyRange(
    customsdr_rfic_channel ch, Range& range) 
{ 
    return false; 
};

bool Afe7769FtdiAccessor::setRficLowIfMode(
    customsdr_rfic_channel ch, customsdr_rfic_low_if mode) 
{ 
    return false; 
};

bool Afe7769FtdiAccessor::getRficLowIfMode(
    customsdr_rfic_channel ch, customsdr_rfic_low_if& mode) 
{ 
    return false; 
};

bool Afe7769FtdiAccessor::setRficNcoFrequency(
    customsdr_rfic_channel ch, double frequency) 
{ 
    return false; 
};

bool Afe7769FtdiAccessor::getRficNcoFrequency(
    customsdr_rfic_channel ch, double& frequency) 
{ 
    return false; 
};

bool Afe7769FtdiAccessor::getRficNcoFrequencyRange(
    customsdr_rfic_channel ch, Range& range) 
{ 
    return false; 
};

bool Afe7769FtdiAccessor::setRficBand(
    customsdr_rfic_channel ch, customsdr_rfic_band band) 
{ 
    return false; 
};

bool Afe7769FtdiAccessor::setRficGain(
    customsdr_rfic_channel ch, float gain) 
{ 
    return false; 
};

bool Afe7769FtdiAccessor::getRficGain(
    customsdr_rfic_channel ch, float& gain)
{ 
    return false; 
};

bool Afe7769FtdiAccessor::getRficGainRange(
    customsdr_rfic_channel ch, Range& range)
{ 
    return false; 
};

bool Afe7769FtdiAccessor::setRficGainMode(
    customsdr_rfic_channel ch, customsdr_rfic_gain_mode mode)
{ 
    return false; 
};

bool Afe7769FtdiAccessor::getRficGainMode(
    customsdr_rfic_channel ch, customsdr_rfic_gain_mode& mode)
{ 
    return false; 
};

bool Afe7769FtdiAccessor::setRficAgcParam(
    customsdr_rfic_channel ch, customsdr_rfic_agc_param  param)
{ 
    return false; 
};

bool Afe7769FtdiAccessor::getRficAgcParam(
    customsdr_rfic_channel ch, customsdr_rfic_agc_param& param)
{ 
    return false; 
};

bool Afe7769FtdiAccessor::setRficDgcMode(
    customsdr_rfic_channel ch, int mode)
{ 
    return false; 
};

bool Afe7769FtdiAccessor::getRficDgcMode(
    customsdr_rfic_channel ch, int& mode)
{ 
    return false; 
};

bool Afe7769FtdiAccessor::rficEnableModule(
    customsdr_rfic_channel ch, bool enable)
{ 
    return false; 
}

void Afe7769FtdiAccessor::wait(int millisec){
    std::this_thread::sleep_for(
        std::chrono::milliseconds(millisec));
}

Afe7769ChipInfo::Ptr Afe7769FtdiAccessor::getChipInfo() {
    auto ptr = Afe7769ChipInfo::Ptr(new Afe7769ChipInfo);
    memset(ptr.get(), 0 , sizeof(Afe7769ChipInfo));

    read(0x03, ptr->chip_type);
    read(0x04, reinterpret_cast<uint8_t*>(&ptr->chip_id)[0]);
    read(0x05, reinterpret_cast<uint8_t*>(&ptr->chip_id)[1]);
    read(0x06, ptr->chip_version);
    read(0x07, reinterpret_cast<uint8_t*>(&ptr->vendor_id)[0]);
    read(0x08, reinterpret_cast<uint8_t*>(&ptr->vendor_id)[1]);

    return ptr;
};

bool Afe7769FtdiAccessor::write(
    uint16_t address, uint8_t value)
{
    auto paddr = ((uint8_t*)&address) + 1;
    uint8_t packet[3] = {*paddr, *(--paddr), value};

    return mpsseWrite(packet, 3);
}

bool Afe7769FtdiAccessor::read(
    uint16_t address, uint8_t &value)
{
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

bool Afe7769FtdiAccessor::burstRead(
    uint16_t address, uint8_t *values, uint32_t length)
{
    auto size = length + 2;
    auto data = std::unique_ptr<uint8_t[]>(new uint8_t[size]);

    auto paddr = ((uint8_t*)&address) + 1;
    data[0] = *paddr | 0x80;
    data[1] = *(--paddr);

    memset(&data[2], 0, length);

    if(!mpsseWriteAndRead(data.get(), data.get(), size))
    {
         __DEBUG_ERROR__("Can`t write registers, "
                "address:" + std::to_string(address));

        return false;
    }

    memcpy(values, &data[2], length);
    return true;
}

bool Afe7769FtdiAccessor::burstWrite(
    uint16_t address, uint8_t *values, uint32_t length)
{
    auto size = length + 2;
    auto data = std::unique_ptr<uint8_t[]>(new uint8_t[size]);

    auto paddr = ((uint8_t*)&address) + 1;
    data[0] = *paddr;
    data[1] = *(--paddr);

    memcpy(&data[2], values, length);

    if(!mpsseWrite(data.get(), size)) {
         __DEBUG_ERROR__("Can`t write registers, "
                "address:" + std::to_string(address));

        return false;
    }
    return true;
}

bool Afe7769FtdiAccessor::waitUntilMacroReady(int sec) {
    CHECK_RESULT(write(AFE_MACRO_PAGE_REG_ADDR, AFE_MACRO_PAGE_SEL_VAL)); 

    int i = 0;
    int milisec = 100;
    int count = sec * 1000 / milisec;

    uint8_t value = 0;
	while ((!(value & 1)) && (i++ < count)) 
    {
        wait(milisec);
		CHECK_RESULT(read(AFE_MACRO_STATUS_REG_ADDR, value)); // SBAU329 -> addr: 0x20
	}

    std::cout << "last ready-status value: " 
              << std::hex << (uint32_t)value 
              << std::dec << std::endl;

	CHECK_RESULT(write(AFE_MACRO_PAGE_REG_ADDR, 0x00));
    return --i * milisec < count;
}

bool Afe7769FtdiAccessor::waitUntilMacroDone(int sec) {
    CHECK_RESULT(write(AFE_MACRO_PAGE_REG_ADDR, /*AFE_MACRO_PAGE_SEL_VAL*/ 0x20));

    int i = 0;
    int milisec = 100;
    int count = sec * 1000 / milisec;

    uint8_t value = 0;
	while ((!(value & 4)) && (i++ < count)) 
    {
        wait(milisec);
		CHECK_RESULT(read(AFE_MACRO_STATUS_REG_ADDR, value)); // SBAU329 -> addr: 0x20
	}

    std::cout << "last done-status value: " 
              << std::hex << (uint32_t)value 
              << std::dec << std::endl;

	CHECK_RESULT(write(AFE_MACRO_PAGE_REG_ADDR, 0x00));
    return --i * milisec < count;
}

bool Afe7769FtdiAccessor::executeMacro (
    uint32_t* operands, uint16_t len, uint8_t opcode)
{
	CHECK_RESULT(write(AFE_MACRO_PAGE_REG_ADDR, AFE_MACRO_PAGE_SEL_VAL));
    
    /*for (uint16_t i = 0; i < len; i++)
        CHECK_RESULT(write(0x0a0 + i, operands[i]));*/ // SBAU329 -> addr: 0x48

    const int size = len * 4;
    uint8_t values[size] = {0};

    for (size_t i = 0; i < len; i++) {
        auto p = &values[i * 4];

        p[0] = operands[i] >> 0;
        p[1] = operands[i] >> 8;
        p[2] = operands[i] >> 16;
        p[3] = operands[i] >> 24;

        std::cout << "write macro operand " << i << " value "
                  << std::hex << " " << (int)p[0] 
                                     << ":" << (int)p[1] 
                                     << ":" << (int)p[2] 
                                     << ":" << (int)p[3]
                  << std::dec << std::endl;
    }          
    CHECK_RESULT(burstWrite(AFE_MACRO_OPERAND_START_REG_ADDR, values, len));

	CHECK_RESULT(write(AFE_MACRO_OPCODE_REG_ADDR, opcode));
    std::cout << "write opcode: " << std::hex << (int)opcode 
                                  << std::dec << std::endl;

    /*CHECK_RESULT(write(0x192, 0x00));
    CHECK_RESULT(write(0x191, 0x00));
    CHECK_RESULT(write(0x190, 0x00));*/

	CHECK_RESULT(write(AFE_MACRO_PAGE_REG_ADDR, 0x00));
    return true;
}

bool Afe7769FtdiAccessor::readMacroResultRegister (
    uint8_t numOfResReg, uint32_t &result)
{
	CHECK_RESULT(write(AFE_MACRO_PAGE_REG_ADDR, AFE_MACRO_PAGE_SEL_VAL));
    uint16_t addr = AFE_MACRO_RESULT_START_REG_ADDR + numOfResReg * 4;

    uint8_t values[4] = {0};
    CHECK_RESULT(burstRead(addr, values, 4));

    result = result | values[0];
    result = result | (((uint32_t)values[1]) << 8);
    result = result | (((uint32_t)values[2]) << 16);
    result = result | (((uint32_t)values[3]) << 24);

    std::cout << "read macro result nof " << (int)numOfResReg << " value: "
              << std::hex << result
              << std::dec << std::endl;

	CHECK_RESULT(write(AFE_MACRO_PAGE_REG_ADDR, 0x00));
    return true;
}
