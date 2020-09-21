#ifndef __H_VE_DIRECT_UTILS__
#define __H_VE_DIRECT_UTILS__

#include <stdlib.h>
#include <stdint.h>

class VEDirectUtils
{
public:
    static size_t maxStrcpy(char *dest,
                            const char *src,
                            size_t maxLen);

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
};

#endif