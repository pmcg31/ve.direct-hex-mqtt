#include <string.h>
#include <algorithm>
#include "VEDirectUtils.hpp"
#include "VEDirectBMVAppVersionResp.hpp"

VEDirectBMVAppVersionResp::VEDirectBMVAppVersionResp(const char *appVersion, FirmwareType firmwareType)
    : _isError(false), _firmwareType(firmwareType)
{
    VEDirectUtils::maxStrcpy(_buf, appVersion, g_maxMsgLen);
}

VEDirectBMVAppVersionResp::VEDirectBMVAppVersionResp(const char *errorMsg)
    : _isError(true)
{
    VEDirectUtils::maxStrcpy(_buf, errorMsg, g_maxMsgLen);
}

bool VEDirectBMVAppVersionResp::isError() const
{
    return _isError;
}

const char *VEDirectBMVAppVersionResp::getErrorMessage() const
{
    if (_isError)
    {
        return (const char *)_buf;
    }
    else
    {
        return "No error";
    }
}

const char *VEDirectBMVAppVersionResp::getAppVersion() const
{
    if (_isError)
    {
        return "Error";
    }
    else
    {
        return (const char *)_buf;
    }
}

VEDirectBMVAppVersionResp::FirmwareType
VEDirectBMVAppVersionResp::getFirmwareType() const
{
    return _firmwareType;
}
