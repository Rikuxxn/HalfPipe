//=============================================================================
//
// ビルボードHPゲージコンポーネント処理 [HpGaugeomponent.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _HP_GAUGE_COMPONENT_H_// このマクロ定義がされていなかったら
#define _HP_GAUGE_COMPONENT_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "BillboardGaugeRenderComponent.h"

//*****************************************************************************
// ビルボードHPゲージコンポーネントクラス
//*****************************************************************************
class CHpGaugeComponent : public CBillboardGaugeRenderComponent
{
public:

    CHpGaugeComponent();
    ~CHpGaugeComponent() = default;

public:

    void    Draw(void) override;

    // 影を描画するか
    bool IsShadowCaster(void) override
    {
        return false;
    }

    void SetBackgroundSize(float width, float height)
    {
        m_BackgroundWidth = width;
        m_BackgroundHeight = height;
    }

    void SetBackgroundColor(D3DXCOLOR color)
    {
        m_BackgroundColor = color;
    }

    void SetGaugeSize(float width, float height)
    {
        m_GaugeWidth = width;
        m_GaugeHeight = height;
    }

    void SetGaugeColor(D3DXCOLOR color)
    {
        m_GaugeColor = color;
    }

    void SetOffset(const D3DXVECTOR3& offset)
    {
        m_Offset = offset;
    }

private:

    // ビルボード用頂点を作成
    void SetBillboardVertex(VERTEX_3D* vertex, float rate) override;

private:

    // 背景
    float       m_BackgroundWidth;
    float       m_BackgroundHeight;
    D3DXCOLOR   m_BackgroundColor;

    // ゲージ
    float       m_GaugeWidth;
    float       m_GaugeHeight;
    D3DXCOLOR   m_GaugeColor;

    D3DXVECTOR3 m_Offset;
};

#endif