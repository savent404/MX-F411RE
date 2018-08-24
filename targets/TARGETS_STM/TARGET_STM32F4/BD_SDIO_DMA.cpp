#include "BD_SDIO_DMA.h"

static Semaphore tx_sem(0);
static Semaphore rx_sem(0);

SDIO_DMA_bd_t::SDIO_DMA_bd_t()
{

}

int SDIO_DMA_bd_t::init()
{
    SDIO_IO_Init();
    int res = -1;
    if (osKernelGetState() == osKernelRunning)
    {
#if !defined(DISABLE_SD_INIT)
        if (BSP_SD_Init() == MSD_OK)
        {
            res = BSP_SD_GetCardState() != MSD_OK;
        }
#else
        res = BSP_SD_GetCardState() != MSD_OK;
#endif
    }
    return res;
}

int SDIO_DMA_bd_t::deinit()
{
    return 0;
}

int SDIO_DMA_bd_t::sync()
{
    return 0;
}

int SDIO_DMA_bd_t::read(void* buffer, bd_addr_t addr, bd_size_t size)
{
    int res = -1;
    uint32_t timer;
#if (ENABLE_SD_DMA_CACHE_MAINTENANCE == 1)
    uint32_t alignedAddr;
#endif

    if (BSP_SD_ReadBlocks_DMA((uint32_t*)buffer,
            uint32_t(addr / get_read_size()),
            uint32_t(size / get_read_size()))
        == MSD_OK) {
        /* wait for a message from the queue or a timeout */
        res = rx_sem.wait(SD_TIMEOUT);
        if (res > 0) {
            timer = osKernelGetTickCount() + SD_TIMEOUT;
            /* block until SDIO IP is ready or a timeout occur */
            while (timer > osKernelGetTickCount()) {
                res = 0;
#if (ENABLE_SD_DMA_CACHE_MAINTENANCE == 1)
                /* the SCB_InvalidateDCache_by_Addr() requires a 32-Byte aligned address,
                 * adjust the address and the D-Cache size to invalidate accordingly.
                 */
                alignedAddr = (uint32_t)buffer & ~0x1F;
                SCB_InvalidateDCache_by_Addr((uint32_t*)alignedAddr, uint32_t(count * get_read_size()) + ((uint32_t)buffer - alignedAddr));
#endif
                break;
            }
        }
    }
    return res;
}

int SDIO_DMA_bd_t::program(const void *buff, bd_addr_t addr, bd_size_t size)
{
    return 0;
}

bd_size_t SDIO_DMA_bd_t::get_read_size() const
{
    return 512;
}

bd_size_t SDIO_DMA_bd_t::get_program_size() const
{
    return 512;
}

bd_size_t SDIO_DMA_bd_t::size() const
{
    return 1024*1024*1024;
}

extern "C" void BSP_SD_WriteCpltCallback(void)
{
    tx_sem.release();
}

extern "C" void BSP_SD_ReadCpltCallback(void)
{
    rx_sem.release();
}
