//=============================================================================
//
// 空処理 [Sky.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "DepthOfField.h"
#include "DepthOfFieldShader.h"
#include "Manager.h"
#include "Renderer.h"
#include "Camera.h"
#include "MeshResource.h"
#include "MeshRenderComponent.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "ShaderManager.h"
#include "ShaderResource.h"
#include "SpriteRenderComponent.h"

//=============================================================================
// コンストラクタ
//=============================================================================
CDepthOfField::CDepthOfField()
{

}

//=============================================================================
// デストラクタ
//=============================================================================
CDepthOfField::~CDepthOfField()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CDepthOfField::Init(void)
{
    //---------------------------------
    // 球メッシュ生成
    //---------------------------------

    // シーンの取得
    auto scene = CManager::Instance()->GetSceneManager()->GetScene();

    // メッシュスフィアオブジェクトの生成
    m_GameObject = scene->CreateObject();

    m_GameObject->SetTag("DoF");

    auto meshTransform = m_GameObject->AddComponent<CTransformComponent>();

    meshTransform->Position = { 0.0f, 0.0f, 0.0f };

    auto SpriteRenderer = m_GameObject->AddComponent<CSpriteRenderComponent>();

    // 描画順の設定
    SpriteRenderer->SetLayer(RenderLayer::Mesh);

    CManager::Instance()->GetShaderManager()->Load<CDoFShader>(
        "data/Shader/Sky/Sky.hlsl");



    return S_OK;
}

//=============================================================================
// 描画処理
//=============================================================================
void CDepthOfField::Draw(void)
{

}