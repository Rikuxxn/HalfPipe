//=============================================================================
//
// レンダーターゲットマネージャー処理 [RenderTargetManager.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _RENDER_TARGET_MANAGER_H_// このマクロ定義がされていなかったら
#define _RENDER_TARGET_MANAGER_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "RenderTarget.h"


//*****************************************************************************
// レンダーターゲットマネージャークラス
//*****************************************************************************
class CRenderTargetManager
{
public:
    ~CRenderTargetManager()
    {
        Uninit();
    }

    // インスタンスの取得
    static CRenderTargetManager& GetInstance(void)
    {
        static CRenderTargetManager instance;
        return instance;
    }

    HRESULT Init(void);
    void Uninit(void);
    void Release(const std::string& name);

    CRenderTarget* Create(
        const std::string& name,
        UINT width,
        UINT height,
        D3DFORMAT color,
        D3DFORMAT depth);

    CRenderTarget* Get(const std::string& name);

private:

    CRenderTargetManager() = default;

    CRenderTargetManager(const CRenderTargetManager&) = delete;
    CRenderTargetManager& operator=(const CRenderTargetManager&) = delete;

    std::unordered_map<std::string, std::unique_ptr<CRenderTarget>> m_RenderTargets;
};

#endif