#include "Lmk04828FtdiAccessor.hpp"
#include <math.h>
#include <string.h>

#include <sstream>
#include <iostream>
#include <algorithm>
#include <thread>
#include <chrono>

#include "spi.h"
#include "ftdi.h"
#include "Debug.hpp"
#include "Settings.hpp"
#include "Lmk04828InitRegisters.hpp"

Lmk04828FtdiAccessor::Lmk04828FtdiAccessor(FtdiDeviceInfoList::Ptr infoList)
    : FtdiSpiMemoryAccessor(BitbangConfig{ .sck  = 0x04, 
                                           .mosi = 0x08, 
                                           .miso = 0x01, 
                                           .nss  = 0x02 }, 
                            infoList->getByDescription("Quad RS232-HS D")) 
{
    if(isInitedBitbangMode())
        __DEBUG_INFO__("Lmk04828 ftdi accessor info:" + deviceInfo()->string());
}

bool Lmk04828FtdiAccessor::write(uint32_t dataToWrire) {
    return bitbangWrite(dataToWrire);
}

bool Lmk04828FtdiAccessor::read(uint32_t dataToWrire, uint32_t *readData) {
        unsigned char d[3];

    for (int k = 0; k < 3; ++k)
        d[2 - k] = dataToWrire >> (k * 8);

    bool result = false;
    unsigned int written;
    if(result = bitbangWriteAndRead(d, 3, &written)) {
        if(3 != written) 
            return false;

        *readData = d[2];
    }
    
    return result;
}

bool Lmk04828FtdiAccessor::init(double refClkFreq) {
    if(!isInitedBitbangMode()) return false;

    if(refClkFreq == -1.0) {
        __DEBUG_ERROR__("refClkFreq is -1.");
        return false;
    }
    bool externalRefFreqIsDefined = refClkFreq != 0.0 ? true : false;

    std::map<RegisterAddr, RegisterValue> registers;
    parseInitRegs(Lmk04828InitRegisters, registers);

    auto vcxcoTrim = Settings::instance().getPllConfig()->vcxco_trim;
    setVcxcoTrimRegs(vcxcoTrim, registers);

    if(externalRefFreqIsDefined) {
        computeInitRegsByFrequency(refClkFreq, registers);
    }

    if(!bitbangWrite(0x000090)) 
        __DEBUG_ERROR__("Can`t write: 0x000090");

    writeRegisters(registers);

    if(externalRefFreqIsDefined) {
        //only if external ref freq is defined
        initPll1();
    }

    //wait for PLL2 lock
    initPll2();

    //switch off sync mode
    if(!bitbangWrite(0x0144FF)) __DEBUG_ERROR__("Can`t write: 0x0144FF"); //switch off sync
    if(!bitbangWrite(0x013902)) __DEBUG_ERROR__("Can`t write: 0x013902"); //mux = sysref pulser
    if(!bitbangWrite(0x014313)) __DEBUG_ERROR__("Can`t write: 0x014313"); //sysref_clr = 0, spiPulser mode

    //power down of dealay circuits
    if(!bitbangWrite(0x014002)) __DEBUG_ERROR__("Can`t write: 0x014002"); //sysref_ddly_pd
    if(!bitbangWrite(0x0106F2)) __DEBUG_ERROR__("Can`t write: 0x0106F2"); //ch1 ddly_pd
    if(!bitbangWrite(0x010EF2)) __DEBUG_ERROR__("Can`t write: 0x010EF2"); //ch3 ddly_pd
    if(!bitbangWrite(0x0116F2)) __DEBUG_ERROR__("Can`t write: 0x0116F2"); //ch5 ddly_pd

    return true;
}

bool Lmk04828FtdiAccessor::sendSysref(void) {
    return bitbangWrite(0x013E00);
}

int Lmk04828FtdiAccessor::getLosStatus() {
    uint8_t ret;
    if(!bitbangWriteAndRead(0x818400, &ret)) {
        __DEBUG_ERROR__("Can`t read: 0x818400");
        return -1;
    }

    return ret & 0x02;
}

int Lmk04828FtdiAccessor::getPll1Lock() {
    uint8_t ret;
    if(!bitbangWriteAndRead(0x818200, &ret)) {
        __DEBUG_ERROR__("Can`t read: 0x818200");
        return -1;
    }

    return ret & 0x02;
}

int Lmk04828FtdiAccessor::getPll2Lock() {
    uint8_t ret;
    if(!bitbangWriteAndRead(0x818300, &ret)) {
        __DEBUG_ERROR__("Can`t read: 0x818300");
        return -1;
    }

    return ret & 0x02;
}

int Lmk04828FtdiAccessor::getHoldover() {
    uint8_t ret;
    if(!bitbangWriteAndRead(0x818800, &ret)){
        __DEBUG_ERROR__("Can`t read: 0x818300");
        return -1;
    }

    return ret & 0x10;
}

bool Lmk04828FtdiAccessor::readRegisters(uint16_t address,
                                         uint8_t *values,
                                         uint32_t length)
{
    if(!isInitedBitbangMode()) return false;
    
    for (size_t i = 0; i < length; i++) {
        if(!readRegister(address + i, values[i])) {
             __DEBUG_ERROR__("Can`t write register, "
                "address:" + std::to_string(address + i));
            
            continue;
        }
    }
    return true;
}

bool Lmk04828FtdiAccessor::writeRegisters(uint16_t address, 
                                         uint8_t *values, 
                                         uint32_t length) 
{
    if(!isInitedBitbangMode()) return false;

    for (size_t i = 0; i < length; i++) {
        if(!writeRegister(address + i, values[i])) {
             __DEBUG_ERROR__("Can`t write register, "
                "address:" + std::to_string(address + i));
            
            continue;
        }
    }
    return true;
}

bool Lmk04828FtdiAccessor::writeRegister(uint16_t address, uint8_t value) {
    auto paddr = ((uint8_t*)&address) + 1;
    uint8_t packet[3] = {*paddr, *(--paddr), value};

    unsigned int written = 0;
    return bitbangWrite(packet, 3, &written);
}

bool Lmk04828FtdiAccessor::readRegister(uint16_t address, uint8_t &value) {
    auto paddr = ((uint8_t*)&address) + 1;
    uint8_t packet[3] = {*paddr, *(--paddr), 0xff};
    packet[0] |= 0x80;

    bool result = false;
    unsigned int written;
    if(result = bitbangWriteAndRead((uint8_t*)&packet, 3, &written)) {
        if(3 != written) 
            return false;

        value = packet[2];
    }
    
    return result;
}

void Lmk04828FtdiAccessor::initPll1() {
    uint8_t ret;

    if (!(getLosStatus()))
    {
        if(!bitbangWrite(0x015008)) //holdover off, hitless_sw=0
            __DEBUG_ERROR__("Can`t write: 0x015008");

        if(!bitbangWrite(0x014B72)) //off holdover force, MAN_DAC_ENA=0
            __DEBUG_ERROR__("Can`t write: 0x014B72");

        //wait for PLL1 lock
        int cnt = 30; //3sec must be parametrized
        bool lock = false;
        do
        {
            if(getPll1Lock()) break;

            std::this_thread::sleep_for(std::chrono::milliseconds(100)); //replace polling by event

            if(getLosStatus()) break;
        }
        while (--cnt);

        if(!bitbangWrite(0x01500B)) //holdover on, hitless_sw=1
            __DEBUG_ERROR__("Can`t write: 0x01500B");

        if (!lock) {
            if(!bitbangWrite(0x014B7E)) //holdover force on, MAN_DAC_ENA=1
                __DEBUG_ERROR__("Can`t write: 0x014B7E");
        }
    }
}

void Lmk04828FtdiAccessor::initPll2() {
    uint8_t ret;
    int cnt = 10; //1sec must be parametrized
    bool lock = false;
    do
    {
        if (getPll2Lock()) break;

        std::this_thread::sleep_for(std::chrono::milliseconds(100)); //replace polling by event
    }
    while (--cnt);
}

bool Lmk04828FtdiAccessor::parseInitRegs(
    const std::string &text, std::map<RegisterAddr, RegisterValue> &registers)
{
    std::stringstream stream(text);

    std::string line;
    while(std::getline(stream, line, '\n')) {
        if(line.empty()) continue;

        auto hexstr = line.substr(line.find_first_of("x") - 1, 8);
        uint32_t hexvalue = strtoul(hexstr.c_str(), nullptr, 0);

        registers.emplace(hexvalue >> 8, hexvalue & 0xff);
    }

    return true;
}

bool Lmk04828FtdiAccessor::setVcxcoTrimRegs(
    uint16_t value, std::map<RegisterAddr, RegisterValue> &registers)
{
    if(value == -1) {
        __DEBUG_ERROR__("vcxcoTrim is -1.");
        return false;
    }

    uint8_t prev14Bvalue = registers.find(0x14B)->second;

    auto hi = [](uint16_t a) -> unsigned char {return ((a >> 8) & 0xff);};
    auto lo = [](uint16_t a) -> unsigned char {return (a & 0xff);};

    registers[0x14B] = (prev14Bvalue & 0xfc) | (hi(value) & 0x03);
    registers[0x14C] = lo(value);

    uint8_t a = registers.find(0x14B)->second;
    uint8_t b = registers.find(0x14C)->second;

    return true;
}

bool Lmk04828FtdiAccessor::computeInitRegsByFrequency(
    double frequency, std::map<RegisterAddr, RegisterValue>& registers)
{
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

    if (valid) {
	    //@0x155..0x156 <= R;
        registers[0x155] = hi(dividerR);
        registers[0x156] = lo(dividerR);

 	    //@0x159..0x15A <= N;
        registers[0x159] = hi(dividerN);
        registers[0x15A] = lo(dividerN);

	    int cp = (std::min(lround(0.02 * dividerN - 0.5), 15L)) | 0x10;

	    //@0x15B <= CP | 0x10;
        registers[0x15B] = (uint8_t)cp;
	    
        double value = lround(fpfd * (LMK04828_WND_SIZE / LMK04828_DLD_PREC));
        int dldCnt = std::min(value, LMK04828_MAX_DLD_CNT);

	    //@0x15C..0x15D <= dldCnt;
        registers[0x15C] = hi(dldCnt);
        registers[0x15D] = lo(dldCnt);
    }
    else {
        __DEBUG_ERROR__("Frequency not valid.");
        return false;
    }

    return true;
}

void Lmk04828FtdiAccessor::writeRegisters(
    std::map<RegisterAddr, RegisterValue> &registers) 
{
    for(auto reg: registers) {
        writeRegister(reg.first, reg.second);
    }
}