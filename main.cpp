#include "mbed.h"

#include "ff.h"
#include "ff_gen_drv.h"
#include "sd_diskio.h"

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

    /**
     * Init flag check
     */
    if (fInit == false) {
        debug("MX-F411RE Init OK\r\n");
    } else {
        debug("MX-F411RE Init Error\r\n");
        return -1;
    }

    while (1)
        ;

    return 0;
}
