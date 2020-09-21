#include "VEDirectUtils.hpp"
#include "VEDirectBMVProductIdResp.hpp"

VEDirectBMVProductIdResp::VEDirectBMVProductIdResp(uint16_t productId,
                                                   const char *productName)
    : _isError(false), _productId(productId)
{
    VEDirectUtils::maxStrcpy(_buf, productName, g_maxMsgLen);
}

VEDirectBMVProductIdResp::VEDirectBMVProductIdResp(const char *errorMsg)
    : _isError(true), _productId(0)
{
    VEDirectUtils::maxStrcpy(_buf, errorMsg, g_maxMsgLen);
}

bool VEDirectBMVProductIdResp::isError() const
{
    return _isError;
}

const char *VEDirectBMVProductIdResp::getErrorMessage() const
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

uint16_t VEDirectBMVProductIdResp::getProductId() const
{
    return _productId;
}

const char *VEDirectBMVProductIdResp::getProductName() const
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
