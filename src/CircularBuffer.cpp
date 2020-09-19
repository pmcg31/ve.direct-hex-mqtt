#include <algorithm>
#include <string.h>
#include "CircularBuffer.hpp"

CircularBuffer::CircularBuffer(uint8_t *buf, size_t len)
    : _buf(buf), _bufSize(len), _isEmpty(true), _fillPos(0), _drainPos(0) {}

bool CircularBuffer::isEmpty() const
{
    return _isEmpty;
}

size_t CircularBuffer::availableToFill() const
{
    if (_isEmpty)
    {
        return _bufSize;
    }
    else
    {
        if (_fillPos == _drainPos)
        {
            return 0;
        }
        else
        {
            if (_fillPos < _drainPos)
            {
                return (_drainPos - _fillPos);
            }
            else
            {
                return ((_bufSize - _fillPos) + _drainPos);
            }
        }
    }
}

size_t CircularBuffer::availableToDrain() const
{
    if (_isEmpty)
    {
        return 0;
    }
    else
    {
        if (_fillPos == _drainPos)
        {
            return _bufSize;
        }
        else
        {
            if (_drainPos < _fillPos)
            {
                return (_fillPos - _drainPos);
            }
            else
            {
                return ((_bufSize - _drainPos) + _fillPos);
            }
        }
    }
}

size_t CircularBuffer::fill(uint8_t *buf,
                            size_t len)
{
    size_t bytesToCopy = std::min(len, availableToFill());

    if (bytesToCopy > 0)
    {
        _isEmpty = false;

        if ((_bufSize - _fillPos) <= bytesToCopy)
        {
            memcpy(&(_buf[_fillPos]), buf, bytesToCopy);
            _fillPos += bytesToCopy;
            if (_fillPos == _bufSize)
            {
                _fillPos = 0;
            }
        }
        else
        {
            size_t firstCopyLen = _bufSize - _fillPos;
            size_t secondCopyLen = bytesToCopy - firstCopyLen;

            memcpy(&(_buf[_fillPos]), buf, firstCopyLen);
            memcpy(&(_buf[0]), buf + firstCopyLen, secondCopyLen);
            _fillPos = secondCopyLen;
        }
    }

    return bytesToCopy;
}

size_t CircularBuffer::drain(uint8_t *buf,
                             size_t len)
{
    size_t bytesToCopy = std::min(len, availableToDrain());

    if (bytesToCopy > 0)
    {
        if ((_bufSize - _drainPos) <= bytesToCopy)
        {
            memcpy(buf, &(_buf[_drainPos]), bytesToCopy);
            _drainPos += bytesToCopy;
            if (_drainPos == _bufSize)
            {
                _drainPos = 0;
            }
        }
        else
        {
            size_t firstCopyLen = _bufSize - _drainPos;
            size_t secondCopyLen = bytesToCopy - firstCopyLen;

            memcpy(buf, &(_buf[_drainPos]), firstCopyLen);
            memcpy(buf + firstCopyLen, &(_buf[0]), secondCopyLen);
            _drainPos = secondCopyLen;
        }

        if (_drainPos == _fillPos)
        {
            _isEmpty = true;
        }
    }

    return bytesToCopy;
}

size_t CircularBuffer::drainToTransport(VEDirectTransport *transport,
                                        size_t len)
{
    size_t bytesToCopy = std::min(len, availableToDrain());
    size_t bytesWritten = 0;

    if (bytesToCopy > 0)
    {
        if ((_bufSize - _drainPos) <= bytesToCopy)
        {
            bytesWritten = transport->write(&(_buf[_drainPos]), bytesToCopy);
            _drainPos += bytesWritten;
            if (_drainPos == _bufSize)
            {
                _drainPos = 0;
            }
        }
        else
        {
            size_t firstCopyLen = _bufSize - _drainPos;
            size_t secondCopyLen = bytesToCopy - firstCopyLen;

            bytesWritten = transport->write(&(_buf[_drainPos]), firstCopyLen);
            if (bytesWritten == firstCopyLen)
            {
                size_t tmp = transport->write(&(_buf[0]), secondCopyLen);
                bytesWritten += tmp;
                _drainPos = tmp;
            }
            else
            {
                _drainPos += bytesWritten;
            }
        }

        if (_drainPos == _fillPos)
        {
            _isEmpty = true;
        }
    }

    return bytesWritten;
}
