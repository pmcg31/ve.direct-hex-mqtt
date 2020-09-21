#include "VEDirectUtils.hpp"
#include "VEDirectHexFieldResp.hpp"

VEDirectHexFieldResp::VEDirectHexFieldResp(const char *msg,
                                           VEDirectHexValue value,
                                           JsonObject fieldInfo)
    : _isError(false), _value(value), _fieldInfo(fieldInfo)
{
    VEDirectUtils::maxStrcpy(_msg, msg, g_maxMsgLen);
}

VEDirectHexFieldResp::VEDirectHexFieldResp(const char *errorMsg)
    : _isError(true), _value((uint32_t)0)
{
    VEDirectUtils::maxStrcpy(_msg, errorMsg, g_maxMsgLen);
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
