#include "SPI_Enhance.h"

#define TRACE_GROUP "SPI_Enhance"

SPI_DMA::SPI_DMA(PinName mosi,
                 PinName miso,
                 PinName sclk,
                 PinName cs)
    : SPI(mosi, miso, sclk)
    , _tx_dma_irq(this)
    , _rx_dma_irq(this)
    , tx_cplt_sem(1)
    , rx_cplt_sem(1)
    , txEnableDMA(false)
    , rxEnableDMA(false)
{
    /**
     * for now only support tx dam function
     */
    if (_spi.spi.pin_mosi != NC) {
        const SPI_DMA_Map_t *ptr = find_tx_map(uint32_t(_spi.spi.handle.Instance));
        if (ptr != NULL) {
            txEnableDMA = true;
            
#ifdef DMA1_BASE
            if ((ptr->dma_base & ~0xFF) == DMA1_BASE) {
                __HAL_RCC_DMA1_CLK_ENABLE();
            }
#endif
#ifdef DMA2_BASE
            if ((ptr->dma_base & ~0xFF) == DMA2_BASE) {
                __HAL_RCC_DMA2_CLK_ENABLE();
            }
#endif
            tx_dma.Instance = (DMA_Stream_TypeDef*)ptr->dma_base;
            tx_dma.Init.Channel = ptr->dma_channel;
            tx_dma.Init.Direction = DMA_MEMORY_TO_PERIPH;
            tx_dma.Init.PeriphInc = DMA_PINC_DISABLE;
            tx_dma.Init.MemInc = DMA_MINC_ENABLE;
            tx_dma.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
            tx_dma.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
            tx_dma.Init.Mode = DMA_NORMAL;
            tx_dma.Init.Priority = DMA_PRIORITY_LOW;
            tx_dma.Init.FIFOMode = DMA_FIFOMODE_DISABLE;

            HAL_DMA_Init(&tx_dma);
            __HAL_LINKDMA(&_spi.spi.handle, hdmatx, tx_dma);
            _tx_dma_irq.callback(&SPI_DMA::tx_dma_irq_handle);
            NVIC_SetVector((IRQn_Type)ptr->dma_irq, _tx_dma_irq.entry());
            NVIC_EnableIRQ((IRQn_Type)ptr->dma_irq);
            NVIC_SetPriority((IRQn_Type)ptr->dma_irq, 4);
        }
    }
}

SPI_DMA::~SPI_DMA()
{
    if (txEnableDMA)
        HAL_DMA_DeInit(&tx_dma);
    if (rxEnableDMA)
        HAL_DMA_DeInit(&rx_dma);
}

int SPI_DMA::write(const char* tx_buffer,
                   int tx_length,
                   char* rx_buffer,
                   int rx_length)
{
    if (tx_length != 0 && rx_length != 0) {
        return SPI::write(tx_buffer,
                          tx_length,
                          rx_buffer,
                          rx_length);
    }
    else if (tx_length != 0 && txEnableDMA) {
        HAL_StatusTypeDef res;

        tx_cplt_sem.wait();
        lock();
        res = HAL_SPI_Transmit_DMA(&_spi.spi.handle,
                                   (uint8_t*)tx_buffer,
                                   tx_length);
        unlock();
        if (res != HAL_OK) {
            tr_debug("SPI Transmit_DMA error");
            return -1;
        }
    } else {
        return SPI::write(tx_buffer,
                          tx_length,
                          rx_buffer,
                          rx_length);
    }
}

void SPI_DMA::tx_dma_irq_handle()
{
    int event = 0;
    event = event;

    HAL_DMA_IRQHandler(&tx_dma);
    if (tx_dma.State == HAL_DMA_STATE_READY) {
        int error = tx_dma.ErrorCode;
        if (error != HAL_DMA_ERROR_NONE) {
            event = SPI_EVENT_ERROR |
                    SPI_EVENT_INTERNAL_TRANSFER_COMPLETE;
        } else {
            event = SPI_EVENT_COMPLETE |
                    SPI_EVENT_INTERNAL_TRANSFER_COMPLETE;
        }
        if (_tx_callback)
            _tx_callback.call(event);
    }
    tx_cplt_sem.release();
}

void SPI_DMA::rx_dma_irq_handle()
{
    int event = 0;
    event = event;

    HAL_DMA_IRQHandler(&rx_dma);
    if (rx_dma.State == HAL_DMA_STATE_READY) {
        int error = rx_dma.ErrorCode;
        if (error != HAL_DMA_ERROR_NONE) {
            event = SPI_EVENT_ERROR |
                    SPI_EVENT_INTERNAL_TRANSFER_COMPLETE;
        } else {
            event = SPI_EVENT_COMPLETE |
                    SPI_EVENT_INTERNAL_TRANSFER_COMPLETE;
        }
        if (_rx_callback)
            _rx_callback.call(event);
    }
    rx_cplt_sem.release();
}

void SPI_DMA::setTxCallback(const event_callback_t callback)
{
    _tx_callback = callback;
}

void SPI_DMA::setRxCallback(const event_callback_t callback)
{
    _rx_callback = callback;
}

static const SPI_DMA_Map_t SPI_DMA_Map_SPI_Tx[] = {
    { SPI1_BASE, DMA2_Stream2_BASE, DMA_CHANNEL_2, DMA2_Stream2_IRQn },
    { SPI2_BASE, DMA2_Stream4_BASE, DMA_CHANNEL_4, DMA2_Stream4_IRQn },
    { SPI3_BASE, DMA1_Stream5_BASE, DMA_CHANNEL_0, DMA1_Stream5_IRQn },
    { SPI4_BASE, DMA2_Stream4_BASE, DMA_CHANNEL_5, DMA2_Stream4_IRQn },
    { SPI5_BASE, DMA2_Stream6_BASE, DMA_CHANNEL_7, DMA2_Stream6_IRQn }
};

extern "C" const SPI_DMA_Map_t* find_tx_map(uint32_t spi_base)
{
    int number = sizeof(SPI_DMA_Map_SPI_Tx) / sizeof(SPI_DMA_Map_t);
    for (int i = 0; i < number; i++) {
        if (SPI_DMA_Map_SPI_Tx[i].spi_base == spi_base) {
            return &SPI_DMA_Map_SPI_Tx[i];
        }
    }
    return NULL;
}

extern "C" const SPI_DMA_Map_t* find_rx_map(uint32_t spi_base)
{
    return NULL;
}
