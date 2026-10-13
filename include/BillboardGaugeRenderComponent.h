//=============================================================================
//
// ビルボードゲージレンダーコンポーネント処理 [BillboardGaugeRenderComponent.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _BILLBOARD_GAUGE_RENDER_COMPONENT_H_// このマクロ定義がされていなかったら
#define _BILLBOARD_GAUGE_RENDER_COMPONENT_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "BillboardRenderComponent.h"

//*****************************************************************************
// ビルボードゲージレンダーコンポーネントクラス
//*****************************************************************************
class CBillboardGaugeRenderComponent : public CBillboardRenderComponent
{
private:

    float m_Rate = 1.0f;

public:

    CBillboardGaugeRenderComponent();
    ~CBillboardGaugeRenderComponent() = default;

public:

    void Draw(void) override;

    // 影を描画するか
    bool IsShadowCaster(void) override
    {
        return false;
    }

public:

    void SetRate(float rate);
    float GetRate(void) const;

private:

    // ビルボード用頂点を作成
    virtual void SetBillboardVertex(VERTEX_3D* vertex, float rate) override;

};

#endif