#include <thread>
#include <chrono>

#include "Debug.hpp"
#include "afe77xx.h"
#include "FtdiDeviceInfoList.hpp"
#include "Lmk4828FtdiAccessor.hpp"
#include "Afe7769FtdiAccessor.hpp"
#include "Settings.hpp"

int main(int argc, char** argv) 
{
    auto infoList = FtdiDeviceInfoList::create();
    infoList->printAllDeviceInfo();

    Lmk4828FtdiAccessor lmk(infoList);
    if(!lmk.init(10000000)){
        __DEBUG_ERROR__("Lmk not inited.");
    }

    lmk.sysref();
    
    Afe7769FtdiAccessor afe(infoList);
    if(!afe.init()){
        __DEBUG_ERROR__("Afe not inited.");                                                                                                                                                                        
    }

    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        Lmk4828Status status;
        if(lmk.status(status)) {
            std::cout << "dac: " << status.dac << "  ";
            std::cout << "clkLOS: " << status.clkLOS << "  ";
            std::cout << "dacTrackLock: " << status.dacTrackLock << "  ";
            std::cout << "holdOver: " << status.holdOver << "  ";
            std::cout << "pll1Lock: " << status.pll1Lock << "  ";
            std::cout << "pll1LockLost: " << status.pll1LockLost << "  ";
            std::cout << "pll2Lock: " << status.pll2Lock << "  ";
            std::cout << "pll2LockLost: " << status.pll2LockLost << std::endl;
        }
    }
    
    return 0;
}