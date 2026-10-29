//=============================================================================
//
// 3Dレンダーコンポーネント処理 [3DSpriteComponent.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "3DSpriteRenderComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "Manager.h"
#include "TextureResource.h"
#include "Renderer.h"
#include "MathConstants.h"
#include "ColorConstants.h"

// 名前空間の使用
using namespace MathConst;
using namespace ColorConst;


//=============================================================================
// コンストラクタ
//=============================================================================
C3DSpriteRenderComponent::C3DSpriteRenderComponent() :
    m_pVtxBuff      (nullptr),
    m_Color         (ColorConst::DX::WHITE),
    m_Width         (100.0f),
    m_Height        (100.0f),
    m_isVisible     (true)
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT C3DSpriteRenderComponent::Init(void)
{
    // デバイスの取得
    auto device = CManager::Instance()->GetRenderer()->GetDevice();

    device->CreateVertexBuffer(
        sizeof(VERTEX_3D) * MathConst::VERTEX,
        0,
        FVF_VERTEX_3D,
        D3DPOOL_MANAGED,
        m_pVtxBuff.GetAddressOf(),
        nullptr);

    return S_OK;
}

//=============================================================================
// 描画処理
//=============================================================================
void C3DSpriteRenderComponent::Draw(void)
{
    if (!m_isVisible)
    {
        return;
    }

    // トランスフォームコンポーネントの取得
    auto transform = m_pOwner->GetComponent<CTransformComponent>();

    if (!transform)
    {
        return;
    }

    // デバイスの取得
    auto device = CManager::Instance()->GetRenderer()->GetDevice();

    VERTEX_3D* vertex = nullptr;

    // 頂点バッファをロックし、頂点情報へのポインタを取得
    m_pVtxBuff->Lock(0, 0, (void**)&vertex, 0);

     // 板ポリゴンの半分のサイズ
    float halfW = m_Width * MathConst::HALF;
    float halfH = m_Height * MathConst::HALF;

    // XY平面
    //vertex[0].pos = { -halfW, halfH, 0.0f };    // 左上
    //vertex[1].pos = { halfW, halfH, 0.0f };     // 右上
    //vertex[2].pos = { -halfW, -halfH, 0.0f };   // 左下
    //vertex[3].pos = { halfW, -halfH, 0.0f };    // 右下

    // XZ平面
    vertex[0].pos = { -halfW, 0.0f, halfH };    // 左上
    vertex[1].pos = { halfW, 0.0f, halfH };     // 右上
    vertex[2].pos = { -halfW, 0.0f, -halfH };   // 左下
    vertex[3].pos = { halfW, 0.0f, -halfH };    // 右下

    // 法線・色・UVの設定
    for (int i = 0; i < MathConst::VERTEX; i++)
    {
        vertex[i].nor = { 0.0f, -1.0f, 0.0f };
        vertex[i].col = m_Color;
    }

    vertex[0].tex = { 0.0f, 0.0f };
    vertex[1].tex = { 1.0f, 0.0f };
    vertex[2].tex = { 0.0f, 1.0f };
    vertex[3].tex = { 1.0f, 1.0f };

    //頂点バッファをアンロックする
    m_pVtxBuff->Unlock();

    // 計算用マトリックス
    D3DXMATRIX mtxRot, mtxTrans;

    // ワールド行列の取得
    D3DXMATRIX matWorld = transform->GetWorldMatrix();

    //-------------------------------------------------------------------------
    // αテスト
    //-------------------------------------------------------------------------
    device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    device->SetRenderState(D3DRS_ALPHAREF, 0);
    device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

    // カリング設定を無効化
    device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    // ライティングを無効化
    device->SetRenderState(D3DRS_LIGHTING, FALSE);

    // ワールドマトリックスを設定
    device->SetTransform(D3DTS_WORLD, &matWorld);

    // テクスチャの設定
    device->SetTexture(0, m_TextureResource ? m_TextureResource->Texture.Get() : nullptr);

    // 頂点バッファをデータストリームに設定
    device->SetStreamSource(0, m_pVtxBuff.Get(), 0, sizeof(VERTEX_3D));

    // 頂点フォーマットの設定
    device->SetFVF(FVF_VERTEX_3D);

    // ポリゴンの描画
    device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

    // カリング設定を有効化
    device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

    // ライティングを有効化
    device->SetRenderState(D3DRS_LIGHTING, TRUE);

    //-------------------------------------------------------------------------
    // αテスト無効
    //-------------------------------------------------------------------------
    device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
}