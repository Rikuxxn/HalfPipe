//=============================================================================
//
// レンダーターゲットチェンジコンポーネント処理 [RenderTargetChangeComponent.h]
// Author : SOICHIRO SASAKI
//
//=============================================================================
#ifndef _RENDERTARGET_CHANGE_COMPONENT_H_// このマクロ定義がされていなかったら
#define _RENDERTARGET_CHANGE_COMPONENT_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "RenderComponent.h"

class CShaderResource;
class CRenderTarget;
class CRenderTargetMulti;

//*****************************************************************************
// スプライトコンポーネントクラス
//*****************************************************************************
class CRenderTargetChangeComponent : public CRenderComponent
{
private:

    ComPtr<IDirect3DVertexBuffer9> m_pVtxBuff;
    std::shared_ptr<CShaderResource> m_Shader;
    D3DXCOLOR    m_Color;
    RECT        m_UV;
    CRenderTarget m_pRenderTarget;
    CRenderTargetMulti m_pRenderTargetMulti;
    float       m_Width;
    float       m_Height;
    bool m_isVisible = true;
    bool m_isMulti = false;

public:

    CRenderTargetChangeComponent();
    ~CRenderTargetChangeComponent() = default;

public:

    HRESULT Init(void) override;
    void Uninit(void) override;
    void Draw(void) override;
    void DrawPolygon(void);

    // 影を描画するか
    bool IsShadowCaster(void) override
    {
        return false;
    }

public:

    void SetSize(float width, float height)
    {
        m_Width = width;
        m_Height = height;
    }

    void SetColor(D3DXCOLOR color)
    {
        m_Color = color;
    }

    void SetUV(
        int left,
        int top,
        int right,
        int bottom)
    {
        m_UV.left = left;
        m_UV.top = top;
        m_UV.right = right;
        m_UV.bottom = bottom;
    }

    void SetShader(std::shared_ptr<CShaderResource> shader)
    {
        m_Shader = shader;
    }

    void SetIsVisible(bool visible) { m_isVisible = visible; }

    bool IsVisible(void) { return m_isVisible; }

    void SetRenderTargetMulti(bool isMulti) { m_isMulti; }

    CRenderTarget GetRenderTarget(void) { return m_pRenderTarget; }
    CRenderTargetMulti GetRenderTargetMulti(void) { return m_pRenderTargetMulti; }
};

#endif