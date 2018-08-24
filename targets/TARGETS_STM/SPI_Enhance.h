#pragma once

#include "mbed.h"
#include "cmsis.h"
#include "mbed_trace.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t spi_base;
    uint32_t dma_base;
    uint32_t dma_channel;
    uint32_t dma_irq;
} SPI_DMA_Map_t;

const SPI_DMA_Map_t *find_tx_map(uint32_t spi_base);
const SPI_DMA_Map_t *find_rx_map(uint32_t spi_base);

#ifdef __cplusplus
}
#endif

class SPI_DMA : public SPI {
    protected:
        DMA_HandleTypeDef tx_dma;
        DMA_HandleTypeDef rx_dma;

        CThunk<SPI_DMA> _tx_dma_irq;
        CThunk<SPI_DMA> _rx_dma_irq;

        virtual void tx_dma_irq_handle();
        virtual void rx_dma_irq_handle();

        event_callback_t _tx_callback;
        event_callback_t _rx_callback;

        Semaphore tx_cplt_sem;
        Semaphore rx_cplt_sem;

        bool txEnableDMA;
        bool rxEnableDMA;
    public:
        SPI_DMA(PinName mosi,
                PinName miso = NC,
                PinName sclk = NC,
                PinName cs = NC);
        ~SPI_DMA();

        virtual int write(const char* tx_buffer,
                          int tx_length,
                          char* rx_buffer,
                          int rx_length);
        virtual void setTxCallback(const event_callback_t callback);
        virtual void setRxCallback(const event_callback_t callback);
};
