#ifndef __H_VE_DIRECT_HEX_VALUE__
#define __H_VE_DIRECT_HEX_VALUE__

#include <stdint.h>

class VEDirectHexValue
{
public:
    VEDirectHexValue(uint8_t un8);
    VEDirectHexValue(int16_t sn16);
    VEDirectHexValue(uint16_t un16);
    VEDirectHexValue(int32_t sn32);
    VEDirectHexValue(uint32_t un32);

    uint8_t asUn8() const;
    int16_t asSn16() const;
    uint16_t asUn16() const;
    uint32_t asUn24() const;
    int32_t asSn32() const;
    uint32_t asUn32() const;

private:
    union variant
    {
        uint8_t un8;
        int16_t sn16;
        uint16_t un16;
        uint32_t un24;
        int32_t sn32;
        uint32_t un32;
    };

private:
    variant _value;
};

#endif