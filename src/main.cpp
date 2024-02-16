#include <thread>
#include <chrono>
#include <fstream>
#include <vector>

#include "Debug.hpp"
#include "afe77xx.h"
#include "Afe7769FtdiAccessor.hpp"

void save_bin_to_file(Afe7769FtdiAccessor& afe)
{
    const int size = 128;
    uint8_t values[size];
    afe.burstRead(0x00, values, size);

    static int increase = 0;

    std::ofstream output_file;
    output_file.open ("output_file_" + std::to_string(increase) 
                                     + ".bin", std::ios::binary | std::ios::trunc);

    for (int i = 0; i < size; i++)
        output_file << values[i];

    output_file.close();

    if(increase){
        uint8_t prev_values[size];
        std::ifstream input_file;
        input_file.open ("output_file_" + std::to_string(increase - 1) 
                                     + ".bin", std::ios::binary | std::ios::trunc);

        for (int i = 0; i < size; i++){
            input_file >> prev_values[i];

            if(prev_values[i] != values[i])
                std::cout << std::hex << "addr: 0x" << i 
                                      << " value: 0x" << (int)(values[i]) 
                                      << " prev: 0x"  << (int)(prev_values[i]) 
                                      << std::dec << std::endl;
        }

        input_file.close();
    }

    ++increase;
}

void save_txt_to_file(Afe7769FtdiAccessor& afe, uint16_t first, uint16_t size)
{
    //const uint16_t size = 32;
    std::vector<uint8_t> values(size);
    //uint16_t first = 0x20;
    //afe.burstRead(first, values, size);

    //std::ofstream output_file;
    //output_file.open ("output_file.txt", std::ios::trunc);

    for (uint16_t i = 0; i < size; i++)
    {
        uint8_t val = values[i];
        afe.read(i + first, &val);
        if(val != 0)
                std::cout << std::hex << "addr: 0x" << i + first
                                        << " value: 0x" << (int)(val)
                                        << std::dec << std::endl;
    }
    //output_file.close();
}


int bringup(Afe7769FtdiAccessor& afe)
{
    //wakeUp
    afe.write(0x0015,0x80); //open GPIO page
    afe.write(0x0600,0x03); //MISC_SPI_GLOBAL_PDN_SIG
    afe.write(0x0600,0x01); //MISC_SPI_GLOBAL_PDN_SIG

    //reloadEfuseChain
    afe.write(0x07f0,0x00); //SPI_OVR_SYS_AUTOLOAD_CHAIN bits 2-5
    afe.write(0x0804,0x00); //SPI_OVR_SYS_AUTOLOAD_CHAIN bits 2-5
    afe.write(0x07f0,0x3c); //SPI_OVR_SYS_AUTOLOAD_CHAIN bits 2-5
    afe.write(0x0804,0x3c); //SPI_OVR_SYS_AUTOLOAD_CHAIN bits 2-5
    afe.write(0x07f0,0x3c); //SPI_OVR_SYS_AUTOLOAD_CHAIN bits
    afe.write(0x07f0,0x3d); //SPI_OVR_SYS_AUTOLOAD_CHAIN bits
    afe.write(0x07f0,0x3f); //SPI_OVR_SYS_AUTOLOAD_CHAIN #Resets the autoload state machine and counters
    afe.write(0x07f0,0x3c); //SPI_OVR_SYS_AUTOLOAD_CHAIN bits 2-5
    afe.write(0x0804,0x3c); //SPI_OVR_SYS_AUTOLOAD_CHAIN bits
    afe.write(0x0804,0x3d); //SPI_OVR_SYS_AUTOLOAD_CHAIN bits
    afe.write(0x0804,0x3f); //SPI_OVR_SYS_AUTOLOAD_CHAIN bits #Resets the autoload state machine and counters
    afe.write(0x0804,0x3c); //SPI_OVR_SYS_AUTOLOAD_CHAIN bits 2-5
    afe.write(0x07f0,0x00); //SPI_OVR_SYS_AUTOLOAD_CHAIN
    afe.write(0x0804,0x00); //SPI_OVR_SYS_AUTOLOAD_CHAIN

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    //checkEfuse
    uint16_t addr[2] = {0x0138, 0x013C};
    for (int k = 0; k<2; ++k)
    {
        uint8_t reg;
        afe.read(addr[k], &reg); //OBS_FUNC_SPI_CHAIN_AUTOLOAD_ERROR(7-4), OBS_FUNC_SPI_CHAIN_AUTOLOAD_DONE(3-0)
        if (reg == 0x0F)
            std::cout << "Fuse farm load autoload done successful" << std::endl;//success
        else if((reg&0x0F) == 0x0F)
        {
            std::cout <<"Auto load error " << (reg>>4) << ". Please retrigger fuse.. unburnt part can give false errors ignore it" << std::endl;//error
            return 1;
        }
        else
        {
            std::cout << "Fuse farm load autoload done didnt go high"  << std::endl;//error
            return 2;
        }
    }

    //enableEfuseClock
    afe.write(0x0131,0x01); //SPI_OVR_SYSDIV_CLK_GATE
    afe.write(0x0015,0x00); //close GPIO page
    return 0;
}





int reset(Afe7769FtdiAccessor& afe)
{
    afe.write(0x0013,0x10); //open CUSTOMER_MACRO page
    afe.write(0x0140,0x01); //.MACRO.FW_REGS.MCU_CONTROL.ss_reset_reg
    afe.write(0x0140,0x00); //.MACRO.FW_REGS.MCU_CONTROL.ss_reset_reg
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    afe.write(0x0013,0x00); //close CUSTOMER_MACRO page
    return 0;
}


int enableTempSensor(Afe7769FtdiAccessor& afe)
{
    afe.write(0x0015,0x80); //open GPIO page
    afe.write(0x03a3,0x08); //TEMP_SENSE1_IPTAT_ON_TP(7), TEMP_SENSE1_IREF_ON_TP(6), TEMP_SENSE1_BG_ON_TP(4), TEMP_SENSE1_SEL_INPUT(3), TEMP_SENSE1_I1SWITCH(2), TEMP_SENSE1_I2SWITCH(1)
    afe.write(0x03a0,0x7e); //TEMP_SENSE1_ANALOG_POWERUP(6), TEMP_SENSE1_DIV2(5-4), TEMP_SENSE1_DIV1(3-2), TEMP_SENSE1_EN(1), TEMP_SENSE1_SOFT_RESET(0)
    afe.write(0x0722,0x02); //TEMP_SENSE_25M_CLOCK_UNGATE(1), SMART_REFLEX_25M_CLOCK_UNGATE(0)
    afe.write(0x0015,0x00); //close GPIO page
    return 0;
}

int main(int argc, char** argv) 
{
    Afe7769FtdiAccessor afe;
    if(!afe.init()){
        __DEBUG_ERROR__("Afe not inited.");                                                                                                                                                                        
    }

    if (bringup(afe) != 0)
        return -1;
    reset(afe);

    enableTempSensor(afe);

    uint32_t req;
    uint32_t ans;
    afe.waitUntilMacroReady(1000);
    afe.executeMacro(0x01, 0x01);
    afe.waitUntilMacroDone(1000);
    afe.readMacroResultRegister(&ans, 1);
    std::cout << "Chip ID " <<std::dec<< (int)((uint16_t)ans);

    afe.waitUntilMacroReady(1000);
    afe.executeMacro(0x02, 0x01);
    afe.waitUntilMacroDone(1000);
    afe.readMacroResultRegister(&ans, 1);
    std::cout << ", version number " <<std::dec<< (int)((uint8_t)ans);

    afe.waitUntilMacroReady(1000);
    afe.executeMacro(0x03, 0x01);
    afe.waitUntilMacroDone(1000);
    afe.readMacroResultRegister(&ans, 1);
    std::cout << ", analog temp " <<std::dec<< (int)((char)ans);


    afe.waitUntilMacroReady(1000);
    afe.executeMacro(0x04, 0x01);
    afe.waitUntilMacroDone(1000);
    afe.readMacroResultRegister(&ans, 1);
    std::cout << ", dig temp " <<std::dec<< (int)((char)ans)<< std::endl;



    uint8_t reg;




    afe.write(0x0914,0x00); //MASK_PDN_HIGH_INTR
    afe.write(0x0918,0x00); //MASK_PDN_FALL_INTR
    afe.read(0x056d, &reg);
    reg = reg&0xF3 + (0x01<<3) + (0x00 << 2); //OVR_SEL_INTPI_GLOBAL_PDN=1, OVR_INTPI_GLOBAL_PDN=0
    afe.write(0x056d,reg);


    afe.write(0x0015,0x00);

    std::this_thread::sleep_for(std::chrono::milliseconds(3));


    afe.write(0x0014,0x04);
    afe.write(0x0126,0x00);
    afe.write(0x0127,0x00);
    afe.write(0x0128,0x00);
    afe.write(0x0129,0x00);
    afe.write(0x012A,0x00);
    afe.write(0x012B,0x00);
    afe.write(0x012C,0x00);
    afe.write(0x012D,0x00);
    afe.write(0x012E,0x00);
    afe.write(0x012F,0x00);
    afe.write(0x0014,0x00);


    afe.write(0x0015,0x80);
    afe.write(0x0118,0x00);
    afe.write(0x0119,0x00);
    afe.write(0x011A,0x00);
    afe.write(0x011B,0x01);  //Alarm0 -> MACRO_DONE

    afe.write(0x011B,0x00);
    afe.write(0x011C,0x01); //Alarm1 -> SPI_ALARM
    afe.write(0x011D,0x00);
    afe.write(0x011F,0x00);


    afe.write(0x0124,0x01); //SEL_REFCLK_FOR_TOPCM4, clock is refclk/2
    afe.write(0x08e0,0x10); //SPI_DIV_SYS_CLK_SEL, 15.36MHz
    afe.write(0x08e1,0x00); //
    afe.write(0x0015,0x00);

        //clearAllAlarms(0);
        //configurePll(0, 0, 1800, 61.44*8);


    afe.write(0x13, 0x10);
    afe.write(0x00a0, 0x02);
    afe.write(0x00a1, 0x00);
    afe.write(0x00a2, 0x00);
    afe.write(0x00a3, 0x00);
    afe.write(0x193, 0x01);
    afe.write(0x13, 0x00);

    afe.write(0x15, 0x08);
    // afe.write(0x0020, 0x0f);

    std::this_thread::sleep_for(std::chrono::milliseconds(2));

    std::cout << "\nSUBCHIP: " << std::endl;
    //save_txt_to_file(afe, 0x20, 96);

    afe.write(0x15, 0x00);

    uint8_t value_1e, value_1f;
    afe.read(0x1e, &value_1e);
    afe.read(0x1f, &value_1f);

    std::cout << "alarm " << std::hex << ((int)value_1e | ((int)value_1f)<<8) << std::endl;


    afe.write(0x13, 0x10);
    afe.read(0xf0, &value_1e);
    std::cout << "\nMACRO: " << std::endl;
    save_txt_to_file(afe, 0xf8, 32);
    afe.write(0x13, 0x00);
    std::cout << "status " << std::hex << (int)value_1e << "; result " << (int)value_1f << std::endl;


    //configurePll(0, 0, 1800, 61.44*4);

//    afe.write(0x0015,0x80);
//    afe.write(0x01d4,0x01);
//    afe.write(0x0374,0x00);
//    afe.write(0x0015,0x00);

//        afe.write(0x0015,0x80);
//        afe.read(0x0722,&value_1e);
//        value_1e |= 0x01;
//        afe.write(0x0722,value_1e);
//        afe.write(0x03a0,0xfc);
//        afe.write(0x03a3,0x08);
//        afe.write(0x0015,0x00);

std::this_thread::sleep_for(std::chrono::milliseconds(100));

        afe.write(0x0015,0x80);
        afe.write(0x08f0,0x01);
        afe.read(0x03ac,&value_1e);
        afe.read(0x03ad,&value_1f);
        afe.write(0x08f0,0x00);
        afe.write(0x0015,0x00);

        std::cout << "\n temp: " <<std::dec<< (((int)value_1f)<<8) + value_1e << std::endl;

    afe.write(0x13, 0x10);

    std::cout << "\nM: " << std::endl;
    //std::this_thread::sleep_for(std::chrono::milliseconds(2));

    save_txt_to_file(afe, 0xA0, 64);

    afe.write(0x13, 0x00);



    afe.read(0x1e, &value_1e);
    afe.read(0x1f, &value_1f);
    std::cout << "alarm " << std::hex << ((int)value_1e | ((int)value_1f)<<8) << std::endl;


    //findpllFrequency(0, 0, 61.44*4);

//    for (uint32_t k=0; k<256; ++k)
//        std::cout << k << " " << SerdesLaneRead(0, 0, k) << std::endl;

    return 0;
}
