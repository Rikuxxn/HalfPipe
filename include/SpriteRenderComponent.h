//=============================================================================
//
// 2Dレンダーコンポーネント処理 [SpriteRenderComponent.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _SPRITE_RENDER_COMPONENT_H_// このマクロ定義がされていなかったら
#define _SPRITE_RENDER_COMPONENT_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "RenderComponent.h"

//*****************************************************************************
// スプライトコンポーネントクラス
//*****************************************************************************
class CSpriteRenderComponent : public CRenderComponent
{
private:

    ComPtr<IDirect3DVertexBuffer9> m_pVtxBuff;
    D3DXCOLOR    m_Color;
    RECT        m_UV;
    float       m_Width;
    float       m_Height;
    bool m_isVisible = true;

public:

    CSpriteRenderComponent();
    ~CSpriteRenderComponent() = default;

public:

    HRESULT Init(void) override;
	void Draw(void) override;

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

    void SetIsVisible(bool visible) { m_isVisible = visible; }

    bool IsVisible(void) { return m_isVisible; }
};

#endif