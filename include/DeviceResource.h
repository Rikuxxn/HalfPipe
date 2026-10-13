//=============================================================================
//
// デバイスリソース管理システム処理 [DeviceResource.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _DEVICE_RESOURCE_H_// このマクロ定義がされていなかったら
#define _DEVICE_RESOURCE_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// デバイスリソース管理システムインターフェース
//*****************************************************************************
class IDeviceResource
{
public:

    virtual ~IDeviceResource() {}

    virtual void ReleaseResource() = 0;

    virtual void OnLostDevice(void) = 0;
    virtual void OnResetDevice(void) = 0;
};

#endif