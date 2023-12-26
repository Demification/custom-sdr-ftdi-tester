#include <cstdlib>

#include "interface.h"
#include "ftdi_wrapper.h"

#ifdef __cplusplus
extern "C"
{
#endif

    // Inside this "extern C" block, I can implement functions in C++, which will externally
    //   appear as C functions (which means that the function IDs will be their names, unlike
    //   the regular C++ behavior, which allows defining multiple functions with the same name
    //   (overloading) and hence uses function signature hashing to enforce unique IDs),

    static FTDIRegProgrammer *ftdi_instance = NULL;
    void lazyftdi(void* handle)
    {
        int status = 0;
        if (ftdi_instance == NULL)
        {
            ftdi_instance = new FTDIRegProgrammer(handle, &status);
            if (status == 1)
            {
                ftdi_instance = NULL;
            }
        }
    }

    int ftdi_set(void *handle)
    {
        lazyftdi(handle);
        // int status =  ftdi_instance->open(name);
        return 0;
    }

    int ftdi_readReg(int addr)
    {
        int val = 0;
        if (ftdi_instance != NULL)
        {
            val = ftdi_instance->readReg(addr);
        }
        return val;
    }

    int ftdi_writeReg(int addr, int val)
    {
        int retval = 0;
        if (ftdi_instance != NULL)
        {
            retval = ftdi_instance->writeReg(addr, val);
        }
        return retval;
    }
#ifdef __cplusplus
}
#endif