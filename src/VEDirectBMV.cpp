#include <stdlib.h>
#include <algorithm>
#include "VEDirectUtils.hpp"
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
    : _transport(transport)
{
}

VEDirectBMVAppVersionResp VEDirectBMV::ping()
{
    const size_t bufLen = 100;
    uint8_t buf[bufLen];
    int idx = 0;

    // VE.Direct messages start with a colon and the command nibble
    buf[idx++] = ':';
    buf[idx++] = VEDirectUtils::hexDigits[c_ping];

    // Compute the check byte
    uint8_t check = g_checkMagic - c_ping;

    // Add check byte to message
    idx += VEDirectUtils::intToVicUn8(check, (char *)(&(buf[idx])));

    // Messages end with newline
    buf[idx++] = '\n';

    // Send it
    _transport->write(buf, idx);

    // Read the response synchronously
    // (there is a built-in timeout in
    // _readResponse)
    int len = _readResponse(buf, bufLen);

    // Error?
    if (len < 0)
    {
        // Unfortunately, yes
        if (len == g_errTimeout)
        {
            return VEDirectBMVAppVersionResp("Timeout waiting for response");
        }
        else if (len == g_errMsgTooLong)
        {
            return VEDirectBMVAppVersionResp("Message buffer overflow reading response");
        }
        else
        {
            return VEDirectBMVAppVersionResp("Unknown error reading response");
        }
    }

    // Check response type
    CheckResponseResult res = _checkResponseType(buf[1], r_ping);
    if (res != cr_ok)
    {
        switch (res)
        {
        case cr_badVicResponseType:
            // buf[1] didn't contain a hex digit
            return VEDirectBMVAppVersionResp("Incorrectly formatted response from device");
        case cr_cmdNotRecognized:
            // 'Unknown' response; our request was
            // not recognized
            return VEDirectBMVAppVersionResp("Command not recognized by device");
        case cr_unknownResponseType:
            // Some other response type, definitely
            // not what was asked for
            char errorMsg[200];
            sprintf(errorMsg, "Unexpected response type to 'ping' command: 0x%X resp: [%s]", buf[1], buf);

            return VEDirectBMVAppVersionResp(errorMsg);
        default:
            // We have no idea what went wrong
            return VEDirectBMVAppVersionResp("Unknown error");
        }
    }

    // Payload should be a un16; verify
    int payloadSize = len - 4;
    if (payloadSize != 4)
    {
        char errorMsg[200];
        sprintf(errorMsg, "Unexpected payload size: expected %d bytes, received %d",
                4, payloadSize);

        return VEDirectBMVAppVersionResp(errorMsg);
    }

    // Get payload as a uint16_t
    uint16_t payload = VEDirectUtils::vicUn16ToInt((char *)(&(buf[2])));

    // The two most significant bits indicate
    // the firmware type
    VEDirectBMVAppVersionResp::FirmwareType firmwareType = VEDirectBMVAppVersionResp::ft_bootloader;
    switch ((payload >> 14) & 0x0003)
    {
    case 0:
        // Bootloader
        firmwareType = VEDirectBMVAppVersionResp::ft_bootloader;
        break;
    case 1:
        // Application
        firmwareType = VEDirectBMVAppVersionResp::ft_application;
        break;
    case 2:
        // Tester
        firmwareType = VEDirectBMVAppVersionResp::ft_tester;
        break;
    case 3:
        // Release candidate
        firmwareType = VEDirectBMVAppVersionResp::ft_releaseCandidate;
        break;
    }

    // Clear the firmware bits
    payload &= 0x3fff;

    // Decode the version
    uint8_t *payloadBytes = (uint8_t *)&payload;
    char appVersion[20];
    if (VEDirectUtils::isLittleEndian)
    {
        sprintf(appVersion, "%X.%02X", payloadBytes[1], payloadBytes[0]);
    }
    else
    {
        sprintf(appVersion, "%X.%02X", payloadBytes[0], payloadBytes[1]);
    }

    return VEDirectBMVAppVersionResp(appVersion, firmwareType);
}

VEDirectBMVAppVersionResp VEDirectBMV::appVersion()
{
    const size_t bufLen = 100;
    uint8_t buf[bufLen];
    int idx = 0;

    // VE.Direct messages start with a colon and the command nibble
    buf[idx++] = ':';
    buf[idx++] = VEDirectUtils::hexDigits[c_appVer];

    // Compute the check byte
    uint8_t check = g_checkMagic - c_appVer;

    // Add check byte to message
    idx += VEDirectUtils::intToVicUn8(check, (char *)(&(buf[idx])));

    // Messages end with newline
    buf[idx++] = '\n';

    // Send it
    _transport->write(buf, idx);

    // Read the response synchronously
    // (there is a built-in timeout in
    // _readResponse)
    int len = _readResponse(buf, bufLen);

    // Error?
    if (len < 0)
    {
        // Unfortunately, yes
        if (len == g_errTimeout)
        {
            return VEDirectBMVAppVersionResp("Timeout waiting for response");
        }
        else if (len == g_errMsgTooLong)
        {
            return VEDirectBMVAppVersionResp("Message buffer overflow reading response");
        }
        else
        {
            return VEDirectBMVAppVersionResp("Unknown error reading response");
        }
    }

    // Check response type
    CheckResponseResult res = _checkResponseType(buf[1], r_done);
    if (res != cr_ok)
    {
        switch (res)
        {
        case cr_badVicResponseType:
            // buf[1] didn't contain a hex digit
            return VEDirectBMVAppVersionResp("Incorrectly formatted response from device");
        case cr_cmdNotRecognized:
            // 'Unknown' response; our request was
            // not recognized
            return VEDirectBMVAppVersionResp("Command not recognized by device");
        case cr_unknownResponseType:
            // Some other response type, definitely
            // not what was asked for
            char errorMsg[200];
            sprintf(errorMsg, "Unexpected response type to 'appVersion' command: 0x%X resp: [%s]", buf[1], buf);

            return VEDirectBMVAppVersionResp(errorMsg);
        default:
            // We have no idea what went wrong
            return VEDirectBMVAppVersionResp("Unknown error");
        }
    }

    // Payload should be a un16; verify
    int payloadSize = len - 4;
    if (payloadSize != 4)
    {
        char errorMsg[200];
        sprintf(errorMsg, "Unexpected payload size: expected %d bytes, received %d",
                4, payloadSize);

        return VEDirectBMVAppVersionResp(errorMsg);
    }

    // Get payload as a uint16_t
    uint16_t payload = VEDirectUtils::vicUn16ToInt((char *)(&(buf[2])));

    // The two most significant bits indicate
    // the firmware type
    VEDirectBMVAppVersionResp::FirmwareType firmwareType = VEDirectBMVAppVersionResp::ft_bootloader;
    switch ((payload >> 14) & 0x0003)
    {
    case 0:
        // Bootloader
        firmwareType = VEDirectBMVAppVersionResp::ft_bootloader;
        break;
    case 1:
        // Application
        firmwareType = VEDirectBMVAppVersionResp::ft_application;
        break;
    case 2:
        // Tester
        firmwareType = VEDirectBMVAppVersionResp::ft_tester;
        break;
    case 3:
        // Release candidate
        firmwareType = VEDirectBMVAppVersionResp::ft_releaseCandidate;
        break;
    }

    // Clear the firmware bits
    payload &= 0x3fff;

    // Decode the version
    uint8_t *payloadBytes = (uint8_t *)&payload;
    char appVersion[20];
    if (VEDirectUtils::isLittleEndian)
    {
        sprintf(appVersion, "%X.%02X", payloadBytes[1], payloadBytes[0]);
    }
    else
    {
        sprintf(appVersion, "%X.%02X", payloadBytes[0], payloadBytes[1]);
    }

    return VEDirectBMVAppVersionResp(appVersion, firmwareType);
}

VEDirectBMVProductIdResp VEDirectBMV::productId()
{
    const size_t bufLen = 100;
    uint8_t buf[bufLen];
    int idx = 0;

    // VE.Direct messages start with a colon and the command nibble
    buf[idx++] = ':';
    buf[idx++] = VEDirectUtils::hexDigits[c_prodId];

    // Compute the check byte
    uint8_t check = g_checkMagic - c_prodId;

    // Add check byte to message
    idx += VEDirectUtils::intToVicUn8(check, (char *)(&(buf[idx])));

    // Messages end with newline
    buf[idx++] = '\n';

    // Send it
    _transport->write(buf, idx);

    // Read the response synchronously
    // (there is a built-in timeout in
    // _readResponse)
    int len = _readResponse(buf, bufLen);

    // Error?
    if (len < 0)
    {
        // Unfortunately, yes
        if (len == g_errTimeout)
        {
            return VEDirectBMVProductIdResp("Timeout waiting for response");
        }
        else if (len == g_errMsgTooLong)
        {
            return VEDirectBMVProductIdResp("Message buffer overflow reading response");
        }
        else
        {
            return VEDirectBMVProductIdResp("Unknown error reading response");
        }
    }

    // Check response type
    CheckResponseResult res = _checkResponseType(buf[1], r_done);
    if (res != cr_ok)
    {
        switch (res)
        {
        case cr_badVicResponseType:
            // buf[1] didn't contain a hex digit
            return VEDirectBMVProductIdResp("Incorrectly formatted response from device");
        case cr_cmdNotRecognized:
            // 'Unknown' response; our request was
            // not recognized
            return VEDirectBMVProductIdResp("Command not recognized by device");
        case cr_unknownResponseType:
            // Some other response type, definitely
            // not what was asked for
            char errorMsg[200];
            sprintf(errorMsg, "Unexpected response type to 'productId' command: 0x%X resp: [%s]", buf[1], buf);

            return VEDirectBMVProductIdResp(errorMsg);
        default:
            // We have no idea what went wrong
            return VEDirectBMVProductIdResp("Unknown error");
        }
    }

    // Payload should be a un16; verify
    int payloadSize = len - 4;
    if (payloadSize != 4)
    {
        char errorMsg[200];
        sprintf(errorMsg, "Unexpected payload size: expected %d bytes, received %d",
                4, payloadSize);

        return VEDirectBMVProductIdResp(errorMsg);
    }

    // Get payload as a uint16_t; call it the product id
    uint16_t productId = VEDirectUtils::vicUn16ToInt((char *)(&(buf[2])));

    // Look up the product name in the product id map
    JsonArray map = g_bmvHexDefs["map_product_id"];
    for (JsonVariant v : map)
    {
        JsonObject mapEntry = v.as<JsonObject>();

        uint16_t map_productId = strtoul(mapEntry["key"], 0, 0);
        if (map_productId == productId)
        {
            // Found it!
            return VEDirectBMVProductIdResp(productId, mapEntry["value"]);
        }
    }

    // Didn't find it, but return what we have anyway
    return VEDirectBMVProductIdResp(productId, "Unknown product name");
}

void VEDirectBMV::restart()
{
    const size_t bufLen = 100;
    uint8_t buf[bufLen];
    int idx = 0;

    // VE.Direct messages start with a colon and the command nibble
    buf[idx++] = ':';
    buf[idx++] = VEDirectUtils::hexDigits[c_restart];

    // Compute the check byte
    uint8_t check = g_checkMagic - c_restart;

    // Add check byte to message
    idx += VEDirectUtils::intToVicUn8(check, (char *)(&(buf[idx])));

    // Messages end with newline
    buf[idx++] = '\n';

    // Send it
    _transport->write(buf, idx);
}

VEDirectHexFieldStringResp VEDirectBMV::getStringById(uint16_t id)
{
    // Iterate field definitions to find the field with 'id'
    JsonArray fieldDefs = g_bmvHexDefs["fields"];
    for (JsonVariant v : fieldDefs)
    {
        JsonObject fieldDef = v.as<JsonObject>();

        uint16_t fieldDefId = strtoul(fieldDef["id"], 0, 0);
        if (fieldDefId == id)
        {
            // Found it! Is it write only?
            if (strcmp(fieldDef["wo"], "true") == 0)
            {
                // Yes, so reading it isn't going to work
                char errorMsg[200];
                sprintf(errorMsg, "Field '0x%04X' is write-only", id);

                return VEDirectHexFieldStringResp(errorMsg);
            }
            // Is it a string field?
            else if (strcmp(fieldDef["storage"], "string") != 0)
            {
                // No, this function is only for strings
                return VEDirectHexFieldStringResp("Non-string field; use getById() instead");
            }
            else
            {
                // All is well, call _getString to do the rest
                return _getString(fieldDef);
            }
        }
    }

    // Didn't find it
    char errorMsg[200];
    sprintf(errorMsg, "No field found with id '0x%04X'", id);

    return VEDirectHexFieldStringResp(errorMsg);
}

VEDirectHexFieldStringResp VEDirectBMV::getStringByName(const char *name)
{
    // Iterate field definitions to find the field with 'name'
    JsonArray fieldDefs = g_bmvHexDefs["fields"];
    for (JsonVariant v : fieldDefs)
    {
        JsonObject fieldDef = v.as<JsonObject>();

        if (strcmp(fieldDef["name"], name) == 0)
        {
            // Found it! Is it write only?
            if (strcmp(fieldDef["wo"], "true") == 0)
            {
                // Yes, so reading it isn't going to work
                char errorMsg[200];
                sprintf(errorMsg, "Field '%s' is write-only", name);

                return VEDirectHexFieldStringResp(errorMsg);
            }
            // Is it a string field?
            else if (strcmp(fieldDef["storage"], "string") != 0)
            {
                // No, this function is only for strings
                return VEDirectHexFieldStringResp("Non-string field; use getByName() instead");
            }
            else
            {
                // All is well, call _getString to do the rest
                return _getString(fieldDef);
            }
        }
    }

    // Didn't find it
    char errorMsg[200];
    sprintf(errorMsg, "No field found with name '%s'", name);

    return VEDirectHexFieldStringResp(errorMsg);
}

VEDirectHexFieldResp VEDirectBMV::getById(uint16_t id)
{
    // Iterate field definitions to find the field with 'id'
    JsonArray fieldDefs = g_bmvHexDefs["fields"];
    for (JsonVariant v : fieldDefs)
    {
        JsonObject fieldDef = v.as<JsonObject>();

        uint16_t fieldDefId = strtoul(fieldDef["id"], 0, 0);
        if (fieldDefId == id)
        {
            // Found it! Is it write only?
            if (strcmp(fieldDef["wo"], "true") == 0)
            {
                // Yes, so reading it isn't going to work
                char errorMsg[200];
                sprintf(errorMsg, "Field '0x%04X' is write-only", id);

                return VEDirectHexFieldResp(errorMsg);
            }
            // Is it a string field?
            else if (strcmp(fieldDef["storage"], "string") == 0)
            {
                // Yes, this function isn't for strings
                return VEDirectHexFieldResp("String field; use getStringById() instead");
            }
            else
            {
                // All is well, call _get to do the rest
                return _get(fieldDef);
            }
        }
    }

    // Didn't find it
    char errorMsg[200];
    sprintf(errorMsg, "No field found with id '0x%04X'", id);

    return VEDirectHexFieldResp(errorMsg);
}

VEDirectHexFieldResp VEDirectBMV::getByName(const char *name)
{
    // Iterate field definitions to find the field with 'name'
    JsonArray fieldDefs = g_bmvHexDefs["fields"];
    for (JsonVariant v : fieldDefs)
    {
        JsonObject fieldDef = v.as<JsonObject>();

        if (strcmp(fieldDef["name"], name) == 0)
        {
            // Found it! Is it write only?
            if (strcmp(fieldDef["wo"], "true") == 0)
            {
                // Yes, so reading it isn't going to work
                char errorMsg[200];
                sprintf(errorMsg, "Field '%s' is write-only", name);

                return VEDirectHexFieldResp(errorMsg);
            }
            // Is it a string field?
            else if (strcmp(fieldDef["storage"], "string") == 0)
            {
                // Yes, this function isn't for strings
                return VEDirectHexFieldResp("String field; use getStringByName() instead");
            }
            else
            {
                // All is well, call _get to do the rest
                return _get(fieldDef);
            }
        }
    }

    // Didn't find it
    char errorMsg[200];
    sprintf(errorMsg, "No field found with name '%s'", name);

    return VEDirectHexFieldResp(errorMsg);
}

VEDirectHexFieldResp VEDirectBMV::setById(uint16_t id, VEDirectHexValue value)
{
    // Iterate field definitions to find the field with 'id'
    JsonArray fieldDefs = g_bmvHexDefs["fields"];
    for (JsonVariant v : fieldDefs)
    {
        JsonObject fieldDef = v.as<JsonObject>();

        uint16_t fieldDefId = strtoul(fieldDef["id"], 0, 0);
        if (fieldDefId == id)
        {
            // Found it! Is it read only?
            if (strcmp(fieldDef["ro"], "true") == 0)
            {
                // Yes, so setting it isn't going to work
                char errorMsg[200];
                sprintf(errorMsg, "Field '0x%04X' is read-only", id);

                return VEDirectHexFieldResp(errorMsg);
            }
            else
            {
                // All is well, call _set to do the rest
                return _set(fieldDef, value);
            }
        }
    }

    // Didn't find it
    char errorMsg[200];
    sprintf(errorMsg, "No field found with id '0x%04X'", id);

    return VEDirectHexFieldResp(errorMsg);
}

VEDirectHexFieldResp VEDirectBMV::setByName(const char *name, VEDirectHexValue value)
{
    // Iterate field definitions to find the field with 'name'
    JsonArray fieldDefs = g_bmvHexDefs["fields"];
    for (JsonVariant v : fieldDefs)
    {
        JsonObject fieldDef = v.as<JsonObject>();

        if (strcmp(fieldDef["name"], name) == 0)
        {
            // Found it! Is it read only?
            if (strcmp(fieldDef["ro"], "true") == 0)
            {
                // Yes, so setting it isn't going to work
                char errorMsg[200];
                sprintf(errorMsg, "Field '%s' is read-only", name);

                return VEDirectHexFieldResp(errorMsg);
            }
            else
            {
                // All is well, call _set to do the rest
                return _set(fieldDef, value);
            }
        }
    }

    // Didn't find it
    char errorMsg[200];
    sprintf(errorMsg, "No field found with name '%s'", name);

    return VEDirectHexFieldResp(errorMsg);
}

VEDirectHexFieldStringResp VEDirectBMV::_getString(JsonObject fieldInfo)
{
    const size_t bufLen = 100;
    uint8_t buf[bufLen];

    // Convert field id to int
    uint16_t id = strtoul(fieldInfo["id"], 0, 0);

    // Send a get command for it
    _sendGetCommand(id, buf, bufLen);

    // Read the response synchronously
    // (there is a built-in timeout in
    // _readResponse)
    int len = _readResponse(buf, bufLen);

    // How'd we do?
    if (len < 0)
    {
        // Oh, nice! Miserable again! What flavor?
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

    // Check response type
    CheckResponseResult res = _checkResponseType(buf[1], r_get);
    if (res != cr_ok)
    {
        // Not ok (this comment tells you nothing new)
        switch (res)
        {
        case cr_badVicResponseType:
            // buf[1] didn't contain a hex digit
            return VEDirectHexFieldStringResp("Incorrectly formatted response from device");
        case cr_cmdNotRecognized:
            // 'Unknown' response; our request was
            // not recognized
            return VEDirectHexFieldStringResp("Command not recognized by device");
        case cr_unknownResponseType:
            // Some other response type, definitely
            // not what was asked for
            char errorMsg[200];
            sprintf(errorMsg, "Unexpected response type to 'getString' command: 0x%X resp: [%s]", buf[1], buf);

            return VEDirectHexFieldStringResp(errorMsg);
        default:
            // We have no idea what went wrong
            return VEDirectHexFieldStringResp("Unknown error");
        }
    }

    // Response type is good! Check id & flags
    // before we get too excited...
    uint16_t resp_id = VEDirectUtils::vicUn16ToInt((char *)(&(buf[2])));
    uint8_t resp_flags = VEDirectUtils::vicUn8ToInt((char *)(&(buf[6])));

    // Check that the id on the response matches the one requested
    if (resp_id != id)
    {
        char errorMsg[200];
        sprintf(errorMsg, "Unexpected 'id' in response to 'get' command: 0x%04X; requested 0x%04X", resp_id, id);

        return VEDirectHexFieldStringResp(errorMsg);
    }

    // No flags is good flags
    if (resp_flags != 0)
    {
        // Alas, bad flags...
        char errorMsg[200];
        int idx = 0;

        if ((resp_flags & f_unknownId) != 0)
        {
            idx += sprintf(errorMsg, "id does not exist");
        }
        if ((resp_flags & f_notSupported) != 0)
        {
            if (idx != 0)
            {
                idx += sprintf(errorMsg + idx, " | ");
            }
            idx += sprintf(errorMsg, "attempt to write read only value");
        }
        if ((resp_flags & f_parameterError) != 0)
        {
            if (idx != 0)
            {
                idx += sprintf(errorMsg + idx, " | ");
            }
            idx += sprintf(errorMsg, "parameter error");
        }

        return VEDirectHexFieldStringResp(errorMsg);
    }

    // Good message! The rest of the message is the string aside from the last check byte
    int payloadLen = len - 10;
    char str[33];
    if (payloadLen <= 64)
    {
        int msgIdx = 8;
        int strIdx = 0;
        for (; msgIdx < len; msgIdx += 2, strIdx++)
        {
            str[strIdx] = VEDirectUtils::vicUn8ToInt((const char *)(&(buf[msgIdx])));
            if (str[strIdx] == 0)
            {
                break;
            }
        }
        str[32] = '\0';

        return VEDirectHexFieldStringResp((const char *)buf, str, fieldInfo);
    }
    else
    {
        return VEDirectHexFieldStringResp("Response string too long");
    }

    // If this line is reached then something was missed above...
    return VEDirectHexFieldStringResp("Unknown error");
}

VEDirectHexFieldResp VEDirectBMV::_get(JsonObject fieldInfo)
{
    const size_t bufLen = 100;
    uint8_t buf[bufLen];

    // Convert field id to int
    uint16_t id = strtoul(fieldInfo["id"], 0, 0);

    // Send a get command for it
    _sendGetCommand(id, buf, bufLen);

    // Read the response synchronously
    // (there is a built-in timeout in
    // _readResponse)
    int len = _readResponse(buf, bufLen);

    // How'd we do?
    if (len < 0)
    {
        // Oh, nice! Miserable again! What flavor?
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

    // Check response type
    CheckResponseResult res = _checkResponseType(buf[1], r_get);
    if (res != cr_ok)
    {
        // Not ok (this comment tells you nothing new)
        switch (res)
        {
        case cr_badVicResponseType:
            // buf[1] didn't contain a hex digit
            return VEDirectHexFieldResp("Incorrectly formatted response from device");
        case cr_cmdNotRecognized:
            // 'Unknown' response; our request was
            // not recognized
            return VEDirectHexFieldResp("Command not recognized by device");
        case cr_unknownResponseType:
            // Some other response type, definitely
            // not what was asked for
            char errorMsg[200];
            sprintf(errorMsg, "Unexpected response type to 'get' command: 0x%X resp: [%s]", buf[1], buf);

            return VEDirectHexFieldResp(errorMsg);
        default:
            // We have no idea what went wrong
            return VEDirectHexFieldResp("Unknown error");
        }
    }

    // Response type is good! Check id & flags
    // before we get too excited...
    uint16_t resp_id = VEDirectUtils::vicUn16ToInt((char *)(&(buf[2])));
    uint8_t resp_flags = VEDirectUtils::vicUn8ToInt((char *)(&(buf[6])));

    // Check that the id on the response matches the one requested
    if (resp_id != id)
    {
        char errorMsg[200];
        sprintf(errorMsg, "Unexpeced 'id' in response to 'get' command: 0x%04X; requested 0x%04X", resp_id, id);

        return VEDirectHexFieldResp(errorMsg);
    }

    // No flags is good flags
    if (resp_flags != 0)
    {
        // Alas, bad flags...
        char errorMsg[200];
        int idx = 0;

        if ((resp_flags & f_unknownId) != 0)
        {
            idx += sprintf(errorMsg, "id does not exist");
        }
        if ((resp_flags & f_notSupported) != 0)
        {
            if (idx != 0)
            {
                idx += sprintf(errorMsg + idx, " | ");
            }
            idx += sprintf(errorMsg, "attempt to write read only value");
        }
        if ((resp_flags & f_parameterError) != 0)
        {
            if (idx != 0)
            {
                idx += sprintf(errorMsg + idx, " | ");
            }
            idx += sprintf(errorMsg, "parameter error");
        }

        return VEDirectHexFieldResp(errorMsg);
    }

    // Check storage type
    int payloadSize = len - 10;
    char *payload = (char *)(&(buf[8]));
    int expectedPayloadSize = -1;
    const char *storageType = fieldInfo["storage"];
    if (strcmp("un8", storageType) == 0)
    {
        // un8 is 2 hex digits
        if (payloadSize == 2)
        {
            return VEDirectHexFieldResp((const char *)buf,
                                        VEDirectUtils::vicUn8ToInt(payload),
                                        fieldInfo);
        }
        else
        {
            // Setting this triggers an error response below
            expectedPayloadSize = 2;
        }
    }
    else if (strcmp("sn16", storageType) == 0)
    {
        // sn16 is 4 hex digits
        if (payloadSize == 4)
        {
            return VEDirectHexFieldResp((const char *)buf,
                                        VEDirectUtils::vicSn16ToInt(payload),
                                        fieldInfo);
        }
        else
        {
            // Setting this triggers an error response below
            expectedPayloadSize = 4;
        }
    }
    else if (strcmp("un16", storageType) == 0)
    {
        // un16 is 4 hex digits
        if (payloadSize == 4)
        {
            return VEDirectHexFieldResp((const char *)buf,
                                        VEDirectUtils::vicUn16ToInt(payload),
                                        fieldInfo);
        }
        else
        {
            // Setting this triggers an error response below
            expectedPayloadSize = 4;
        }
    }
    else if (strcmp("un24", storageType) == 0)
    {
        // un24 is 6 hex digits
        if (payloadSize == 6)
        {
            return VEDirectHexFieldResp((const char *)buf,
                                        VEDirectUtils::vicUn24ToInt(payload),
                                        fieldInfo);
        }
        else
        {
            // Setting this triggers an error response below
            expectedPayloadSize = 6;
        }
    }
    else if (strcmp("sn32", storageType) == 0)
    {
        // sn32 is 8 hex digits
        if (payloadSize == 8)
        {
            return VEDirectHexFieldResp((const char *)buf,
                                        VEDirectUtils::vicSn32ToInt(payload),
                                        fieldInfo);
        }
        else
        {
            // Setting this triggers an error response below
            expectedPayloadSize = 8;
        }
    }
    else if (strcmp("un32", storageType) == 0)
    {
        // un32 is 8 hex digits
        if (payloadSize == 8)
        {
            return VEDirectHexFieldResp((const char *)buf,
                                        VEDirectUtils::vicUn32ToInt(payload),
                                        fieldInfo);
        }
        else
        {
            // Setting this triggers an error response below
            expectedPayloadSize = 8;
        }
    }
    else
    {
        // Our field definitions contain a storage type that isn't
        // being accounted for here...ooops! Go fix that...
        char errorMsg[200];
        sprintf(errorMsg, "Unknown storage type for field id 0x%04X: '%s'", id, storageType);

        return VEDirectHexFieldResp(errorMsg);
    }

    // The payload received was a different size than required
    // by the storage type in our field definitions
    if (expectedPayloadSize != -1)
    {
        char errorMsg[200];
        sprintf(errorMsg, "Unexpected payload size for field id 0x%04X: expected %d bytes, received %d",
                id, expectedPayloadSize, payloadSize);

        return VEDirectHexFieldResp(errorMsg);
    }

    // If this line is reached then something was missed above...
    return VEDirectHexFieldResp("Unknown error");
}

VEDirectHexFieldResp VEDirectBMV::_set(JsonObject fieldInfo, VEDirectHexValue value)
{
    const size_t bufLen = 100;
    uint8_t buf[bufLen];
    int idx = 0;

    // Convert field id to int
    uint16_t id = strtoul(fieldInfo["id"], 0, 0);

    // Send a set command for it
    // VE.Direct messages start with a colon and the command nibble
    buf[idx++] = ':';
    buf[idx++] = VEDirectUtils::hexDigits[c_set];

    // Add the field id
    idx += VEDirectUtils::intToVicUn16(id, (char *)(&(buf[idx])));

    // Add the flags
    idx += VEDirectUtils::intToVicUn8(0, (char *)(&(buf[idx])));

    // Add the data
    const char *storageType = fieldInfo["storage"];
    if (strcmp("un8", storageType) == 0)
    {
        idx += VEDirectUtils::intToVicUn8(value.asUn8(), (char *)(&(buf[idx])));
    }
    else if (strcmp("sn16", storageType) == 0)
    {
        idx += VEDirectUtils::intToVicSn16(value.asSn16(), (char *)(&(buf[idx])));
    }
    else if (strcmp("un16", storageType) == 0)
    {
        idx += VEDirectUtils::intToVicUn16(value.asUn16(), (char *)(&(buf[idx])));
    }
    else if (strcmp("un24", storageType) == 0)
    {
        idx += VEDirectUtils::intToVicUn24(value.asUn24(), (char *)(&(buf[idx])));
    }
    else if (strcmp("sn32", storageType) == 0)
    {
        idx += VEDirectUtils::intToVicSn32(value.asSn32(), (char *)(&(buf[idx])));
    }
    else if (strcmp("un32", storageType) == 0)
    {
        idx += VEDirectUtils::intToVicUn32(value.asUn32(), (char *)(&(buf[idx])));
    }
    else
    {
        // Our field definitions contain a storage type that isn't
        // being accounted for here...ooops! Go fix that...
        char errorMsg[200];
        sprintf(errorMsg, "Unknown storage type for field id 0x%04X: '%s'", id, storageType);

        return VEDirectHexFieldResp(errorMsg);
    }

    // Compute the check byte
    uint8_t check = c_set;
    for (int i = 2; i < idx; i += 2)
    {
        check += VEDirectUtils::vicUn8ToInt((const char *)(&(buf[i])));
    }
    check = g_checkMagic - check;

    // Add check byte to message
    idx += VEDirectUtils::intToVicUn8(check, (char *)(&(buf[idx])));

    // Messages end with newline
    buf[idx++] = '\n';

    // Send it
    _transport->write(buf, idx);
    _sendGetCommand(id, buf, bufLen);

    // Read the response synchronously
    // (there is a built-in timeout in
    // _readResponse)
    int len = _readResponse(buf, bufLen);

    // How'd we do?
    if (len < 0)
    {
        // Oh, nice! Miserable again! What flavor?
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

    // Check response type
    CheckResponseResult res = _checkResponseType(buf[1], r_set);
    if (res != cr_ok)
    {
        // Not ok (this comment tells you nothing new)
        switch (res)
        {
        case cr_badVicResponseType:
            // buf[1] didn't contain a hex digit
            return VEDirectHexFieldResp("Incorrectly formatted response from device");
        case cr_cmdNotRecognized:
            // 'Unknown' response; our request was
            // not recognized
            return VEDirectHexFieldResp("Command not recognized by device");
        case cr_unknownResponseType:
            // Some other response type, definitely
            // not what was asked for
            char errorMsg[200];
            sprintf(errorMsg, "Unexpected response type to 'set' command: 0x%X resp: [%s]", buf[1], buf);

            return VEDirectHexFieldResp(errorMsg);
        default:
            // We have no idea what went wrong
            return VEDirectHexFieldResp("Unknown error");
        }
    }

    // Response type is good! Check id & flags
    // before we get too excited...
    uint16_t resp_id = VEDirectUtils::vicUn16ToInt((char *)(&(buf[2])));
    uint8_t resp_flags = VEDirectUtils::vicUn8ToInt((char *)(&(buf[6])));

    // Check that the id on the response matches the one requested
    if (resp_id != id)
    {
        char errorMsg[200];
        sprintf(errorMsg, "Unexpected 'id' in response to 'get' command: 0x%04X; requested 0x%04X", resp_id, id);

        return VEDirectHexFieldResp(errorMsg);
    }

    // No flags is good flags
    if (resp_flags != 0)
    {
        // Alas, bad flags...
        char errorMsg[200];
        int idx = 0;

        if ((resp_flags & f_unknownId) != 0)
        {
            idx += sprintf(errorMsg, "id does not exist");
        }
        if ((resp_flags & f_notSupported) != 0)
        {
            if (idx != 0)
            {
                idx += sprintf(errorMsg + idx, " | ");
            }
            idx += sprintf(errorMsg, "attempt to write read only value");
        }
        if ((resp_flags & f_parameterError) != 0)
        {
            if (idx != 0)
            {
                idx += sprintf(errorMsg + idx, " | ");
            }
            idx += sprintf(errorMsg, "parameter error");
        }

        return VEDirectHexFieldResp(errorMsg);
    }

    // Check storage type
    int payloadSize = len - 10;
    char *payload = (char *)(&(buf[8]));
    int expectedPayloadSize = -1;
    if (strcmp("un8", storageType) == 0)
    {
        // un8 is 2 hex digits
        if (payloadSize == 2)
        {
            return VEDirectHexFieldResp((const char *)buf,
                                        VEDirectUtils::vicUn8ToInt(payload),
                                        fieldInfo);
        }
        else
        {
            // Setting this triggers an error response below
            expectedPayloadSize = 2;
        }
    }
    else if (strcmp("sn16", storageType) == 0)
    {
        // sn16 is 4 hex digits
        if (payloadSize == 4)
        {
            return VEDirectHexFieldResp((const char *)buf,
                                        VEDirectUtils::vicSn16ToInt(payload),
                                        fieldInfo);
        }
        else
        {
            // Setting this triggers an error response below
            expectedPayloadSize = 4;
        }
    }
    else if (strcmp("un16", storageType) == 0)
    {
        // un16 is 4 hex digits
        if (payloadSize == 4)
        {
            return VEDirectHexFieldResp((const char *)buf,
                                        VEDirectUtils::vicUn16ToInt(payload),
                                        fieldInfo);
        }
        else
        {
            // Setting this triggers an error response below
            expectedPayloadSize = 4;
        }
    }
    else if (strcmp("un24", storageType) == 0)
    {
        // un24 is 6 hex digits
        if (payloadSize == 6)
        {
            return VEDirectHexFieldResp((const char *)buf,
                                        VEDirectUtils::vicUn24ToInt(payload),
                                        fieldInfo);
        }
        else
        {
            // Setting this triggers an error response below
            expectedPayloadSize = 6;
        }
    }
    else if (strcmp("sn32", storageType) == 0)
    {
        // sn32 is 8 hex digits
        if (payloadSize == 8)
        {
            return VEDirectHexFieldResp((const char *)buf,
                                        VEDirectUtils::vicSn32ToInt(payload),
                                        fieldInfo);
        }
        else
        {
            // Setting this triggers an error response below
            expectedPayloadSize = 8;
        }
    }
    else if (strcmp("un32", storageType) == 0)
    {
        // un32 is 8 hex digits
        if (payloadSize == 8)
        {
            return VEDirectHexFieldResp((const char *)buf,
                                        VEDirectUtils::vicUn32ToInt(payload),
                                        fieldInfo);
        }
        else
        {
            // Setting this triggers an error response below
            expectedPayloadSize = 8;
        }
    }
    else
    {
        // Our field definitions contain a storage type that isn't
        // being accounted for here...ooops! Go fix that...
        char errorMsg[200];
        sprintf(errorMsg, "Unknown storage type for field id 0x%04X: '%s'", id, storageType);

        return VEDirectHexFieldResp(errorMsg);
    }

    // The payload received was a different size than required
    // by the storage type in our field definitions
    if (expectedPayloadSize != -1)
    {
        char errorMsg[200];
        sprintf(errorMsg, "Unexpected payload size for field id 0x%04X: expected %d bytes, received %d",
                id, expectedPayloadSize, payloadSize);

        return VEDirectHexFieldResp(errorMsg);
    }

    // If this line is reached then something was missed above...
    return VEDirectHexFieldResp("Unknown error");
}

void VEDirectBMV::_sendGetCommand(uint16_t id,
                                  uint8_t *buf,
                                  int bufLen)
{
    int idx = 0;

    // VE.Direct messages start with a colon and the command nibble
    buf[idx++] = ':';
    buf[idx++] = VEDirectUtils::hexDigits[c_get];

    // Add the field id we want to get to the message
    idx += VEDirectUtils::intToVicUn16(id, (char *)(&(buf[idx])));

    // Compute the check byte
    uint8_t *idBytes = (uint8_t *)(&id);
    uint8_t check = g_checkMagic - (c_get + idBytes[0] + idBytes[1]);

    // Add check byte to message
    idx += VEDirectUtils::intToVicUn8(check, (char *)(&(buf[idx])));

    // Messages end with newline
    buf[idx++] = '\n';

    // Ship it!
    _transport->write(buf, idx);
}

int VEDirectBMV::_readResponse(uint8_t *buf, size_t bufLen)
{
    // Set up a timeout so we don't stall
    // the microcontroller
    unsigned long millisTimeout = millis() + g_responseTimeout_ms;

    // Loop until we have a complete response
    // or time runs out
    int idx = 0;
    bool responseStarted = false;
    bool responseComplete = false;
    bool responseTypeNext = false;
    while (!responseComplete)
    {
        // How are we doing on time?
        if (millis() > millisTimeout)
        {
            // Not so good lol
            return g_errTimeout;
        }

        // If there is data available, grab the next byte
        if (_transport->availableRead() > 0)
        {
            int ch = _transport->read();

            // Are we into the response yet?
            if (!responseStarted)
            {
                // No, is this the first character?
                if (ch == ':')
                {
                    // Yes! NOW the response is started
                    responseStarted = true;
                    responseTypeNext = true;
                    buf[idx++] = ch;
                }
            }
            else
            {
                // Yes, is this the last character?
                if (ch == '\n')
                {
                    // Yes, tidy up our string and signal
                    // the end of the loop
                    buf[idx] = '\0';
                    responseComplete = true;
                }
                else
                {
                    // Is this the response type?
                    if (responseTypeNext)
                    {
                        // Yes, reset the flag
                        responseTypeNext = false;

                        // Ignore response types that don't
                        // exist in the doc or the ASYNC resp
                        if (!((ch == '1') ||
                              (ch == '3') ||
                              (ch == '5') ||
                              (ch == '7') ||
                              (ch == '8')))
                        {
                            responseStarted = false;
                            responseComplete = false;
                            responseTypeNext = false;
                            idx = 0;
                            continue;
                        }
                    }

                    // Nope, squirrel it away
                    buf[idx++] = ch;
                    if (idx >= bufLen)
                    {
                        // Squirrel ran out of hiding places
                        return g_errMsgTooLong;
                    }
                }
            }
        }
    }

    // Check checksum
    uint8_t check = 0;

    // First add the response type nibble
    for (int i = 0; i < 16; i++)
    {
        if (buf[1] == VEDirectUtils::hexDigits[i])
        {
            check = i;
            break;
        }
    }

    // Add up the rest of the bytes
    for (int i = 2; i < idx; i += 2)
    {
        check += VEDirectUtils::vicUn8ToInt((char *)(&(buf[i])));
    }

    // Survey says....
    if (check != g_checkMagic)
    {
        // XXX
        return g_badChecksum;
    }

    // Good message, idx now has the length
    return idx;
}

VEDirectBMV::CheckResponseResult
VEDirectBMV::_checkResponseType(const char vicResponseType,
                                uint8_t expectedResponseType)
{
    // Convert text nibble to int
    uint8_t responseType = 0xff;
    for (int i = 0; i < 16; i++)
    {
        if (VEDirectUtils::hexDigits[i] == vicResponseType)
        {
            responseType = i;
        }
    }

    // Did conversion fail?
    if (responseType == 0xff)
    {
        // Yup, sorry...
        return cr_badVicResponseType;
    }

    // Did we get what we wanted?
    if (responseType == expectedResponseType)
    {
        // Rainbows and Unicorns!
        return cr_ok;
    }
    else if (responseType == r_unknown)
    {
        // No, the device didn't like either
        // the command or its format
        return cr_cmdNotRecognized;
    }

    // No, we got something else entirely
    return cr_unknownResponseType;
}
