#include "filesystem/bd/BlockDevice.h"
#include "bsp_driver_sd.h"
#include "mbed.h"
#include "cmsis.h"

#ifndef SD_TIMEOUT
#define SD_TIMEOUT 5000
#endif

class SDIO_DMA_bd_t : public BlockDevice
{
    public:
    SDIO_DMA_bd_t();
    virtual int init();
    virtual int deinit();
    virtual int sync();
    virtual int read(void *buffer, bd_addr_t addr, bd_size_t size);
    virtual int program(const void* buffer, bd_addr_t addr, bd_size_t size);
    virtual bd_size_t get_read_size() const;
    virtual bd_size_t get_program_size() const;
    virtual bd_size_t size() const;
};