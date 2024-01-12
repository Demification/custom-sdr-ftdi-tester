#include "Debug.hpp"
#include "afe77xx.h"
#include "FtdiDeviceInfoList.hpp"
#include "Lmk04828FtdiAccessor.hpp"
#include "Afe77xxFtdiAccessor.hpp"
#include "Settings.hpp"

int main(int argc, char** argv) 
{
    Settings::instance().setVcxcoTrim(640);

    auto infoList = FtdiDeviceInfoList::create();
    infoList->printAllDeviceInfo();

    Lmk04828FtdiAccessor lmk(infoList);
    if(!lmk.init(10000000)){
        __DEBUG_ERROR__("Lmk not inited.");
        return 1;
    }

    lmk.sendSysref();
    
    Afe77xxFtdiAccessor afe(infoList);
    if(!afe.init()){
        __DEBUG_ERROR__("Afe not inited.");
        return 1;                                                                                                                                                                          
    }

    uint8_t value;
    afe.readRegister(0x01, value);

    /*AFE77xx_RegIfSet(afe.handle());
    int fd = AFE77xx_RegIfGetFd();

    U32 data;
    AFE77xx_RegWrite(fd, 0x000130, 0x30);
    AFE77xx_RegRead(fd, 0x800100, &data);*/


    return 0;
}