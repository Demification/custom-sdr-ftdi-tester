#include <thread>
#include <chrono>

#include "Debug.hpp"
#include "FtdiDeviceInfoList.hpp"
#include "Lmk4828FtdiAccessor.hpp"
#include "Afe7769FtdiAccessor.hpp"
#include "Settings.hpp"

int main(int argc, char** argv) 
{
    auto infoList = FtdiDeviceInfoList::create();
    /*infoList->printAllDeviceInfo();
    Lmk4828FtdiAccessor lmk(infoList);
    if(!lmk.init(10000000)){
        __DEBUG_ERROR__("Lmk not inited.");
    }*/

    
    Afe7769FtdiAccessor afe(infoList);
    if(!afe.init()){
        __DEBUG_ERROR__("Afe not inited.");                                                                                                                                                                        
    }

    return 0;
}