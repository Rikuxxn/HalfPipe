//=============================================================================
//
// シャドウマップ処理 [ShadowMap.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _SHADOWMAP_H_// このマクロ定義がされていなかったら
#define _SHADOWMAP_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************


// 前方宣言
class CRenderTarget;


//*****************************************************************************
// シャドウマップクラス
//*****************************************************************************
class CShadowMap
{
public:
    CShadowMap();
    ~CShadowMap();

    HRESULT Init(void);

    void Begin(void);
    void End(void);

    const D3DXMATRIX& GetViewMatrix         (void) const { return m_View; }
    const D3DXMATRIX& GetProjectionMatrix   (void) const { return m_Projection; }
    const D3DXVECTOR3& GetLightDirection    (void) const { return m_LightDir; }
    IDirect3DTexture9* GetShadowTexture     (void) const;

    // デバイスリセット時
    void OnLostDevice(void);
    void OnResetDevice(void);

private:

    //----------------------------------
    // ライト行列
    //----------------------------------

    D3DXMATRIX m_View;
    D3DXMATRIX m_Projection;

    // ライト方向
    D3DXVECTOR3 m_LightDir;

    std::unique_ptr<CRenderTarget> m_RenderTarget;
};

#endif