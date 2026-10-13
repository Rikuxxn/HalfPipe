//=============================================================================
//
// 水処理 [Water.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Water.h"
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
#include "WaterShader.h"


//=============================================================================
// コンストラクタ
//=============================================================================
CWater::CWater()
{

}

//=============================================================================
// デストラクタ
//=============================================================================
CWater::~CWater()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CWater::Init(void)
{
    //---------------------------------
    // 平面メッシュ生成
    //---------------------------------

    // シーンの取得
    auto scene = CManager::Instance()->GetSceneManager()->GetScene();

    // 平面メッシュオブジェクトの生成
    auto plane = scene->CreateObject();

    plane->SetTag("WaterField");

    auto meshTransform = plane->AddComponent<CTransformComponent>();

    meshTransform->Position = { 0.0f, 0.0f, 0.0f };

    auto renderer = plane->AddComponent<CMeshRenderComponent>();

    // 描画順の設定
    renderer->SetLayer(RenderLayer::Mesh);

    auto mesh = std::make_shared<CMeshResource>();

    mesh->CreatePlane(1000.0f, 1000.0f, 100, 100);

    // ポインタ設定
    renderer->SetMesh(mesh);

    // シェーダーの設定
    renderer->SetShader(
        CManager::Instance()->GetShaderManager()->Load<CWaterShader>(
            "data/Shader/Water/Water.hlsl"));

    return S_OK;
}

//=============================================================================
// 描画処理
//=============================================================================
void CWater::Draw(void)
{

}