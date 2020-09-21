#include <string.h>
#include "VEDirectHexValue.hpp"

VEDirectHexValue::VEDirectHexValue(uint8_t un8)
{
    _value.un8 = un8;
}

VEDirectHexValue::VEDirectHexValue(int16_t sn16)
{
    _value.sn16 = sn16;
}

VEDirectHexValue::VEDirectHexValue(uint16_t un16)
{
    _value.un16 = un16;
}

VEDirectHexValue::VEDirectHexValue(int32_t sn32)
{
    _value.sn32 = sn32;
}

VEDirectHexValue::VEDirectHexValue(uint32_t un32)
{
    _value.un32 = un32;
}

uint8_t VEDirectHexValue::asUn8() const
{
    return _value.un8;
}

int16_t VEDirectHexValue::asSn16() const
{
    return _value.sn16;
}

uint16_t VEDirectHexValue::asUn16() const
{
    return _value.un16;
}

uint32_t VEDirectHexValue::asUn24() const
{
    return _value.un24;
}

int32_t VEDirectHexValue::asSn32() const
{
    return _value.sn32;
}

uint32_t VEDirectHexValue::asUn32() const
{
    return _value.un32;
}
