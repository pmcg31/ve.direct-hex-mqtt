#ifndef __H_VE_DIRECT_HEX_TRANSPORT__
#define __H_VE_DIRECT_HEX_TRANSPORT__

class VEDirectTransport
{
public:
    virtual size_t availableRead() = 0;
    virtual size_t availableWrite() = 0;

    virtual int read() = 0;
    virtual size_t write(const uint8_t *buf, size_t len) = 0;
};

#endif