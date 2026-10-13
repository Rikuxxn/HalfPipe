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
            m_Layer);
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

    //char buf[256];

    //sprintf_s(buf, "Model : %s\n", filepath);
    //OutputDebugStringA(buf);

    //sprintf_s(buf, "Vertices : %u\n", m_Model->Mesh->GetNumVertices());
    //OutputDebugStringA(buf);

    //sprintf_s(buf, "Faces : %u\n", m_Model->Mesh->GetNumFaces());
    //OutputDebugStringA(buf);

    //sprintf_s(buf, "Subsets : %zu\n", m_Model->Materials.size());
    //OutputDebugStringA(buf);

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