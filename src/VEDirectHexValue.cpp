#include <string.h>
#include "VEDirectHexValue.hpp"

const char VEDirectHexValue::hexDigits[] = {'0', '1', '2', '3',
                                            '4', '5', '6', '7',
                                            '8', '9', 'A', 'B',
                                            'C', 'D', 'E', 'F'};

static uint16_t tester16 = 0x0102;
static uint8_t *testerP = (uint8_t *)(&tester16);
const bool VEDirectHexValue::isLittleEndian = (testerP[0] == 2);

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

uint8_t VEDirectHexValue::vicUn8ToInt(const char *s)
{
    uint8_t value = 0;

    for (int j = 0; j < 2; j++)
    {
        bool found = false;
        for (int i = 0; (i < 16) && (!found); i++)
        {
            if (s[j] == hexDigits[i])
            {
                if (j == 0)
                {
                    value += 16 * i;
                }
                else
                {
                    value += i;
                }
                found = true;
            }
        }
        if (!found)
        {
            return 0;
        }
    }

    return value;
}

int16_t VEDirectHexValue::vicSn16ToInt(const char *s)
{
    int16_t value = 0;

    uint8_t *valueBytes = (uint8_t *)(&value);
    for (int i = 0; i < sizeof(value); i++)
    {
        int byteIdx = isLittleEndian ? i : (((int)sizeof(value) - 1) - i);
        valueBytes[byteIdx] = vicUn8ToInt(s + (i * 2));
    }

    return value;
}

uint16_t VEDirectHexValue::vicUn16ToInt(const char *s)
{
    uint16_t value = 0;

    uint8_t *valueBytes = (uint8_t *)(&value);
    for (int i = 0; i < sizeof(value); i++)
    {
        int byteIdx = isLittleEndian ? i : (((int)sizeof(value) - 1) - i);
        valueBytes[byteIdx] = vicUn8ToInt(s + (i * 2));
    }

    return value;
}

uint32_t VEDirectHexValue::vicUn24ToInt(const char *s)
{
    uint32_t value = 0;

    uint8_t *valueBytes = (uint8_t *)(&value);
    for (int i = 0; i < 3; i++)
    {
        int byteIdx = isLittleEndian ? i : (((int)sizeof(value) - 1) - i);
        valueBytes[byteIdx] = vicUn8ToInt(s + (i * 2));
    }

    return value;
}

int32_t VEDirectHexValue::vicSn32ToInt(const char *s)
{
    int32_t value;

    uint8_t *valueBytes = (uint8_t *)(&value);
    for (int i = 0; i < sizeof(value); i++)
    {
        int byteIdx = isLittleEndian ? i : (((int)sizeof(value) - 1) - i);
        valueBytes[byteIdx] = vicUn8ToInt(s + (i * 2));
    }

    return value;
}

uint32_t VEDirectHexValue::vicUn32ToInt(const char *s)
{
    uint32_t value;

    uint8_t *valueBytes = (uint8_t *)(&value);
    for (int i = 0; i < sizeof(value); i++)
    {
        int byteIdx = isLittleEndian ? i : (((int)sizeof(value) - 1) - i);
        valueBytes[byteIdx] = vicUn8ToInt(s + (i * 2));
    }

    return value;
}

size_t VEDirectHexValue::intToVicUn8(uint8_t v, char *buf)
{
    buf[0] = hexDigits[v / 16];
    buf[1] = hexDigits[v % 16];

    return 2;
}

size_t VEDirectHexValue::intToVicSn16(int16_t v, char *buf)
{
    uint8_t *vBytes = (uint8_t *)(&v);

    for (int i = 0; i < sizeof(v); i++)
    {
        int byteIdx = isLittleEndian ? i : (((int)sizeof(v) - 1) - i);
        intToVicUn8(vBytes[byteIdx], buf + (i * 2));
    }

    return 4;
}

size_t VEDirectHexValue::intToVicUn16(uint16_t v, char *buf)
{
    uint8_t *vBytes = (uint8_t *)(&v);

    for (int i = 0; i < sizeof(v); i++)
    {
        int byteIdx = isLittleEndian ? i : (((int)sizeof(v) - 1) - i);
        intToVicUn8(vBytes[byteIdx], buf + (i * 2));
    }

    return 4;
}

size_t VEDirectHexValue::intToVicUn24(uint32_t v, char *buf)
{
    uint8_t *vBytes = (uint8_t *)(&v);

    for (int i = 0; i < 3; i++)
    {
        int byteIdx = isLittleEndian ? i : (((int)sizeof(v) - 1) - i);
        intToVicUn8(vBytes[byteIdx], buf + (i * 2));
    }

    return 6;
}

size_t VEDirectHexValue::intToVicSn32(int32_t v, char *buf)
{
    uint8_t *vBytes = (uint8_t *)(&v);

    for (int i = 0; i < sizeof(v); i++)
    {
        int byteIdx = isLittleEndian ? i : (((int)sizeof(v) - 1) - i);
        intToVicUn8(vBytes[byteIdx], buf + (i * 2));
    }

    return 8;
}

size_t VEDirectHexValue::intToVicUn32(uint32_t v, char *buf)
{
    uint8_t *vBytes = (uint8_t *)(&v);

    for (int i = 0; i < sizeof(v); i++)
    {
        int byteIdx = isLittleEndian ? i : (((int)sizeof(v) - 1) - i);
        intToVicUn8(vBytes[byteIdx], buf + (i * 2));
    }

    return 8;
}
