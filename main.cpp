#include "mbed.h"

#include "ff.h"
#include "ff_gen_drv.h"
#include "sd_diskio.h"

#include "param.h"
#include "blade.h"

extern "C" void SDIO_IO_Init();
char sd_path[4];

FATFS fatfs;

int main(void)
{
    bool fInit = true;
    /**
     * Fatfs Initialization
     */
    FATFS_LinkDriver(&SD_Driver, sd_path);
    SDIO_IO_Init();
    FRESULT fatfs_res = f_mount(&fatfs, sd_path, 1);
    if (fatfs_res != FR_OK) {
        fInit = false;
        debug("Fatfs mount:%d\r\n", int(fatfs_res));
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
