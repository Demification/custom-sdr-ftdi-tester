#include "Debug.hpp"
#include "Lmk04828FtdiAccessor.hpp"
#include "Afe77xxFtdiAccessor.hpp"

int main(int argc, char** argv) 
{
    /*Lmk04828FtdiAccessor lmk04828Ftdi;
    if(!lmk04828Ftdi.bringup())
        __DEBUG_INFO__("Can`t bringup lmk.");*/
    
    Afe77xxFtdiAccessor afe77xxFtdi;
}