#ifndef __H_VE_DIRECT_HEX_MPPT__
#define __H_VE_DIRECT_HEX_MPPT__

#include <stddef.h>
#include <stdint.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include "CircularBuffer.hpp"
#include "VEDirectTransport.hpp"
#include "VEDirectHexFieldResp.hpp"
#include "VEDirectHexFieldStringResp.hpp"

class VEDirectBMV
{
public:
    static bool loadBmvHexDefs(File dataFile);
    static const char *getLoadBmvHexDefsError();

public:
    VEDirectBMV(VEDirectTransport *transport);

    void loop();

    void ping();
    void appVersion();
    void productId();
    void restart();

    VEDirectHexFieldStringResp getStringById(uint16_t id);
    VEDirectHexFieldStringResp getStringByName(const char *name);

    VEDirectHexFieldResp getById(uint16_t id);
    VEDirectHexFieldResp getByName(const char *name);

    VEDirectHexFieldResp setById(uint16_t id, VEDirectHexValue value);
    VEDirectHexFieldResp setByName(const char *name, VEDirectHexValue value);

private:
    VEDirectHexFieldStringResp _getString(JsonObject fieldInfo);
    VEDirectHexFieldResp _get(JsonObject fieldInfo);
    VEDirectHexFieldResp _set(JsonObject fieldInfo, VEDirectHexValue value);

    void sendGetCommand(uint16_t id, uint8_t *buf, int bufLen);

    int readResponse(uint8_t *buf, size_t bufLen);

public:
    enum Command
    {
        c_ping = 0x1,
        c_appVer = 0x3,
        c_prodId = 0x4,
        c_restart = 0x6,
        c_get = 0x7,
        c_set = 0x8,
        c_async = 0xA
    };

    enum Response
    {
        r_done = 0x1,
        r_unknown = 0x3,
        r_ping = 0x5,
        r_get = 0x7,
        r_set = 0x8
    };

    enum Flag
    {
        f_unknownId = 0x01,
        f_notSupported = 0x02,
        f_parameterError = 0x04
    };

private:
    size_t writeBufAvailableToDrain();
    size_t writeBufAvailableToFill();

private:
    static const size_t g_bufSize = 1024;
    static const size_t g_maxErrorLen = 2048;

    static const uint8_t g_checkMagic = 0x55;

    static const int g_responseTimeout_ms = 5000;

    static const int g_errTimeout = -1;
    static const int g_errMsgTooLong = -2;
    static const int g_badChecksum = -3;

private:
    VEDirectTransport *_transport;

    uint8_t _readBuf[g_bufSize];
    size_t _readPos;

    uint8_t _writeBufStorage[g_bufSize];
    CircularBuffer _writeBuf;

private:
    static DynamicJsonDocument g_bmvHexDefs;
    static char g_loadBmvHexDefsError[g_maxErrorLen];
};

#endif
