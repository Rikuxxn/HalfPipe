//=============================================================================
//
// シーン処理 [Scene.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Scene.h"
#include "GameObject.h"
#include "Title.h"
#include "Connect.h"
#include "Game.h"
#include "Result.h"
#include "RenderComponent.h"
#include "LightComponent.h"
#include "TransformComponent.h"
#include "Manager.h"
#include "ModelInstancingRenderer.h"

//=============================================================================
// コンストラクタ
//=============================================================================
CScene::CScene()
{
	// 値のクリア
}
//=============================================================================
// デストラクタ
//=============================================================================
CScene::~CScene()
{

}
//=============================================================================
// 生成処理
//=============================================================================
std::unique_ptr<CScene>CScene::Create(Mode mode)
{
    CScene* scene = nullptr;

    switch (mode)
    {
    case Mode::Title:
        scene = new CTitle();
        break;

    case Mode::Connect:
        scene = new CConnect();
        break;

    case Mode::Game:
        scene = new CGame();
        break;

    case Mode::Result:
        scene = new CResult();
        break;
    }

    //if (scene)
    //{
    //    if (FAILED(scene->Init()))
    //    {
    //        delete scene;
    //        return nullptr;
    //    }
    //}

    return std::unique_ptr<CScene>(scene);
}

//=============================================================================
// オブジェクト生成
//=============================================================================
CGameObject* CScene::CreateObject(void)
{
    auto object = std::make_unique<CGameObject>();

    //---------------------------------
    // Scene設定
    //---------------------------------

    object->SetScene(this);

    CGameObject* ptr = object.get();

    m_Objects.push_back(std::move(object));

    return ptr;
}

//=============================================================================
// 終了処理
//=============================================================================
void CScene::Uninit(void)
{
    m_Objects.clear();
}

//=============================================================================
// 更新処理
//=============================================================================
void CScene::Update(void)
{
    for (auto& object : m_Objects)
    {
        if (!object)
        {
            continue;
        }

        if (object)
        {
            object->Update();
        }
    }

    m_Objects.erase(
        std::remove_if(
            m_Objects.begin(),
            m_Objects.end(),
            [](const std::unique_ptr<CGameObject>& object)
            {
                return !object || object->IsDestroy();
            }),
        m_Objects.end());

    // 遅延登録
    for (auto& obj : m_AddQueue)
    {
        m_Objects.push_back(std::move(obj));
    }

    m_AddQueue.clear();
}

//=============================================================================
// 描画処理
//=============================================================================
void CScene::Draw(void)
{
    auto modelRenderer = CManager::Instance()->GetModelInstancingRenderer();

    //---------------------------------
    // Layer順描画
    //---------------------------------

    for (int layer = 0; layer < (int)RenderLayer::Fade + 1; layer++)
    {
        for (auto& object : m_Objects)
        {
            if (!object)
            {
                continue;
            }

            // レンダーコンポーネントの取得
            auto render = object->GetComponent<CRenderComponent>();

            if (!render)
            {
                continue;
            }

            if ((int)render->GetLayer() != layer)
            {
                continue;
            }

            // オブジェクトの描画
            object->Draw();
        }

        //---------------------------------
        // Modelレイヤー終了
        //---------------------------------

        if (layer == (int)RenderLayer::Model)
        {
            modelRenderer->FlushMain();
        }
    }
}

//=============================================================================
// 描画処理(影用)
//=============================================================================
void CScene::DrawShadow(void)
{
    for (auto& object : m_Objects)
    {
        if (!object)
        {
            continue;
        }

        object->DrawShadow();
    }
}

//=============================================================================
// オブジェクトを探す処理
//=============================================================================
CGameObject* CScene::FindGameObject(const std::string& tag)
{
    for (auto& object : m_Objects)
    {
        if (object->CompareTag(tag))
        {
            return object.get();
        }
    }

    return nullptr;
}

//=============================================================================
// 複数のオブジェクトを探す処理
//=============================================================================
std::vector<CGameObject*> CScene::FindGameObjects(const std::string& tag)
{
    std::vector<CGameObject*> result;

    for (auto& object : m_Objects)
    {
        if (object->CompareTag(tag))
        {
            result.push_back(object.get());
        }
    }

    return result;
}

//=============================================================================
// ゲームオブジェクトの取得関数
//=============================================================================
const std::vector<std::unique_ptr<CGameObject>>& CScene::GetObjects(void) const
{
    return m_Objects;
}

//=============================================================================
// ディレクショナルライトの生成
//=============================================================================
CGameObject* CScene::CreateDirectionalLight(
    const D3DXVECTOR3& direction,
    const D3DXCOLOR& color)
{
    auto obj = CreateObject();

    auto light = obj->AddComponent<CLightComponent>();

    // ライトタイプの設定
    light->SetType(D3DLIGHT_DIRECTIONAL);

    // 方向の設定
    light->SetDirection(direction);

    // ディフューズの設定
    light->SetDiffuse(color);

    return obj;
}

//=============================================================================
// ポイントライトの生成
//=============================================================================
CGameObject* CScene::CreatePointLight(
    const D3DXVECTOR3& position,
    const D3DXCOLOR& color,
    float range,
    float attenuation1)
{
    auto obj = CreateObject();

    //---------------------------------
    // 位置
    //---------------------------------

    auto transform = obj->AddComponent<CTransformComponent>();

    transform->Position = position;

    //---------------------------------
    // ライト設定
    //---------------------------------

    auto light = obj->AddComponent<CLightComponent>();

    // ライトタイプの設定
    light->SetType(D3DLIGHT_POINT);

    // ディフューズの設定
    light->SetDiffuse(color);

    // 範囲の設定
    light->SetRange(range);

    light->SetAttenuation(
        0.0f,
        attenuation1,
        0.0f);

    return obj;
}

//=============================================================================
// スポットライトの生成
//=============================================================================
CGameObject* CScene::CreateSpotLight(
    const D3DXVECTOR3& position,
    const D3DXVECTOR3& direction,
    const D3DXCOLOR& color,
    float range,
    float theta,
    float phi)
{
    auto obj = CreateObject();

    //---------------------------------
    // 位置
    //---------------------------------

    auto transform = obj->AddComponent<CTransformComponent>();

    transform->Position = position;

    //---------------------------------
    // ライト設定
    //---------------------------------

    auto light = obj->AddComponent<CLightComponent>();

    // タイプの設定
    light->SetType(D3DLIGHT_SPOT);

    // ディフューズの設定
    light->SetDiffuse(color);

    // 方向の設定
    light->SetDirection(direction);

    // 範囲の設定
    light->SetRange(range);

    light->SetTheta(theta);

    light->SetPhi(phi);

    light->SetFalloff(1.0f);

    light->SetAttenuation(1.0f, 0.01f, 0.0f);

    return obj;
}