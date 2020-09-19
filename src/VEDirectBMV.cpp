#include <stdlib.h>
#include <algorithm>
#include "VEDirectHexValue.hpp"
#include "VEDirectBMV.hpp"

DynamicJsonDocument VEDirectBMV::g_bmvHexDefs(32768);
char VEDirectBMV::g_loadBmvHexDefsError[VEDirectBMV::g_maxErrorLen];

bool VEDirectBMV::loadBmvHexDefs(File dataFile)
{
    g_loadBmvHexDefsError[0] = 0;
    DeserializationError error = deserializeJson(g_bmvHexDefs, dataFile);
    if (error)
    {
        sprintf(g_loadBmvHexDefsError, "VEDirectBMV::loadBmvHexDefs: Error parsing data file '%s' [%s]", dataFile.name(), error.c_str());
        return false;
    }

    return true;
}

const char *VEDirectBMV::getLoadBmvHexDefsError()
{
    return g_loadBmvHexDefsError;
}

VEDirectBMV::VEDirectBMV(VEDirectTransport *transport)
    : _transport(transport),
      _readPos(0),
      _writeBuf(_writeBufStorage, g_bufSize)
{
}

void VEDirectBMV::loop()
{
    // Read data if available
    while (_transport->availableRead() > 0)
    {
        _readBuf[_readPos] = _transport->read();

        if ((_readBuf[_readPos] == '\n') || (_readPos == (g_bufSize - 1)))
        {
            // Process a line
        }
        else
        {
            _readPos++;
        }
    }

    // Write data if available
    size_t bytesToDrain;
    size_t clearToWrite;
    while (((bytesToDrain = _writeBuf.availableToDrain()) > 0) &&
           ((clearToWrite = _transport->availableWrite()) > 0))
    {
        size_t writeLen = std::min(clearToWrite, bytesToDrain);

        _writeBuf.drainToTransport(_transport, writeLen);
    }
}

void VEDirectBMV::ping()
{
}

void VEDirectBMV::appVersion()
{
}

void VEDirectBMV::productId()
{
}

void VEDirectBMV::restart()
{
}

VEDirectHexFieldStringResp VEDirectBMV::getStringById(uint16_t id)
{
    JsonArray fieldDefs = g_bmvHexDefs["fields"];
    for (JsonVariant v : fieldDefs)
    {
        JsonObject fieldDef = v.as<JsonObject>();

        uint16_t fieldDefId = strtoul(fieldDef["id"], 0, 0);
        if (fieldDefId == id)
        {
            if (strcmp(fieldDef["wo"], "true") == 0)
            {
                char errorMsg[200];
                sprintf(errorMsg, "Field '0x%04X' is write-only", id);

                return VEDirectHexFieldStringResp(errorMsg);
            }
            else if (strcmp(fieldDef["storage"], "string") != 0)
            {
                return VEDirectHexFieldStringResp("Non-string field; use getById() instead");
            }
            else
            {
                return _getString(fieldDef);
            }
        }
    }

    char errorMsg[200];
    sprintf(errorMsg, "No field found with id '0x%04X'", id);

    return VEDirectHexFieldStringResp(errorMsg);
}

VEDirectHexFieldStringResp VEDirectBMV::getStringByName(const char *name)
{
    JsonArray fieldDefs = g_bmvHexDefs["fields"];
    for (JsonVariant v : fieldDefs)
    {
        JsonObject fieldDef = v.as<JsonObject>();

        if (strcmp(fieldDef["name"], name) == 0)
        {
            if (strcmp(fieldDef["wo"], "true") == 0)
            {
                char errorMsg[200];
                sprintf(errorMsg, "Field '%s' is write-only", name);

                return VEDirectHexFieldStringResp(errorMsg);
            }
            else if (strcmp(fieldDef["storage"], "string") != 0)
            {
                return VEDirectHexFieldStringResp("Non-string field; use getByName() instead");
            }
            else
            {
                return _getString(fieldDef);
            }
        }
    }

    char errorMsg[200];
    sprintf(errorMsg, "No field found with name '%s'", name);

    return VEDirectHexFieldStringResp(errorMsg);
}

VEDirectHexFieldResp VEDirectBMV::getById(uint16_t id)
{
    JsonArray fieldDefs = g_bmvHexDefs["fields"];
    for (JsonVariant v : fieldDefs)
    {
        JsonObject fieldDef = v.as<JsonObject>();

        uint16_t fieldDefId = strtoul(fieldDef["id"], 0, 0);
        if (fieldDefId == id)
        {
            if (strcmp(fieldDef["wo"], "true") == 0)
            {
                char errorMsg[200];
                sprintf(errorMsg, "Field '0x%04X' is write-only", id);

                return VEDirectHexFieldResp(errorMsg);
            }
            else if (strcmp(fieldDef["storage"], "string") == 0)
            {
                return VEDirectHexFieldResp("String field; use getStringById() instead");
            }
            else
            {
                return _get(fieldDef);
            }
        }
    }

    char errorMsg[200];
    sprintf(errorMsg, "No field found with id '0x%04X'", id);

    return VEDirectHexFieldResp(errorMsg);
}

VEDirectHexFieldResp VEDirectBMV::getByName(const char *name)
{
    JsonArray fieldDefs = g_bmvHexDefs["fields"];
    for (JsonVariant v : fieldDefs)
    {
        JsonObject fieldDef = v.as<JsonObject>();

        if (strcmp(fieldDef["name"], name) == 0)
        {
            if (strcmp(fieldDef["wo"], "true") == 0)
            {
                char errorMsg[200];
                sprintf(errorMsg, "Field '%s' is write-only", name);

                return VEDirectHexFieldResp(errorMsg);
            }
            else if (strcmp(fieldDef["storage"], "string") == 0)
            {
                return VEDirectHexFieldResp("String field; use getStringByName() instead");
            }
            else
            {
                return _get(fieldDef);
            }
        }
    }

    char errorMsg[200];
    sprintf(errorMsg, "No field found with name '%s'", name);

    return VEDirectHexFieldResp(errorMsg);
}

VEDirectHexFieldResp VEDirectBMV::setById(uint16_t id, VEDirectHexValue value)
{
    JsonArray fieldDefs = g_bmvHexDefs["fields"];
    for (JsonVariant v : fieldDefs)
    {
        JsonObject fieldDef = v.as<JsonObject>();

        uint16_t fieldDefId = strtoul(fieldDef["id"], 0, 0);
        if (fieldDefId == id)
        {
            if (strcmp(fieldDef["ro"], "true") == 0)
            {
                char errorMsg[200];
                sprintf(errorMsg, "Field '0x%04X' is read-only", id);

                return VEDirectHexFieldResp(errorMsg);
            }
            else
            {
                return _set(fieldDef, value);
            }
        }
    }

    char errorMsg[200];
    sprintf(errorMsg, "No field found with id '0x%04X'", id);

    return VEDirectHexFieldResp(errorMsg);
}

VEDirectHexFieldResp VEDirectBMV::setByName(const char *name, VEDirectHexValue value)
{
    JsonArray fieldDefs = g_bmvHexDefs["fields"];
    for (JsonVariant v : fieldDefs)
    {
        JsonObject fieldDef = v.as<JsonObject>();

        if (strcmp(fieldDef["name"], name) == 0)
        {
            if (strcmp(fieldDef["ro"], "true") == 0)
            {
                char errorMsg[200];
                sprintf(errorMsg, "Field '%s' is read-only", name);

                return VEDirectHexFieldResp(errorMsg);
            }
            else
            {
                return _set(fieldDef, value);
            }
        }
    }

    char errorMsg[200];
    sprintf(errorMsg, "No field found with name '%s'", name);

    return VEDirectHexFieldResp(errorMsg);
}

VEDirectHexFieldStringResp VEDirectBMV::_getString(JsonObject fieldInfo)
{
    const size_t bufLen = 100;
    uint8_t buf[bufLen];

    uint16_t id = strtoul(fieldInfo["id"], 0, 0);

    sendGetCommand(id, buf, bufLen);

    int len = readResponse(buf, bufLen);
    if (len < 0)
    {
        if (len == g_errTimeout)
        {
            return VEDirectHexFieldStringResp("Timeout waiting for response");
        }
        else if (len == g_errMsgTooLong)
        {
            return VEDirectHexFieldStringResp("Message buffer overflow reading response");
        }
        else
        {
            return VEDirectHexFieldStringResp("Unknown error reading response");
        }
    }
    else
    {
    }

    return VEDirectHexFieldStringResp("Not implemented yet");
}

VEDirectHexFieldResp VEDirectBMV::_get(JsonObject fieldInfo)
{
    const size_t bufLen = 100;
    uint8_t buf[bufLen];

    uint16_t id = strtoul(fieldInfo["id"], 0, 0);

    sendGetCommand(id, buf, bufLen);

    int len = readResponse(buf, bufLen);
    if (len < 0)
    {
        if (len == g_errTimeout)
        {
            return VEDirectHexFieldResp("Timeout waiting for response");
        }
        else if (len == g_errMsgTooLong)
        {
            return VEDirectHexFieldResp("Message buffer overflow reading response");
        }
        else
        {
            return VEDirectHexFieldResp("Unknown error reading response");
        }
    }
    else
    {
    }

    return VEDirectHexFieldResp("Not implemented yet");
}

VEDirectHexFieldResp VEDirectBMV::_set(JsonObject fieldInfo, VEDirectHexValue value)
{
    return VEDirectHexFieldResp("Not implemented yet");
}

void VEDirectBMV::sendGetCommand(uint16_t id,
                                 uint8_t *buf,
                                 int bufLen)
{
    int idx = 0;

    buf[idx++] = ':';
    buf[idx++] = VEDirectHexValue::hexDigits[c_get];

    idx += VEDirectHexValue::intToVicUn16(id, (char *)(&(buf[idx])));

    uint8_t *idBytes = (uint8_t *)(&id);
    uint8_t check = g_checkMagic - (c_get + idBytes[0] + idBytes[1]);

    idx += VEDirectHexValue::intToVicUn8(check, (char *)(&(buf[idx])));

    _transport->write(buf, idx);
}

int VEDirectBMV::readResponse(uint8_t *buf, size_t bufLen)
{
    unsigned long millisTimeout = millis() + g_responseTimeout_ms;
    int idx = 0;
    bool responseStarted = false;
    bool responseComplete = false;
    while (!responseComplete)
    {
        if (millis() > millisTimeout)
        {
            return g_errTimeout;
        }

        if (_transport->availableRead() > 0)
        {
            int ch = _transport->read();

            if (!responseStarted)
            {
                if (ch == ':')
                {
                    responseStarted = true;
                    buf[idx++] = ch;
                }
            }
            else
            {
                if (ch == '\n')
                {
                    buf[idx] = '\0';
                    responseComplete = true;
                }
                else
                {
                    buf[idx++] = ch;
                    if (idx >= bufLen)
                    {
                        return g_errMsgTooLong;
                    }
                }
            }
        }
    }

    return idx;
}
