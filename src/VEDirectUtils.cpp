#include <string.h>
#include <algorithm>
#include <HardwareSerial.h>
#include "VEDirectUtils.hpp"

const char VEDirectUtils::hexDigits[] = {'0', '1', '2', '3',
                                         '4', '5', '6', '7',
                                         '8', '9', 'A', 'B',
                                         'C', 'D', 'E', 'F'};

static uint16_t tester16 = 0x0102;
static uint8_t *testerP = (uint8_t *)(&tester16);
const bool VEDirectUtils::isLittleEndian = (testerP[0] == 2);

size_t VEDirectUtils::maxStrcpy(char *dest, const char *src, size_t maxLen)
{
    // Compute number of bytes to copy,
    // leaving room for null terminator
    size_t bytesToCopy = std::min(maxLen - 1, strlen(src));

    // Copy data
    memcpy(dest, src, bytesToCopy);

    // Add null terminator
    dest[bytesToCopy] = '\0';

    return bytesToCopy;
}

uint8_t VEDirectUtils::vicUn8ToInt(const char *s)
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

int16_t VEDirectUtils::vicSn16ToInt(const char *s)
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

uint16_t VEDirectUtils::vicUn16ToInt(const char *s)
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

uint32_t VEDirectUtils::vicUn24ToInt(const char *s)
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

int32_t VEDirectUtils::vicSn32ToInt(const char *s)
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

uint32_t VEDirectUtils::vicUn32ToInt(const char *s)
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

size_t VEDirectUtils::intToVicUn8(uint8_t v, char *buf)
{
    buf[0] = hexDigits[v / 16];
    buf[1] = hexDigits[v % 16];

    return 2;
}

size_t VEDirectUtils::intToVicSn16(int16_t v, char *buf)
{
    uint8_t *vBytes = (uint8_t *)(&v);

    for (int i = 0; i < sizeof(v); i++)
    {
        int byteIdx = isLittleEndian ? i : (((int)sizeof(v) - 1) - i);
        intToVicUn8(vBytes[byteIdx], buf + (i * 2));
    }

    return 4;
}

size_t VEDirectUtils::intToVicUn16(uint16_t v, char *buf)
{
    uint8_t *vBytes = (uint8_t *)(&v);

    for (int i = 0; i < sizeof(v); i++)
    {
        int byteIdx = isLittleEndian ? i : (((int)sizeof(v) - 1) - i);
        intToVicUn8(vBytes[byteIdx], buf + (i * 2));
    }

    return 4;
}

size_t VEDirectUtils::intToVicUn24(uint32_t v, char *buf)
{
    uint8_t *vBytes = (uint8_t *)(&v);

    for (int i = 0; i < 3; i++)
    {
        int byteIdx = isLittleEndian ? i : (((int)sizeof(v) - 1) - i);
        intToVicUn8(vBytes[byteIdx], buf + (i * 2));
    }

    return 6;
}

size_t VEDirectUtils::intToVicSn32(int32_t v, char *buf)
{
    uint8_t *vBytes = (uint8_t *)(&v);

    for (int i = 0; i < sizeof(v); i++)
    {
        int byteIdx = isLittleEndian ? i : (((int)sizeof(v) - 1) - i);
        intToVicUn8(vBytes[byteIdx], buf + (i * 2));
    }

    return 8;
}

size_t VEDirectUtils::intToVicUn32(uint32_t v, char *buf)
{
    uint8_t *vBytes = (uint8_t *)(&v);

    for (int i = 0; i < sizeof(v); i++)
    {
        int byteIdx = isLittleEndian ? i : (((int)sizeof(v) - 1) - i);
        intToVicUn8(vBytes[byteIdx], buf + (i * 2));
    }

    return 8;
}
