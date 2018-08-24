#include "mbed.h"
#include "filesystem/fat/FATFileSystem.h"
#include "BD_SDIO_DMA.h"

#include "param.h"
#include "blade.h"

SDIO_DMA_bd_t sdio_bd;
FATFileSystem fatfsInstance("0:");
int main(void)
{
    bool fInit = true;
    /**
     * Fatfs Init
     */
    if (fatfsInstance.mount(&sdio_bd) != 0) {
        fInit = false;
        debug("Fatfs mount\r\n");
    }
    _Param *param = new _Param(0);
    _Blade *blade = new _Blade(param);
    if (param == NULL)
        fInit = false;
    if (blade == NULL)
        fInit = false;

    /**
     * Init flag check
     */
    if (fInit == true) {
        debug("MX-F411RE Init OK\r\n");
    } else {
        debug("MX-F411RE Init Error\r\n");
        return -1;
    }

    param->setParameter("NP_Cset", 5);
    param->setParameter("NP_Amode", 1);
    param->setParameter("NP_Density", 99.1f);
    blade->play(Out);

    while (1)
    {
        Thread::wait(2);
        // blade->play(Swing);
    }

    return 0;
}