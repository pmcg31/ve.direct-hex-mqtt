#ifndef __H_VE_DIRECT_HEX_FIELD_RESP__
#define __H_VE_DIRECT_HEX_FIELD_RESP__

#include <ArduinoJson.h>
#include "VEDirectHexValue.hpp"

class VEDirectHexFieldResp
{
public:
    VEDirectHexFieldResp(const char *msg, VEDirectHexValue value, JsonObject fieldInfo);
    VEDirectHexFieldResp(const char *errorMsg);

    bool isError() const;

    const char *getMessage() const;
    const VEDirectHexValue getValue() const;
    const JsonObject getFieldInfo() const;

private:
    static const int g_maxMsgLen = 200;

private:
    bool _isError;
    char _msg[g_maxMsgLen];
    VEDirectHexValue _value;
    JsonObject _fieldInfo;
};

#endif