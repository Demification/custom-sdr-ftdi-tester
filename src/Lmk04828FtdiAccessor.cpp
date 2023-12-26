#include "Lmk04828FtdiAccessor.hpp"
#include <math.h>
#include <sstream>
#include <iostream>
#include <algorithm>

#include "spi.h"
#include "ftdi.h"
#include "Debug.hpp"
#include "Lmk04828SetupRegisters.hpp"

Lmk04828FtdiAccessor::Lmk04828FtdiAccessor()
    : m_config { .sck  = 0x04,
                 .mosi = 0x08,
                 .miso = 0x01,
                 .nss  = 0x02 }
{
    if(initInfo() && 
       initDevice() && 
       initBitbangMode(m_config)) 
    {
        m_inited = true;
    }
    else __DEBUG_INFO__("Can`t init Lmk04828 accessor."); 
}

Lmk04828FtdiAccessor::~Lmk04828FtdiAccessor() {
    if(m_handle) FTD2_Close(m_handle);
}

bool Lmk04828FtdiAccessor::setup() {
    if(!m_inited) return false;

    std::stringstream stream(Lmk04828SetupRegisters);

    std::string line;
    while(std::getline(stream, line, '\n')) {
        if(line.empty()) continue;

        auto hexstr = line.substr(line.find_first_of("x") - 1, 8);
        auto value = strtoul(hexstr.c_str(), nullptr, 0);

        unsigned char data[3];
        for (int i = 0; i < 3; ++i)
            data[2-i] = value >> (i * 8);

        unsigned int written;
        if(!bitbangWrite(m_config, data, 3, &written)){
            return false;
        }
    }

    return true;
}

bool Lmk04828FtdiAccessor::writeRegister(uint16_t address, uint8_t value) {
    if(!m_inited) return false;

    union __attribute__((__packed__, aligned(8))) 
    Packet {
		unsigned char data[3];
		struct __attribute__((packed)) {
			uint8_t  rw_bit  : 1;
            uint8_t  mul_byte: 2;
			uint16_t address : 13;
            uint8_t  value   : 8;
		};
    } packet;

    packet.rw_bit = 0;
    packet.mul_byte = 0;
    packet.address = address;
    packet.value = value;

    unsigned int written = 0;
    return bitbangWrite(m_config, packet.data, 3, &written);
}

double Lmk04828FtdiAccessor::setPllRefClk(double frequency) {
    const double LMK04828_VCXCO_FREQ = 122.88e6;
    const double LMK04828_PLL1_MAX_FPFD = 1.2e6;
    const double LMK04828_PLL1_MIN_FPFD = 80e3;
    const double LMK04828_MAX_R_DIV = 16363L;
    const double LMK04828_MAX_N_DIV = 16363L;
    const double LMK04828_MAX_DLD_CNT = 16363L;
    const double LMK04828_DLD_PREC = 5e-7;
    const double LMK04828_WND_SIZE = 4e-9;

    int dividerN = 0, 
        dividerR = floor(frequency / LMK04828_PLL1_MAX_FPFD);

    double fpfd;

    bool valid = false;
    while ((fpfd = frequency / dividerR) >= LMK04828_PLL1_MIN_FPFD 
        && dividerR <= LMK04828_MAX_R_DIV)
    {
	    double Nf = LMK04828_VCXCO_FREQ / fpfd;
	    dividerN = (int)Nf;
	    if (dividerN > 8191) 
            break;
	    if (dividerN == Nf) {
            valid = true; 
            break;} //integer value
	    ++dividerR;
    }

    auto hi = [](int a) -> unsigned char {return ((a >> 8) & 0xff);};
    auto lo = [](int a) -> unsigned char {return (a & 0xff);};
    auto setRegister = [&](uint16_t reg, uint8_t value) {
        if(!writeRegister(reg, value)) 
            std::cerr << "Register write error: @" << std::hex << reg << "=" << 
                                                      std::hex << value << std::endl;
    };

    if (valid) {
	    //@0x155..0x156 <= R;
        setRegister(0x155, hi(dividerR));
        setRegister(0x156, lo(dividerR));

 	    //@0x159..0x15A <= N;
        setRegister(0x159, hi(dividerN));
        setRegister(0x15A, lo(dividerN));

	    int cp = (std::min(lround(0.02 * dividerN - 0.5), 15L)) | 0x10;

	    //@0x15B <= CP | 0x10;
        setRegister(0x15B, (uint8_t)cp);
	    
        double value = lround(fpfd * (LMK04828_WND_SIZE / LMK04828_DLD_PREC));
        int dldCnt = std::min(value, LMK04828_MAX_DLD_CNT);

	    //@0x15C..0x15D <= dldCnt;
        setRegister(0x15C, hi(dldCnt));
        setRegister(0x15D, lo(dldCnt));
    }
    else {
        __DEBUG_ERROR__("Frequency not valid.");
        return -1;
    }

    return frequency;
}

bool Lmk04828FtdiAccessor::initInfo() {
    DWORD count, result;
    if(FT_OK != (result = FTD2_CreateDeviceInfoList(&count))) {
        __DEBUG_ERROR__("Can`t create device info list."
            " Result: " + std::to_string(result));
        return false;
    }

    if(!count){
        __DEBUG_ERROR__("No spi devices found.");
        return false;
    }

    auto bsz = sizeof(FT_DEVICE_LIST_INFO_NODE);
    auto info = (FT_DEVICE_LIST_INFO_NODE*)malloc(bsz * count);

    if(FT_OK != FTD2_GetDeviceInfoList (info, &count)) {
        __DEBUG_ERROR__("Can`t get device info list.");
        return false;
    }

    for (size_t i = 0; i < count; i++) {
        m_info.push_back(info[i]);
        __DEBUG_INFO__(info[i].Description);
    }
    
    free(info);
    return true;
}

bool Lmk04828FtdiAccessor::initDevice() {
    auto result = FT_OK == FTD2_OpenEx((PVOID)"Quad RS232-HS D", 
                                       FT_OPEN_BY_DESCRIPTION, &m_handle);

    if(!result) __DEBUG_ERROR__("Can`t open device."); 
    return result;
}

bool Lmk04828FtdiAccessor::initBitbangMode(const BitbangConfig &config) {
    m_bitbangBuff.resize(m_maxBurstLen * 16 + 3);

    FT_STATUS result;
    if(FT_OK == (result = FTD2_SetBitmode(m_handle, 
                                          config.nss|config.sck|config.mosi, 
                                          FT_BITMODE_ASYNC_BITBANG)))
    {
        unsigned char nss = config.nss;
        unsigned int wrtn = 0;
        result = FTD2_Write(m_handle, &nss, 1, &wrtn);

        if (wrtn == 1 && result == FT_OK) 
            return true;
        else 
            __DEBUG_ERROR__("Can`t write nss."); 
    }
    else __DEBUG_ERROR__("Can`t set bitbang mode."); 

    return false;
}

bool Lmk04828FtdiAccessor::          bitbangWrite(const BitbangConfig& config, 
                                        unsigned char *data, 
                                        unsigned len, 
                                        unsigned int *written)
{
    unsigned char * p = m_bitbangBuff.data();
    *p++ = 0; //nSS down
    unsigned cnt = 0;
    while (cnt < len && cnt < m_maxBurstLen)
    {
        unsigned char byte = *data++;
        for (unsigned k=0;k<8;++k) //bit counter
        {
            unsigned char b = 0;
            if (byte & 0x80) b |= config.mosi;
            *p++ = b; //sck down, change mosi
            *p++ = b | config.sck; //sck up, mosi do not shanged
            byte <<= 1;
        }
        cnt++;
    }
    *p++ = config.nss | config.sck; //nSS up,
    *p = config.nss; //sck down

    unsigned int wrtn = 0;
    if(FT_OK != FTD2_Write(m_handle, m_bitbangBuff.data(), cnt * 16 + 3, &wrtn)) {
        __DEBUG_ERROR__("Can`t write register in bitbang mode."); 
        return false;
    }
    if (written)
        *written = wrtn>=3 ? (wrtn-3)/16 : 0;

    return true;
}
