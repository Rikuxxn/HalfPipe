//=============================================================================
//
// 空処理 [Sky.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Sky.h"
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
#include "SkyShader.h"


//=============================================================================
// コンストラクタ
//=============================================================================
CSky::CSky()
{

}

//=============================================================================
// デストラクタ
//=============================================================================
CSky::~CSky()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CSky::Init(void)
{
    //---------------------------------
    // 球メッシュ生成
    //---------------------------------

    // シーンの取得
    auto scene = CManager::Instance()->GetSceneManager()->GetScene();

    // メッシュスフィアオブジェクトの生成
    auto sun = scene->CreateObject();

    sun->SetTag("MeshSphere");

    auto meshTransform = sun->AddComponent<CTransformComponent>();

    meshTransform->Position = { 0.0f, 0.0f, 0.0f };

    auto renderer = sun->AddComponent<CMeshRenderComponent>();

    // 描画順の設定
    renderer->SetLayer(RenderLayer::Mesh);

    auto mesh = std::make_shared<CMeshResource>();

    mesh->CreateSphere(
        3000.0f,   // 半径
        64,        // Slice
        32);       // Stack

    // ポインタ設定
    renderer->SetMesh(mesh);

    // シェーダーの設定
    renderer->SetShader(
        CManager::Instance()->GetShaderManager()->Load<CSkyShader>(
            "data/Shader/Sky/Sky.hlsl"));

    return S_OK;
}

//=============================================================================
// 描画処理
//=============================================================================
void CSky::Draw(void)
{

}