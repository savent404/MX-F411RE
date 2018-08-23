#include "blade.h"

using namespace std;

_Blade::_Blade(const iParam* p)
    : iBlade(p)
    , t(osPriorityNormal, 0x4000, NULL, "Blade")
    , spi_dma(PA_7, NC, PA_5)
{
    spi_dma.frequency(int(8e6));
    spi_dma.format(8);
    neoPixelMap = (uint8_t*)malloc(sizeof(uint8_t)
                                   * getPixelNum()
                                   * 24
                                   + resetLength);
    for (int i = 0; i < resetLength; i++)
        neoPixelMap[i] = 0;

    t.start(callback(this, &_Blade::thread_handle));
}

_Blade::~_Blade()
{
    free(neoPixelMap);
}

bool _Blade::play(triggerID_t id, uint32_t audioDuration)
{
    return iBlade::play(id, audioDuration);
}

bool _Blade::abort(triggerID_t id)
{
    return iBlade::abort(id);
}

void _Blade::thread_handle()
{
    for (;;)
    {
        Thread::wait(BLADE_INTERVAL);
        iBlade::hanlde();
        if (isActive)
            update();
    }
}

void _Blade::bit_set(uint8_t* begin, uint8_t data)
{
    uint8_t* ptr = begin;
    for (int i = 0; i < 8; i++) {
        if (data & 0x80)
            *ptr++ = logic_1;
        else
            *ptr++ = logic_0;
        data <<= 1;
    }
}

void _Blade::bit_get(uint8_t* begin, uint8_t& data)
{
    uint8_t* ptr = begin;
    for (int i = 0; i < 8; i++) {
        data <<= 1;
        if (*ptr++ == logic_1)
            data |= 1;
    }
}

void _Blade::setColor(uint8_t* ptr, const RGB& rgb)
{
    uint8_t* _p = ptr;

    bit_set(_p, rgb.wG() * 2 / 3);
    _p += 8;
    bit_set(_p, rgb.wR());
    _p += 8;
    bit_set(_p, rgb.wB());
}

void _Blade::readColor(uint8_t* ptr, RGB& rgb)
{

    uint8_t* _p = ptr;

    bit_get(_p, rgb.G);
    _p += 8;
    bit_get(_p, rgb.R);
    _p += 8;
    bit_get(_p, rgb.B);
}

void _Blade::update()
{
    _update();
}

void _Blade::_update()
{
    uint8_t* p = neoPixelMap + resetLength;
    RGB* pColor = vector;
    for (int i = 0; i < getPixelNum(); i++, p += 24, pColor += 1)
    {
        setColor(p, *pColor);
    }
    spi_dma.write((const char*)neoPixelMap,
                  getPixelNum() * 24 + resetLength,
                  (char*)NULL,
                  0);
}
