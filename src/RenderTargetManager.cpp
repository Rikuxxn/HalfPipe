//=============================================================================
//
// レンダーターゲットマネージャー処理 [RenderTargetManager.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "RenderTargetManager.h"
#include "RenderTarget.h"
#include "DeviceResourceManager.h"



//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CRenderTargetManager::Init(void)
{
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void CRenderTargetManager::Uninit(void)
{
    auto& resourceManager = CDeviceResourceManager::GetInstance();

    for (auto& pair : m_RenderTargets)
    {
        resourceManager.Unregister(pair.second.get());
    }

    // 所有しているレンダーターゲットを破棄
    m_RenderTargets.clear();
}

//=============================================================================
// リリース処理
//=============================================================================
void CRenderTargetManager::Release(const std::string& name)
{
    auto itr = m_RenderTargets.find(name);

    if (itr == m_RenderTargets.end())
    {
        return;
    }

    CRenderTarget* pRT = itr->second.get();

    // 先に登録解除
    CDeviceResourceManager::GetInstance().Unregister(pRT);

    m_RenderTargets.erase(itr);
}

//=============================================================================
// 生成
//=============================================================================
CRenderTarget* CRenderTargetManager::Create(const std::string& name, UINT width, UINT height, D3DFORMAT color, D3DFORMAT depth)
{
    auto itr = m_RenderTargets.find(name);

    if (itr != m_RenderTargets.end())
    {
        return itr->second.get();
    }

    // レンダーターゲットの生成
    auto rt = std::make_unique<CRenderTarget>();

    if (FAILED(rt->Create(
        width,
        height,
        color,
        depth)))
    {
        return nullptr;
    }

    CRenderTarget* pRT = rt.get();

    // デバイスリソース管理へ登録
    CDeviceResourceManager::GetInstance().Register(pRT);

    m_RenderTargets.emplace(name, std::move(rt));

    return pRT;
}

//=============================================================================
// 取得
//=============================================================================
CRenderTarget* CRenderTargetManager::Get(const std::string& name)
{
    auto itr = m_RenderTargets.find(name);

    if (itr == m_RenderTargets.end())
    {
        return nullptr;
    }

    return itr->second.get();
}
