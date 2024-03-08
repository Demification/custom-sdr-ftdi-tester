#include "Lmk4828FtdiAccessor.hpp"
#include <math.h>
#include <string.h>

#include <sstream>
#include <iostream>
#include <algorithm>
#include <thread>
#include <chrono>
#include <iomanip> 

#include "spi.h"
#include "ftdi.h"
#include "Debug.hpp"
#include "Settings.hpp"
#include "Lmk4828InitRegisters.hpp"

Lmk4828FtdiAccessor::Lmk4828FtdiAccessor(FtdiDeviceInfoList::Ptr infoList)
    : FtdiSpiAccessProvider(BitbangConfig{ .sck  = 0x04, 
                                           .mosi = 0x08, 
                                           .miso = 0x01, 
                                           .nss  = 0x02 }, 
                            infoList->getByDescription("Quad RS232-HS D")) 
{
    if(!isInitedBitbangMode()) {
        return;
    }
}

AccessorType Lmk4828FtdiAccessor::type() const {
    return AccessorType::LMK4828;
}

bool Lmk4828FtdiAccessor::write(const uint32_t data) {
    return bitbangWrite(data);
}

bool Lmk4828FtdiAccessor::read(uint32_t data, uint8_t* value) {
    unsigned char d[3];

    for (int k = 0; k < 3; ++k)
        d[2 - k] = data >> (k * 8);

    bool result = false;
    unsigned int written;
    if(result = bitbangWriteAndRead(d, 3, &written)) {
        if(3 != written) 
            return false;

        *value = d[2];
    }
    
    return result;
}

void Lmk4828FtdiAccessor::init() {
    auto pllConfig = Settings::instance().getPllSystemDeviceConfig();
    if(!init(pllConfig->init_freq)) {
        __DEBUG_INFO__("Lmk4828 system device not inited.");
    }

    customsdr_pll_status value;
    status(value);

    int attempts = 20;
    if(pllConfig->init_freq != 0.0 && !value.clkLOS) {
        while (!value.pll2Lock || 
               !value.pll1Lock || 
                value.pll1LockLossLatch || 
                value.pll2LockLossLatch) 
            {
            if(!status(value)){
                __DEBUG_ERROR__("Can`t get pll status.");
                break;
            }
            if(!(--attempts)){
                __DEBUG_ERROR__("Attempts are exhausted.");
                break;
            }
            std::this_thread::sleep_for(
                std::chrono::milliseconds(100));
        }
    }

    auto format = [](double value) -> std::string {
        char string[32];
        sprintf(string, "%.1lf\0", value); 
        return string;
    };

    __DEBUG_INFO__(std::string("Lmk4828 info:") 
        + " vcxco_trim_ppm: " + format(pllConfig->vcxco_trim_ppm)
        + ", ref_clk_freq: "  + format(pllConfig->init_freq));
}

bool Lmk4828FtdiAccessor::init(double refClkFreq)
{
    if(!isInitedBitbangMode()) return false;

    bool externalRefFreqIsDefined = refClkFreq != 0.0 ? true : false;

    std::map<RegisterAddr, RegisterValue> registers;
    parseInitRegs(Lmk4828InitRegisters, registers);

    auto config = Settings::instance().getPllSystemDeviceConfig();
    setVcxcoTrimRegs(config->vcxco_trim_ppm, registers);

    if(externalRefFreqIsDefined) {
        computeInitRegsByFrequency(refClkFreq, registers);
    }

    bitbangWrite(0x000090);
    writeRegisters(registers);

    if(externalRefFreqIsDefined) {
        initPll1();
    }

    initPll2();

    //switch off sync mode
    bitbangWrite(0x0144FF); //switch off sync
    bitbangWrite(0x013902); //mux = sysref pulser
    bitbangWrite(0x014313); //sysref_clr = 0, spiPulser mode

    //power down of dealay circuits
    bitbangWrite(0x014002); //sysref_ddly_pd
    bitbangWrite(0x0106F2); //ch1 ddly_pd
    bitbangWrite(0x010EF2); //ch3 ddly_pd
    bitbangWrite(0x0116F2); //ch5 ddly_pd

    return true;
}

bool Lmk4828FtdiAccessor::sendSysref(void) {
    auto result = bitbangWrite(0x013E00);
    __DEBUG_INFO__("SysRef was sended.");
    return result;
}

bool Lmk4828FtdiAccessor::status(customsdr_pll_status &value) {
    bool result = true;

    uint8_t regPll1;
    if(read(0x818200, &regPll1)) {
        value.pll1Lock = regPll1 & 0x02 ? 1 : 0;
        value.pll1LockLossLatch = regPll1 & 0x04 ? 1 : 0;
        if(value.pll1LockLossLatch) {
            write(0x018201);
            write(0x018200);
        }
    } else result = false;

    uint8_t regPll2;
    if(read(0x818300, &regPll2)) {
        value.pll2Lock = regPll2 & 0x02 ? 1 : 0;
        value.pll2LockLossLatch = regPll2 & 0x04 ? 1 : 0;
        if(value.pll2LockLossLatch) {
            write(0x018301);
            write(0x018300);
        }
    } else result = false;

    uint8_t reg188;
    if(read(0x818800, &reg188)) {
        value.holdOver = reg188 & 0x10 ? 1 : 0;
    } else result = false;

    uint8_t reg184, reg185;
    if(read(0x818400, &reg184) 
        && read(0x818500, &reg185)) 
    {
        value.clkLOS = reg184 & 0x03 ? 1 : 0;

        uint16_t dac = ((uint16_t)(reg184 >> 6)) << 8;
        dac |= reg185;
        value.dac = dac;
    } else result = false;

    return result;
}

bool Lmk4828FtdiAccessor::getLosStatus() {
    uint8_t ret;
    if(!bitbangWriteAndRead(0x818400, &ret)) {
        __DEBUG_ERROR__("Can`t read: 0x818400");
        return -1;
    }

    return ret & 0x02;
}

bool Lmk4828FtdiAccessor::getPll1Lock() {
    uint8_t ret;
    if(!bitbangWriteAndRead(0x818200, &ret)) {
        __DEBUG_ERROR__("Can`t read: 0x818200");
        return false;
    }

    return ret & 0x02;
}

bool Lmk4828FtdiAccessor::getPll2Lock() {
    uint8_t ret;
    if(!bitbangWriteAndRead(0x818300, &ret)) {
        __DEBUG_ERROR__("Can`t read: 0x818300");
        return false;
    }

    return ret & 0x02;
}

bool Lmk4828FtdiAccessor::readRegisters(uint16_t address,
                                         uint8_t *values,
                                         uint32_t length)
{
    if(!isInitedBitbangMode()) return false;
    
    for (size_t i = 0; i < length; i++) {
        if(!readRegister(address + i, &values[i])) {
             __DEBUG_ERROR__("Can`t write register, "
                "address:" + std::to_string(address + i));
            
            continue;
        }
    }
    return true;
}

bool Lmk4828FtdiAccessor::writeRegisters(uint16_t address, 
                                         const uint8_t *values, 
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

bool Lmk4828FtdiAccessor::writeRegister(uint16_t address, const uint8_t value) {
    auto paddr = ((uint8_t*)&address) + 1;
    uint8_t packet[3] = {*paddr, *(--paddr), value};

    unsigned int written = 0;
    return bitbangWrite(packet, 3, &written);
}

bool Lmk4828FtdiAccessor::readRegister(uint16_t address, uint8_t* value) {
    auto paddr = ((uint8_t*)&address) + 1;
    uint8_t packet[3] = {*paddr, *(--paddr), 0xff};
    packet[0] |= 0x80;

    bool result = false;
    unsigned int written;
    if(result = bitbangWriteAndRead((uint8_t*)&packet, 3, &written)) {
        if(3 != written) 
            return false;

        *value = packet[2];
    }
    
    return result;
}

void Lmk4828FtdiAccessor::initPll1() {
    uint8_t ret;

    if (!(getLosStatus()))
    {
        bitbangWrite(0x015008); //holdover off, hitless_sw=0
        bitbangWrite(0x014B72); //off holdover force, MAN_DAC_ENA=0

        int cnt = 30; 
        bool lock = false;
        do {
            if(lock = getPll1Lock()) break;
            if(getLosStatus()) break;

            std::this_thread::sleep_for(
                std::chrono::milliseconds(100)); //replace polling by event
        }
        while (--cnt);

        bitbangWrite(0x01500B); //holdover on, hitless_sw=1
        if (!lock) bitbangWrite(0x014B7E); //holdover force on, MAN_DAC_ENA=1
    }
}

void Lmk4828FtdiAccessor::initPll2() {
    uint8_t ret;
    int cnt = 10; //1sec must be parametrized
    bool lock = false;
    do 
    {
        if(getPll2Lock()) break;
        std::this_thread::sleep_for(
            std::chrono::milliseconds(100));
    }
    while (--cnt);
}

bool Lmk4828FtdiAccessor::parseInitRegs(
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

bool Lmk4828FtdiAccessor::setVcxcoTrimRegs(
    double value, std::map<RegisterAddr, RegisterValue> &registers)
{
    if(value == -1.0) {
        __DEBUG_ERROR__("vcxcoTrimPpm is -1.");
        return false;
    }

    double vcxo_sens = 25.0; //VCHD-950 in ppm/V
    uint16_t trim = (value == 0.0) ? 512 : (1024.0 / 3.3 * value / vcxo_sens - 512.0);
    uint8_t prev14Bvalue = registers.find(0x14B)->second;

    auto hi = [](uint16_t a) -> unsigned char {return ((a >> 8) & 0xff);};
    auto lo = [](uint16_t a) -> unsigned char {return (a & 0xff);};

    registers[0x14B] = (prev14Bvalue & 0xfc) | (hi(trim) & 0x03);
    registers[0x14C] = lo(trim);

    return true;
}

bool Lmk4828FtdiAccessor::computeInitRegsByFrequency(
    double frequency, std::map<RegisterAddr, RegisterValue>& registers)
{
    const double VcxcoFreq = 122.88e6;
    const double Pll1MaxFreq = 1.2e6;
    const double Pll1MinFreq = 80e3;
    const double MaxDividerR = 16363L;
    const double MaxDividerN = 16363L;
    const double MaxDldCnt = 16363L;
    const double DldPrec = 5e-7;
    const double WndSize = 4e-9;

    int dividerN = 0, 
        dividerR = floor(frequency / Pll1MaxFreq);

    double fpfd;

    bool valid = false;
    while ((fpfd = frequency / dividerR) >= Pll1MinFreq 
        && dividerR <= MaxDividerR)
    {
	    double Nf = VcxcoFreq / fpfd;
	    dividerN = (int)Nf;
	    if (dividerN > 8191) 
            break;
	    if (dividerN == Nf) {
            valid = true; 
            break;
        } //integer value
        
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

	    //@0x15B <= CP | 0x10;
        registers[0x15B] = (std::min(lround(0.02 * dividerN - 0.5), 15L)) | 0x10;
	    
        double value = lround(fpfd * (WndSize / DldPrec));
        int dldCnt = std::min(value, MaxDldCnt);

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

void Lmk4828FtdiAccessor::writeRegisters(
    std::map<RegisterAddr, RegisterValue> &registers) 
{
    for(auto reg: registers) {
        writeRegister(reg.first, reg.second);
    }
}