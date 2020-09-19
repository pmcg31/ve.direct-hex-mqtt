#include "VEDirectHexFieldResp.hpp"

VEDirectHexFieldResp::VEDirectHexFieldResp(const char *msg,
                                           VEDirectHexValue value,
                                           JsonObject fieldInfo)
    : _isError(false), _value(value), _fieldInfo(fieldInfo)
{
    strcpy(_msg, msg);
}

VEDirectHexFieldResp::VEDirectHexFieldResp(const char *errorMsg)
    : _isError(true), _value((uint32_t)0)
{
    strcpy(_msg, errorMsg);
}

bool VEDirectHexFieldResp::isError() const
{
    return _isError;
}

const char *VEDirectHexFieldResp::getMessage() const
{
    return _msg;
}

const VEDirectHexValue VEDirectHexFieldResp::getValue() const
{
    return _value;
}

const JsonObject VEDirectHexFieldResp::getFieldInfo() const
{
    return _fieldInfo;
}
