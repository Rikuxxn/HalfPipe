//=============================================================================
//
// 2Dレンダーコンポーネント処理 [SpriteComponent.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "SpriteRenderComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "Manager.h"
#include "TextureResource.h"
#include "Renderer.h"
#include "MathConstants.h"
#include "ColorConstants.h"
#include "UIComponent.h"

// 名前空間の使用
using namespace MathConst;
using namespace ColorConst;


//=============================================================================
// コンストラクタ
//=============================================================================
CSpriteRenderComponent::CSpriteRenderComponent() :
    m_pVtxBuff  (nullptr),
    m_Color     (ColorConst::DX::WHITE),
    m_Width     (100.0f),
    m_Height    (100.0f)
{
    m_UV.left = 0;
    m_UV.top = 0;
    m_UV.right = 1;
    m_UV.bottom = 1;
}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CSpriteRenderComponent::Init(void)
{
    // デバイスの取得
    auto device = CManager::Instance()->GetRenderer()->GetDevice();

    device->CreateVertexBuffer(
        sizeof(VERTEX_2D) * MathConst::VERTEX,
        0,
        FVF_VERTEX_2D,
        D3DPOOL_MANAGED,
        m_pVtxBuff.GetAddressOf(),
        nullptr);

    return S_OK;
}

//=============================================================================
// 描画処理
//=============================================================================
void CSpriteRenderComponent::Draw(void)
{
    if (!m_isVisible)
    {
        return;
    }

    //auto transform = m_pOwner->GetComponent<CTransformComponent>();

    // UIコンポーネントの取得
    auto ui = m_pOwner->GetComponent<CUIComponent>();

    if (!ui)
    {
        return;
    }

    // UIトランスフォームの取得
    const auto& transform = ui->GetTransform();

    // デバイスの取得
    auto device = CManager::Instance()->GetRenderer()->GetDevice();

    //---------------------------------
    // 画面サイズ
    //---------------------------------

    float screenW = (float)CManager::Instance()->GetRenderer()->GetBackBufferWidth();
    float screenH = (float)CManager::Instance()->GetRenderer()->GetBackBufferHeight();

    //---------------------------------
    // 比率 → ピクセル変換
    //---------------------------------

    float centerX = transform.Position.x * screenW;
    float centerY = transform.Position.y * screenH;

    float z = 0.0f;

    float w = screenW * m_Width * transform.Scale.x;
    float h = screenH * m_Height * transform.Scale.y;

    //---------------------------------
    // 中心基準
    //---------------------------------

    float left = centerX - (w * MathConst::HALF);
    float right = centerX + (w * MathConst::HALF);
    float top = centerY - (h * MathConst::HALF);
    float bottom = centerY + (h * MathConst::HALF);

    VERTEX_2D* vertex = nullptr;

    // 頂点バッファをロックし、頂点情報へのポインタを取得
    m_pVtxBuff->Lock(0, 0, (void**)&vertex, 0);

    vertex[0].pos = { left,  top,    z };
    vertex[1].pos = { right, top,    z };
    vertex[2].pos = { left,  bottom, z };
    vertex[3].pos = { right, bottom, z };

    // UIの色と透明度を反映
    D3DXCOLOR color = transform.Color;
    color.a *= transform.Alpha;

    for (int i = 0; i < MathConst::VERTEX; i++)
    {
        // rhwの設定
        vertex[i].rhw = 1.0f;

        // 色の設定
        vertex[i].col = color;
    }

    float u0 = (float)m_UV.left;
    float v0 = (float)m_UV.top;

    float u1 = (float)m_UV.right;
    float v1 = (float)m_UV.bottom;

    vertex[0].tex = { u0, v0 };
    vertex[1].tex = { u1, v0 };
    vertex[2].tex = { u0, v1 };
    vertex[3].tex = { u1, v1 };

    //頂点バッファをアンロックする
    m_pVtxBuff->Unlock();

    // αテストを有効
    device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);//デフォルトはfalse
    device->SetRenderState(D3DRS_ALPHAREF, 0);
    device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);//0より大きかったら描画

    // テクスチャの設定
    device->SetTexture(0, m_TextureResource ? m_TextureResource->Texture.Get() : nullptr);

    // 頂点バッファをデータストリームに設定
    device->SetStreamSource(0, m_pVtxBuff.Get(), 0, sizeof(VERTEX_2D));

    // 頂点フォーマットの設定
    device->SetFVF(FVF_VERTEX_2D);

    // ポリゴンの描画
    device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

    // αテストを無効に戻す
    device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);//デフォルトはfalse
}