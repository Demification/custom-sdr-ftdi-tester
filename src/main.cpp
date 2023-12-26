#include "Debug.hpp"
#include "Lmk04828FtdiAccessor.hpp"
#include "Afe77xxFtdiAccessor.hpp"

int main(int argc, char** argv) 
{
    /*Lmk04828FtdiAccessor lmk04828Ftdi;
    if(!lmk04828Ftdi.setup())
        __DEBUG_INFO__("Can`t setup lmk.");*/
    
    Afe77xxFtdiAccessor afe77xxFtdi;
    if(afe77xxFtdi.setup()){
        AFE77xx_RegIfSet(afe77xxFtdi.handle());
        int fd = AFE77xx_RegIfGetFd();

        /*
            DO SOMETHING USEFUL HERE WITH AFE77XX FUNCS
        */

    }
}