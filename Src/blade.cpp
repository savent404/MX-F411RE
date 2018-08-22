#include "blade.h"

using namespace std;

_Blade::_Blade(const iParam* p)
    : iBlade(p)
    , t(osPriorityNormal, 0x2000, NULL, "Blade")
{
    neoPixelMap = (uint8_t*)malloc(sizeof(uint8_t)
                                   * getPixelNum()
                                   * 24);
    t.start(callback(this, &_Blade::thread_handle));
}

_Blade::~_Blade()
{

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
    Thread::wait(BLADE_INTERVAL);
    if (isActive)
        iBlade::hanlde();
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

    bit_set(_p, rgb.wG());
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
    uint8_t* p = neoPixelMap;
    RGB* pColor = vector;
    for (int i = 0; i < getPixelNum(); i++, p += 24, vector += 1)
    {
        setColor(p, *pColor);
    }
}
