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

public:
    static uint8_t vicUn8ToInt(const char *s);
    static int16_t vicSn16ToInt(const char *s);
    static uint16_t vicUn16ToInt(const char *s);
    static uint32_t vicUn24ToInt(const char *s);
    static int32_t vicSn32ToInt(const char *s);
    static uint32_t vicUn32ToInt(const char *s);

    static size_t intToVicUn8(uint8_t v, char *buf);
    static size_t intToVicSn16(int16_t v, char *buf);
    static size_t intToVicUn16(uint16_t v, char *buf);
    static size_t intToVicUn24(uint32_t v, char *buf);
    static size_t intToVicSn32(int32_t v, char *buf);
    static size_t intToVicUn32(uint32_t v, char *buf);

public:
    static const char hexDigits[];
    static const bool isLittleEndian;

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