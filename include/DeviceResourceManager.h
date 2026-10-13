//=============================================================================
//
// デバイスリソースマネージャー処理 [DeviceResourceManager.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _DEVICE_RESOURCE_MANAGER_H_// このマクロ定義がされていなかったら
#define _DEVICE_RESOURCE_MANAGER_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************

// 前方宣言
class IDeviceResource;


//*****************************************************************************
// デバイスリソースマネージャークラス
//*****************************************************************************
class CDeviceResourceManager
{
public:
    ~CDeviceResourceManager()
    {
        m_Resources.clear();
    }

    static CDeviceResourceManager& GetInstance(void);

    void Register(IDeviceResource* resource);

    void Unregister(IDeviceResource* resource);

    void ReleaseResource(void);

    // デバイスロスト・リセット時
    void OnLostDevice(void);
    void OnResetDevice(void);

private:

    CDeviceResourceManager() = default;

    CDeviceResourceManager(const CDeviceResourceManager&) = delete;
    CDeviceResourceManager& operator=(const CDeviceResourceManager&) = delete;

    std::list<IDeviceResource*> m_Resources;
};

#endif