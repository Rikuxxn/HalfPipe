//=============================================================================
//
// メッシュレンダーコンポーネント処理 [MeshRenderComponent.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "MeshRenderComponent.h"
#include "Manager.h"
#include "Renderer.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "MeshResource.h"
#include "TextureResource.h"
#include "TextureManager.h"
#include "MeshBase.h"
#include "ShaderResource.h"


//=============================================================================
// メッシュ描画処理
//=============================================================================
void CMeshRenderComponent::Draw(void)
{
    // トランスフォームコンポーネントの取得
    auto transform = m_pOwner->GetComponent<CTransformComponent>();

    if (!transform)
    {
        return;
    }

    // デバイスの取得
    auto device = CManager::Instance()->GetRenderer()->GetDevice();

    // ワールド行列の取得
    D3DXMATRIX matWorld = transform->GetWorldMatrix();

    // ワールドマトリックスの設定
    device->SetTransform(D3DTS_WORLD, &matWorld);

    // テクスチャの設定
    device->SetTexture(0, m_TextureResource ? m_TextureResource->Texture.Get() : nullptr);

    if (!m_Mesh)
    {
        return;
    }

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

            m_Mesh->Draw();

            effect->EndPass();
        }

        effect->End();
    }
    //------------------------------------------------
    // シェーダなし
    //------------------------------------------------
    else
    {
        m_Mesh->Draw();
    }
}

//=============================================================================
// テクスチャの設定処理
//=============================================================================
void CMeshRenderComponent::SetTexture(const std::string& path)
{
    m_TextureResource = CManager::Instance()->GetTextureManager()->Load(path);
}
