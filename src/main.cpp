#include "Debug.hpp"
#include "afe77xx.h"
#include "FtdiDeviceInfoList.hpp"
#include "Lmk04828FtdiAccessor.hpp"
#include "Afe77xxFtdiAccessor.hpp"

int main(int argc, char** argv) 
{
    auto infoList = FtdiDeviceInfoList::create();
    infoList->printAllDeviceInfo();
    
    Afe77xxFtdiAccessor afe77xxFtdi(infoList);
    if(afe77xxFtdi.init()){
        AFE77xx_RegIfSet(afe77xxFtdi.handle());
        int fd = AFE77xx_RegIfGetFd();
 

        /*DO SOMETHING USEFUL HERE WITH AFE77XX FUNCS*/
        

    }
}