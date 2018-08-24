#pragma once

#include "iBlade.h"
#include "SPI_Enhance.h"

class _Blade : public iBlade
{
    private:
        /**
         * @name NeoPixel driver var
         * @{ */
        const static int resetLength = 256;
        const static uint8_t logic_1 = 0x7E;
        const static uint8_t logic_0 = 0x70;
        SPI_DMA spi_dma;
        Thread t;
        void thread_handle();
        uint8_t* neoPixelMap;
        static void bit_set(uint8_t* begin, uint8_t data);
        static void bit_get(uint8_t* begin, uint8_t& data);
        static void setColor(uint8_t* ptr, const RGB& rgb);
        static void readColor(uint8_t* ptr, RGB& rgb);
        /** @} */
    public:
        _Blade(const iParam* p);
        virtual ~_Blade();
        virtual bool play(triggerID_t id, uint32_t audioDuration = 0);
        virtual bool abort(triggerID_t id = Unknow);
        virtual void update();
        void _update();
};
