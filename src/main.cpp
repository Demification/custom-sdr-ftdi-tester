#include "Lmk4828FtdiAccessor.hpp"
#include "Afe7769Accessor.hpp"
#include "NativeSpiBackend.hpp"

#include "Debug.hpp"
#include "tiAfe77_macroInterface.h"

int main(int argc, char** argv) 
{
    Lmk4828FtdiAccessor lmk(FtdiDeviceInfoList::create());
    lmk.init();

    Afe7769Accessor afe(std::make_shared<NativeSpiBackend>());
    afe.init();

    CHECK_RESULT(configTxRxFbMacro(&afe, configMode4Tx4Rx2Fb));

    return 0;
}
