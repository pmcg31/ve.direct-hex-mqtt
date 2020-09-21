#ifndef __H_VE_DIRECT_BMV_PING_RESP__
#define __H_VE_DIRECT_BMV_PING_RESP__

class VEDirectBMVAppVersionResp
{
public:
    enum FirmwareType
    {
        ft_bootloader,
        ft_application,
        ft_tester,
        ft_releaseCandidate
    };

public:
    VEDirectBMVAppVersionResp(const char *appVersion, FirmwareType firmwareType);
    VEDirectBMVAppVersionResp(const char *errorMsg);

    bool isError() const;
    const char *getErrorMessage() const;

    const char *getAppVersion() const;
    FirmwareType getFirmwareType() const;

private:
    static const size_t g_maxMsgLen = 200;

private:
    bool _isError;
    char _buf[g_maxMsgLen];
    FirmwareType _firmwareType;
};

#endif