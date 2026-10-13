//=============================================================================
//
// Xモデルレンダーコンポーネント処理 [Component.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _XMODEL_RENDER_COMPONENT_H_// このマクロ定義がされていなかったら
#define _XMODEL_RENDER_COMPONENT_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "RenderComponent.h"

// 前方宣言
class CXModelResource;

//*****************************************************************************
// Xモデルレンダーコンポーネント
//*****************************************************************************
class CXModelRenderComponent : public CRenderComponent
{
public:

    std::shared_ptr<CXModelResource> m_Model;

    D3DXCOLOR m_Color;

    bool m_Visible;

public:

    CXModelRenderComponent()
    {
        //SetLayer(RenderLayer::Model);
        m_Color = D3DXCOLOR(1, 1, 1, 1);
        m_Visible = true;
    }

    D3DXVECTOR3 GetModelSize(void) const;
    std::shared_ptr<CXModelResource> GetModel(void) const;

public:

    virtual void Draw(void) override;

    HRESULT Load(const char* filepath);

    // 表示・非表示
    void SetVisible(bool visible) { m_Visible = visible; }
    bool IsVisible(void) const { return m_Visible; }

private:

    // シャドウ描画用
    void DrawShadow(void) override
    {
        Draw();
    }

    // 影を描画するか
    bool IsShadowCaster(void) override
    {
        return true;
    }

};

#endif