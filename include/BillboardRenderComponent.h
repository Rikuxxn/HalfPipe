//=============================================================================
//
// ビルボードレンダーコンポーネント処理 [BillboardRenderComponent.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _BILLBOARD_RENDER_COMPONENT_H_// このマクロ定義がされていなかったら
#define _BILLBOARD_RENDER_COMPONENT_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "RenderComponent.h"

//*****************************************************************************
// ビルボードレンダーコンポーネントクラス
//*****************************************************************************
class CBillboardRenderComponent : public CRenderComponent
{
protected:

    ComPtr<IDirect3DVertexBuffer9>  m_pVtxBuff;  // 頂点バッファ
    D3DXMATRIX		                m_mtxWorld;	 // ワールドマトリックス
    D3DXCOLOR                       m_Color;     // 色
    float                           m_Width;     // 幅
    float                           m_Height;    // 高さ

private:

    RECT                            m_UV;        // UV
    bool                            m_isVisible; // 表示するか

public:

    CBillboardRenderComponent();
    ~CBillboardRenderComponent() = default;

public:

    HRESULT Init(void) override;
    void Draw(void) override;

    // ビルボード用頂点を作成
    virtual void SetBillboardVertex(VERTEX_3D* vertex, float rate = 1.0f);

    // ビルボードのワールド行列を設定
    void SetBillboardTransform(IDirect3DDevice9* device, const D3DXVECTOR3& position);

    // ビルボードの描画設定
    void BeginBillboardDraw(IDirect3DDevice9* device);

    // ビルボードを描画
    void DrawBillboard(IDirect3DDevice9* device);

    // ビルボードの描画設定を元に戻す
    void EndBillboardDraw(IDirect3DDevice9* device);

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