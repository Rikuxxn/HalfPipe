//=============================================================================
//
// VDブラー処理 [VD.cpp]
// Author : SOICHIRO SASAKI
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "VD.h"
#include "VDShader.h"
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
#include "UIComponent.h"
#include "RenderTargetChangeComponent.h"

//=============================================================================
// コンストラクタ
//=============================================================================
CVD::CVD() :
    m_GameObject(nullptr)
{

}

//=============================================================================
// デストラクタ
//=============================================================================
CVD::~CVD()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CVD::Init(void)
{
    //---------------------------------
    // 球メッシュ生成
    //---------------------------------

    // シーンの取得
    auto scene = CManager::Instance()->GetSceneManager()->GetScene();

    // メッシュスフィアオブジェクトの生成
    m_GameObject = scene->CreateObject();

    m_GameObject->SetTag("VD");

    auto SpriteRenderer = m_GameObject->AddComponent<CRenderTargetChangeComponent>();
    auto UI = m_GameObject->AddComponent<CUIComponent>();

    auto& uiTransform = UI->GetTransform();
    uiTransform.Position = { 0.5f,0.5f };
    uiTransform.Scale = { 0.01f,0.01f };

    // 描画順の設定
    SpriteRenderer->SetLayer(RenderLayer::UI);
    SpriteRenderer->SetShader(CManager::Instance()->GetShaderManager()->Load<CVDShader>(
        "data/Shader/Hex/VDShader.hlsl"));

    return S_OK;
}

//=============================================================================
// 描画処理
//=============================================================================
void CVD::Draw(void)
{

}