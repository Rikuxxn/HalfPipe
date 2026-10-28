//=============================================================================
//
// Xモデルレンダーコンポーネント処理 [XModelRenderComponent.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "XModelRenderComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "Manager.h"
#include "XModelResource.h"
#include "TextureResource.h"
#include "Renderer.h"
#include "ModelManager.h"
#include "ModelInstancingRenderer.h"
#include "TextureManager.h"


//=============================================================================
// 描画処理
//=============================================================================
void CXModelRenderComponent::Draw(void)
{
    if (!m_Model)
    {
        return;
    }

    if (!m_Visible)
    {
        return;
    }

    // トランスフォームコンポーネントの取得
    auto transform = m_pOwner->GetComponent<CTransformComponent>();

    if (!transform)
    {
        return;
    }

    // インスタンシング
    CManager::Instance()
        ->GetModelInstancingRenderer()
        ->AddInstance(
            m_Model.get(),
            transform->GetWorldMatrix(),
            m_Layer,
            m_FaceTextures);
}

//=============================================================================
// モデルロード
//=============================================================================
HRESULT CXModelRenderComponent::Load(const char* filepath)
{
    m_Model = CModelManager::GetInstance().Load(filepath);

    if (!m_Model)
    {
        return E_FAIL;
    }

    return S_OK;
}

//=============================================================================
// モデルサイズの取得
//=============================================================================
D3DXVECTOR3 CXModelRenderComponent::GetModelSize(void) const
{
    if (!m_Model)
    {
        return { 1.0f, 1.0f, 1.0f };
    }

    return m_Model->GetSize();
}

//=============================================================================
// モデルの取得
//=============================================================================
std::shared_ptr<CXModelResource> CXModelRenderComponent::GetModel(void) const
{
    return m_Model;
}

//=============================================================================
// 面のテクスチャを設定
//=============================================================================
void CXModelRenderComponent::SetFaceTexture(int faceIndex, const std::string& path)
{
    if (faceIndex < 0)
    {
        return;
    }

    // テクスチャをロード
    auto texture = CManager::Instance()->GetTextureManager()->Load(path);

    if (!texture)
    {
        return;
    }

    // 指定した面のテクスチャとして登録
    m_FaceTextures[faceIndex] = texture;
}

//=============================================================================
// 面全てにテクスチャを設定
//=============================================================================
void CXModelRenderComponent::SetAllFaceTexture(const std::string& path)
{
    // モデルが読み込まれているか確認
    if (!m_Model)
    {
        return;
    }

    // テクスチャをロード
    auto texture = CManager::Instance()->GetTextureManager()->Load(path);

    if (!texture)
    {
        return;
    }

    // すべての面に同じテクスチャを登録
    m_FaceTextures.clear();

    for (int i = 0; i < m_Model->NumFaces; i++)
    {
        // すべての面のテクスチャとして登録
        m_FaceTextures[static_cast<int>(i)] = texture;
    }
}

//=============================================================================
// 面のテクスチャを削除
//=============================================================================
void CXModelRenderComponent::ClearFaceTexture(int faceIndex)
{
    m_FaceTextures.erase(faceIndex);
}

//=============================================================================
// すべての面のテクスチャを削除
//=============================================================================
void CXModelRenderComponent::ClearFaceTextures(void)
{
    m_FaceTextures.clear();
}

//=============================================================================
// 面のテクスチャを取得
//=============================================================================
const std::unordered_map<int, std::shared_ptr<CTextureResource>>&
CXModelRenderComponent::GetFaceTextures(void) const
{
    return m_FaceTextures;
}