#include <HardwareSerial.h>

#include "VEDirectTransportSerial.hpp"

VEDirectTransportSerial::VEDirectTransportSerial(HardwareSerial *port)
    : _port(port) {}

size_t VEDirectTransportSerial::availableRead()
{
    return _port->available();
}

size_t VEDirectTransportSerial::availableWrite()
{
    return _port->availableForWrite();
}

int VEDirectTransportSerial::read()
{
    return _port->read();
}

size_t VEDirectTransportSerial::write(const uint8_t *buf, size_t len)
{
    return _port->write(buf, len);
}
