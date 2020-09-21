#ifndef __H_VE_DIRECT_BMV_PRODUCT_ID_RESP__
#define __H_VE_DIRECT_BMV_PRODUCT_ID_RESP__

#include <stdlib.h>
#include <stdint.h>

class VEDirectBMVProductIdResp
{
public:
    VEDirectBMVProductIdResp(uint16_t productId, const char *productName);
    VEDirectBMVProductIdResp(const char *errorMsg);

    bool isError() const;
    const char *getErrorMessage() const;

    uint16_t getProductId() const;
    const char *getProductName() const;

private:
    static const size_t g_maxMsgLen = 200;

private:
    bool _isError;
    uint16_t _productId;
    char _buf[g_maxMsgLen];
};

#endif