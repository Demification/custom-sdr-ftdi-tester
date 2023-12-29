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

bool Lmk04828FtdiAccessor::init(double frequency) {
    if(!isInitedBitbangMode()) return false;

    if(!frequency) {
        __DEBUG_ERROR__("Frequency is null.");
        return false;
    }
    bool externalRefFreqIsDefined = frequency != -1.0 ? true : false;

    std::map<RegisterAddr, RegisterValue> registers;
    parseInitRegs(Lmk04828InitRegisters, registers);

    if(externalRefFreqIsDefined) {
        computeInitRegsByFrequency(frequency, registers);
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

bool Lmk04828FtdiAccessor::writeRegister(uint16_t address, uint8_t value) {
    if(!isInitedBitbangMode()) return false;

    auto paddr = ((uint8_t*)&address) + 1;
    uint8_t packet[3] = {*paddr, *(--paddr), value};

    unsigned int written = 0;
    return bitbangWrite(packet, 3, &written);
}

bool Lmk04828FtdiAccessor::readRegister(uint16_t address, uint8_t &value) {
    if(!isInitedBitbangMode()) return false;

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
    if(!bitbangWriteAndRead(0x818400, &ret))
        __DEBUG_ERROR__("Can`t read: 0x818400");

    if (!(ret & 0x02) /* && select_external_sync*/) //clk1 is present (zero LOS condition)
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
            if(!bitbangWriteAndRead(0x818200, &ret))
                __DEBUG_ERROR__("Can`t read: 0x818200");
            if (lock = ret & 0x02) break;

            std::this_thread::sleep_for(std::chrono::milliseconds(100)); //replace polling by event

            if(!bitbangWriteAndRead(0x818400, &ret))
                __DEBUG_ERROR__("Can`t read: 0x818400");
            if(ret & 0x02) break;
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
        if(!bitbangWriteAndRead(0x818300, &ret))
            __DEBUG_ERROR__("Can`t read: 0x818300");

        lock = ret & 0x02;
        if (lock) break;

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
        registers.emplace(0x155, hi(dividerR));
        registers.emplace(0x156, lo(dividerR));

 	    //@0x159..0x15A <= N;
        registers.emplace(0x159, hi(dividerN));
        registers.emplace(0x15A, lo(dividerN));

	    int cp = (std::min(lround(0.02 * dividerN - 0.5), 15L)) | 0x10;

	    //@0x15B <= CP | 0x10;
        registers.emplace(0x15B, (uint8_t)cp);
	    
        double value = lround(fpfd * (LMK04828_WND_SIZE / LMK04828_DLD_PREC));
        int dldCnt = std::min(value, LMK04828_MAX_DLD_CNT);

	    //@0x15C..0x15D <= dldCnt;
        registers.emplace(0x15C, hi(dldCnt));
        registers.emplace(0x15D, lo(dldCnt));
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