#ifndef __H_CIRCULAR_BUFFER__
#define __H_CIRCULAR_BUFFER__

#include <stddef.h>
#include <stdint.h>
#include "VEDirectTransport.hpp"

class CircularBuffer
{
public:
    CircularBuffer(uint8_t *buf, size_t len);

    bool isEmpty() const;
    size_t availableToFill() const;
    size_t availableToDrain() const;

    size_t fill(uint8_t *buf, size_t len);
    size_t drain(uint8_t *buf, size_t len);

    size_t drainToTransport(VEDirectTransport *transport, size_t len);

private:
    uint8_t *_buf;
    size_t _bufSize;

    bool _isEmpty;
    size_t _fillPos;
    size_t _drainPos;
};

#endif