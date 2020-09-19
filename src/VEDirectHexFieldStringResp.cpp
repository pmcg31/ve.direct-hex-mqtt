#include "VEDirectHexFieldStringResp.hpp"

VEDirectHexFieldStringResp::VEDirectHexFieldStringResp(const char *msg,
                                                       const char *value,
                                                       JsonObject fieldInfo)
    : _isError(false), _fieldInfo(fieldInfo)
{
    strcpy(_msg, msg);
    strcpy(_value, value);
}

VEDirectHexFieldStringResp::VEDirectHexFieldStringResp(const char *errorMsg)
    : _isError(true)
{
    strcpy(_msg, errorMsg);
    strcpy(_value, "ERROR!");
}

bool VEDirectHexFieldStringResp::isError() const
{
    return _isError;
}

const char *VEDirectHexFieldStringResp::getMessage() const
{
    return _msg;
}

const char *VEDirectHexFieldStringResp::getValue() const
{
    return _value;
}

const JsonObject VEDirectHexFieldStringResp::getFieldInfo() const
{
    return _fieldInfo;
}
