//=============================================================================
//
// レンダーターゲットチェンジコンポーネント処理 [RenderTargetChangeComponent.cpp]
// Author : SOICHIRO SASAKI
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "RenderTargetChangeComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "Manager.h"
#include "TextureResource.h"
#include "Renderer.h"
#include "MathConstants.h"
#include "ColorConstants.h"
#include "UIComponent.h"
#include "ShaderResource.h"

// 名前空間の使用
using namespace MathConst;
using namespace ColorConst;


//=============================================================================
// コンストラクタ
//=============================================================================
CRenderTargetChangeComponent::CRenderTargetChangeComponent() :
    m_pVtxBuff(nullptr),
    m_Color(ColorConst::DX::WHITE),
    m_Width(100.0f),
    m_Height(100.0f)
{
    m_UV.left = 0;
    m_UV.top = 0;
    m_UV.right = 1;
    m_UV.bottom = 1;
}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CRenderTargetChangeComponent::Init(void)
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

    UINT w = CManager::Instance()->GetRenderer()->GetBackBufferHeight();
    UINT h = CManager::Instance()->GetRenderer()->GetBackBufferWidth();


    // レンダーターゲット1
    if (FAILED(m_pRenderTarget.Create(w / 2, h / 2, D3DFMT_A32B32G32R32F, D3DFMT_UNKNOWN)))
    {
        return E_FAIL;
    }

    // レンダーターゲット2
    if (FAILED(m_pRenderTargetMulti.Create(w / 2, h / 2, D3DFMT_A32B32G32R32F, D3DFMT_UNKNOWN)))
    {
        return E_FAIL;
    }


    return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void CRenderTargetChangeComponent::Uninit(void)
{

}

//=============================================================================
// 描画処理
//=============================================================================
void CRenderTargetChangeComponent::Draw(void)
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

    //---------------------------------
    // UV座標の設定
    //---------------------------------

    // UVの開始位置
    float u0 = transform.UVPosition.x;
    float v0 = transform.UVPosition.y;

    // UVの終了位置
    float u1 = u0 + transform.UVSize.x;
    float v1 = v0 + transform.UVSize.y;

    // 頂点UVの設定
    vertex[0].tex = { u0, v0 };
    vertex[1].tex = { u1, v0 };
    vertex[2].tex = { u0, v1 };
    vertex[3].tex = { u1, v1 };

    //頂点バッファをアンロックする
    m_pVtxBuff->Unlock();

    m_pRenderTarget.Begin();
    m_pRenderTargetMulti.Begin();

    // メッシュの描画
    if (m_Shader)
    {
        auto effect = m_Shader->GetEffect();

        m_Shader->Apply(m_pOwner);

        UINT pass = 0;
        effect->Begin(&pass, 0);

        for (UINT i = 0; i < pass; i++)
        {
            effect->BeginPass(i);

            DrawPolygon();

            effect->EndPass();
        }

        effect->End();
    }
    //------------------------------------------------
    // シェーダなし
    //------------------------------------------------
    else
    {
        DrawPolygon();
    }
    m_pRenderTarget.End();
    m_pRenderTargetMulti.End();

}

//=============================================================================
// 描画処理(ポリゴン)
//=============================================================================
void CRenderTargetChangeComponent::DrawPolygon(void)
{

    // デバイスの取得
    auto device = CManager::Instance()->GetRenderer()->GetDevice();

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