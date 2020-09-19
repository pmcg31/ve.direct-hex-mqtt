#ifndef __H_VE_DIRECT_HEX_FIELD_STRING_RESP__
#define __H_VE_DIRECT_HEX_FIELD_STRING_RESP__

#include <ArduinoJson.h>
#include "VEDirectHexValue.hpp"

class VEDirectHexFieldStringResp
{
public:
    VEDirectHexFieldStringResp(const char *msg, const char *value, JsonObject fieldInfo);
    VEDirectHexFieldStringResp(const char *errorMsg);

    bool isError() const;

    const char *getMessage() const;
    const char *getValue() const;
    const JsonObject getFieldInfo() const;

private:
    static const int g_maxMsgLen = 200;
    static const int g_maxValueLen = 33;

private:
    bool _isError;
    char _msg[g_maxMsgLen];
    char _value[g_maxValueLen];
    JsonObject _fieldInfo;
};

#endif