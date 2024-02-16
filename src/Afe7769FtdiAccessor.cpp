#include "Afe7769FtdiAccessor.hpp"
#include <string.h>

#include <thread>
#include <chrono>
#include <memory>
#include <functional>
#include <vector>

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

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
#define AFE_MACRO_RESULT_START_REG_ADDR  0x104

#define CHECK_RESULT_2(X,Y) if(X==-1) {__DEBUG_ERROR__(Y); return; }

Afe7769FtdiAccessor::Afe7769FtdiAccessor()
{
    int result = -1;

    m_fd = open("/dev/spidev1.0", O_RDWR);
    CHECK_RESULT_2(m_fd, "Can't open device.");

	result = ioctl(m_fd, SPI_IOC_WR_MODE32, &m_mode);
    CHECK_RESULT_2(result, "Can't set spi mode.");

	result = ioctl(m_fd, SPI_IOC_RD_MODE32, &m_mode);
    CHECK_RESULT_2(result, "Can't get spi mode.");

	result = ioctl(m_fd, SPI_IOC_WR_BITS_PER_WORD, &m_bits);
    CHECK_RESULT_2(result, "Can't set bits per word");

    result = ioctl(m_fd, SPI_IOC_RD_BITS_PER_WORD, &m_bits);
    CHECK_RESULT_2(result, "Can't get bits per word");

    result = ioctl(m_fd, SPI_IOC_WR_MAX_SPEED_HZ, &m_speed);
    CHECK_RESULT_2(result, "Can't set max speed hz");

    result = ioctl(m_fd, SPI_IOC_RD_MAX_SPEED_HZ, &m_speed);
    CHECK_RESULT_2(result, "Can't get max speed hz");
}

AccessorType Afe7769FtdiAccessor::type() const {
    return AccessorType::AFE7769;
}


bool Afe7769FtdiAccessor::rawWrite(uint8_t *data, unsigned length)
{
    return rawWriteAndRead(data, nullptr, length);
}

bool Afe7769FtdiAccessor::rawWriteAndRead(uint8_t *data, uint8_t *result, unsigned length)
{
	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long) data,
		.rx_buf = (unsigned long) result,
		.len = length,
		.delay_usecs = 0,
		.bits_per_word = 0,
	};

    if(ioctl(m_fd, SPI_IOC_MESSAGE(1), &tr) == -1) {
        return false;
    }

    return true;
}


bool Afe7769FtdiAccessor::init() {

    auto t1 = std::chrono::high_resolution_clock::now();
    CHECK_RESULT(write(0x00, 0x30));
    auto t2 = std::chrono::high_resolution_clock::now();

    auto mcs_int = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
    std::cout << mcs_int.count() << "mcs\n";

    CHECK_RESULT(write(0x00, 0xB0));
    CHECK_RESULT(write(0x00, 0x30));

    CHECK_RESULT(write(0x01, 0x00));

    auto info = getChipInfo();
    __DEBUG_INFO__(
        + " chip_type: "     + std::to_string(info.chip_type)
        + ", chip_id: "      + std::to_string(info.chip_id)
        + ", chip_version: " + std::to_string(info.chip_version)
        + ", vendor_id: "    + std::to_string(info.vendor_id));
    
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

Afe7769ChipInfo Afe7769FtdiAccessor::getChipInfo()
{
    Afe7769ChipInfo ci;
    uint8_t read_packet[6];
    burstRead(0x03, read_packet, 6);
    ci.chip_type = read_packet[0];
    ci.chip_id = (((int)read_packet[2])<<8) + read_packet[1];
    ci.chip_version = read_packet[3];
    ci.vendor_id = (((int)read_packet[5])<<8) + read_packet[4];

    return ci;
}

bool Afe7769FtdiAccessor::write(uint16_t address, uint8_t value)
{
    uint8_t packet[3] = {(uint8_t)(address>>8), (uint8_t)address, value};
    return rawWrite(packet, 3);
}

bool Afe7769FtdiAccessor::read(uint16_t address, uint8_t * value)
{
    uint8_t read_packet[3] = {0};
    uint8_t write_packet[3] = {(uint8_t)(address>>8) | 0x80, (uint8_t)address, 0x00};

    if(!rawWriteAndRead(write_packet, read_packet, 3))
    {
        return false;
    }

    if (value)
        *value = read_packet[2];
    return true;
}

bool Afe7769FtdiAccessor::burstRead(uint16_t address, uint8_t *values, uint32_t length)
{
    std::vector<uint8_t> data(length + 2, 0);
    data[0] = (address>>8) | 0x80;
    data[1] = address;

    if(!rawWriteAndRead(data.data(), data.data(), length + 2))
    {
         __DEBUG_ERROR__("Can`t write registers, "
                "address:" + std::to_string(address));

        return false;
    }
    std::copy(data.begin()+2, data.end(), values);
    return true;
}

bool Afe7769FtdiAccessor::burstWrite(uint16_t address, uint8_t *values, uint32_t length)
{
    std::vector<uint8_t> data(length + 2);
    data[0] = address>>8;
    data[1] = address;
    std::copy(values, values+length, data.begin()+2);

    if(!rawWrite(data.data(), length + 2))
    {
         __DEBUG_ERROR__("Can`t write registers, "
                "address:" + std::to_string(address));

        return false;
    }
    return true;
}


bool Afe7769FtdiAccessor::waitUntilMacroReady(int msec)
{
    CHECK_RESULT(write(AFE_MACRO_PAGE_REG_ADDR, AFE_MACRO_PAGE_SEL_VAL)); 

    int i = 0;
    int milisec = 10;
    int count = msec  / milisec;

    uint8_t value = 0;
	while ((!(value & 1)) && (i++ < count)) 
    {
        wait(milisec);
        CHECK_RESULT(read(AFE_MACRO_STATUS_REG_ADDR, &value)); // SBAU329 -> addr: 0x20
	}

//    std::cout << "last ready-status value: "
//              << std::hex << (uint32_t)value
//              << std::dec << std::endl;

	CHECK_RESULT(write(AFE_MACRO_PAGE_REG_ADDR, 0x00));
    return --i * milisec < count;
}

bool Afe7769FtdiAccessor::waitUntilMacroDone(int msec)
{
    CHECK_RESULT(write(AFE_MACRO_PAGE_REG_ADDR, AFE_MACRO_PAGE_SEL_VAL));

    int i = 0;
    int milisec = 10;
    int count = msec / milisec;

    uint8_t value = 0;
	while ((!(value & 4)) && (i++ < count)) 
    {
        wait(milisec);
        CHECK_RESULT(read(AFE_MACRO_STATUS_REG_ADDR, &value)); // SBAU329 -> addr: 0x20
	}

//    std::cout << "last done-status value: "
//              << std::hex << (uint32_t)value
//              << std::dec << std::endl;

	CHECK_RESULT(write(AFE_MACRO_PAGE_REG_ADDR, 0x00));
    return --i * milisec < count;
}

bool Afe7769FtdiAccessor::executeMacro (uint32_t* operands, uint16_t len, uint8_t opcode)
{
	CHECK_RESULT(write(AFE_MACRO_PAGE_REG_ADDR, AFE_MACRO_PAGE_SEL_VAL));
    CHECK_RESULT(burstWrite(AFE_MACRO_OPERAND_START_REG_ADDR, (uint8_t*)operands, len*4));
    
    /*for (uint16_t i = 0; i < len; i++)
        CHECK_RESULT(write(0x0a0 + i, operands[i]));*/ // SBAU329 -> addr: 0x48

//    const int size = len * 4;
//    std::vector<uint8_t> values(size);

//    uint8_t == values.data();
//    for (size_t i = 0; i < len; i++) {
//        *p++ = operands[i] >> 0;
//        *p++ = operands[i] >> 8;
//        *p++ = operands[i] >> 16;
//        *p++ = operands[i] >> 24;

//        std::cout << "write macro operand " << i << " value "
//                  << std::hex << " " << operands[i]
//                  << std::dec << std::endl;
//    }
//    CHECK_RESULT(burstWrite(AFE_MACRO_OPERAND_START_REG_ADDR, values, size));


	CHECK_RESULT(write(AFE_MACRO_OPCODE_REG_ADDR, opcode));
//    std::cout << "write opcode: " << std::hex << (int)opcode
//                                  << std::dec << std::endl;
	CHECK_RESULT(write(AFE_MACRO_PAGE_REG_ADDR, 0x00));
    return true;
}

bool Afe7769FtdiAccessor::executeMacro(uint32_t operand, uint8_t opcode)
{
    return executeMacro(&operand, 1, opcode);
}

bool Afe7769FtdiAccessor::readMacroResultRegister (uint32_t * result, uint16_t len)
{
	CHECK_RESULT(write(AFE_MACRO_PAGE_REG_ADDR, AFE_MACRO_PAGE_SEL_VAL));
    CHECK_RESULT(burstRead(AFE_MACRO_RESULT_START_REG_ADDR, (uint8_t*)result, len*4));

//    uint16_t addr = AFE_MACRO_RESULT_START_REG_ADDR + numOfResReg * 4;

//    uint8_t values[4] = {0};
//    CHECK_RESULT(burstRead(addr, values, 4));

//    result = result | values[0];
//    result = result | (((uint32_t)values[1]) << 8);
//    result = result | (((uint32_t)values[2]) << 16);
//    result = result | (((uint32_t)values[3]) << 24);

//    std::cout << "read macro result nof " << (int)numOfResReg << " value: "
//              << std::hex << result
//              << std::dec << std::endl;

	CHECK_RESULT(write(AFE_MACRO_PAGE_REG_ADDR, 0x00));
    return true;
}
