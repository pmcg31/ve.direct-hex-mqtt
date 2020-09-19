#ifndef __H_VE_DIRECT_HEX_TRANSPORT_SERIAL__
#define __H_VE_DIRECT_HEX_TRANSPORT_SERIAL__

#include <HardwareSerial.h>
#include "VEDirectTransport.hpp"

class VEDirectTransportSerial : public VEDirectTransport
{
public:
    VEDirectTransportSerial(HardwareSerial *port);

    virtual size_t availableRead();
    virtual size_t availableWrite();

    virtual int read();
    virtual size_t write(const uint8_t *buf, size_t len);

private:
    HardwareSerial *_port;
};

#endif