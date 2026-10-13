//=============================================================================
//
// デバイスリソースマネージャー処理 [DeviceResourceManager.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "DeviceResourceManager.h"
#include "DeviceResource.h"

//=============================================================================
// インスタンス生成
//=============================================================================
CDeviceResourceManager& CDeviceResourceManager::GetInstance(void)
{
    static CDeviceResourceManager instance;
    return instance;
}

//=============================================================================
// 登録
//=============================================================================
void CDeviceResourceManager::Register(IDeviceResource* resource)
{
    if (resource == nullptr)
    {
        return;
    }

    // 二重登録防止
    auto itr = std::find(m_Resources.begin(), m_Resources.end(), resource);

    if (itr != m_Resources.end())
    {
        return;
    }

    m_Resources.push_back(resource);
}

//=============================================================================
// 登録解除
//=============================================================================
void CDeviceResourceManager::Unregister(IDeviceResource* resource)
{
    if (resource == nullptr)
    {
        return;
    }

    m_Resources.remove(resource);
}

//=============================================================================
// リソースの破棄
//=============================================================================
void CDeviceResourceManager::ReleaseResource(void)
{
    // コピーを作る
    auto resources = m_Resources;

    for (auto* resource : resources)
    {
        if (resource != nullptr)
        {
            resource->ReleaseResource();
        }
    }

    // 全リソースの破棄処理が終わってから登録を解除
    m_Resources.clear();
}

//=============================================================================
// デバイスロスト時
//=============================================================================
void CDeviceResourceManager::OnLostDevice(void)
{
    for (auto* resource : m_Resources)
    {
        if (resource != nullptr)
        {
            resource->OnLostDevice();
        }
    }
}

//=============================================================================
// デバイスリセット時
//=============================================================================
void CDeviceResourceManager::OnResetDevice(void)
{
    for (auto* resource : m_Resources)
    {
        if (resource != nullptr)
        {
            resource->OnResetDevice();
        }
    }
}
