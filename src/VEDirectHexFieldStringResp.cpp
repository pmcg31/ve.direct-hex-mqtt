#include "VEDirectUtils.hpp"
#include "VEDirectHexFieldStringResp.hpp"

VEDirectHexFieldStringResp::VEDirectHexFieldStringResp(const char *msg,
                                                       const char *value,
                                                       JsonObject fieldInfo)
    : _isError(false), _fieldInfo(fieldInfo)
{
    VEDirectUtils::maxStrcpy(_msg, msg, g_maxMsgLen);
    VEDirectUtils::maxStrcpy(_value, value, g_maxValueLen);
}

VEDirectHexFieldStringResp::VEDirectHexFieldStringResp(const char *errorMsg)
    : _isError(true)
{
    VEDirectUtils::maxStrcpy(_msg, errorMsg, g_maxMsgLen);
    VEDirectUtils::maxStrcpy(_value, "ERROR!", g_maxValueLen);
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
