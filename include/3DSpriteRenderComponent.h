//=============================================================================
//
// 3Dレンダーコンポーネント処理 [3DSpriteRenderComponent.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _3D_SPRITE_RENDER_COMPONENT_H_// このマクロ定義がされていなかったら
#define _3D_SPRITE_RENDER_COMPONENT_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "RenderComponent.h"

//*****************************************************************************
// 3Dスプライトコンポーネントクラス (3D板ポリゴン)
//*****************************************************************************
class C3DSpriteRenderComponent : public CRenderComponent
{
private:

    ComPtr<IDirect3DVertexBuffer9>  m_pVtxBuff;
    D3DXCOLOR                       m_Color;
    float                           m_Width;
    float                           m_Height;
    bool                            m_isVisible;

public:

    C3DSpriteRenderComponent();
    ~C3DSpriteRenderComponent() = default;

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

    void SetColor(D3DXCOLOR color) { m_Color = color; }

    void SetIsVisible(bool visible) { m_isVisible = visible; }
};

#endif