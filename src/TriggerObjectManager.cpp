//=============================================================================
//
// トリガーオブジェクトマネージャー処理 [TriggerObjectManager.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "TriggerObjectManager.h"
#include "TriggerFactory.h"
#include "SceneManager.h"
#include "Scene.h"
#include "TransformComponent.h"
#include "Manager.h"
#include "Renderer.h"
#include "GhostObjectComponent.h"
#include "TriggerObjectEditor.h"
#include "CollisionGroup.h"


//=============================================================================
// コンストラクタ
//=============================================================================
CTriggerObjectManager::CTriggerObjectManager()
{
    // 値のクリア

}

//=============================================================================
// デストラクタ
//=============================================================================
CTriggerObjectManager::~CTriggerObjectManager()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CTriggerObjectManager::Init(void)
{
    if (!m_pTriggerObjectEditor)
    {
        // トリガーオブジェクトエディタの生成
        m_pTriggerObjectEditor = std::make_unique<CTriggerObjectEditor>();

        if (!m_pTriggerObjectEditor)
        {
            return E_FAIL;
        }

#ifdef _DEBUG
        // 初回のギズモ生成
        m_pTriggerObjectEditor->CreateGizmo();
#endif

        return m_pTriggerObjectEditor->Init();
    }

#ifdef _DEBUG
    // ギズモ生成
    m_pTriggerObjectEditor->CreateGizmo();
#endif

    return S_OK;
}

//=============================================================================
// リセット処理
//=============================================================================
void CTriggerObjectManager::Reset(void)
{
    if (!m_pTriggerObjectEditor)
    {
        return;
    }

    // トリガーリストのクリア
    m_TriggerObjects.clear();

    // トリガーオブジェクトエディタのリセット処理
    m_pTriggerObjectEditor->Uninit();
}

//=============================================================================
// 終了処理
//=============================================================================
void CTriggerObjectManager::Uninit(void)
{
    m_pTriggerObjectEditor.reset();
    m_TriggerObjects.clear();
}

//=============================================================================
// 更新処理
//=============================================================================
void CTriggerObjectManager::Update(void)
{
    // トリガーオブジェクトエディタの更新処理
    m_pTriggerObjectEditor->Update();
}

//=============================================================================
// オブジェクト生成
//=============================================================================
CTriggerObject* CTriggerObjectManager::CreateTrigger(CTriggerObject::TriggerObjectType type, const D3DXVECTOR3& pos)
{
    // シーンの取得
    auto scene = CManager::Instance()->GetSceneManager()->GetScene();

    // オブジェクトの生成
    auto obj = CTriggerFactory::Create(scene, type);

    if (!obj)
    {
        return nullptr;
    }

    // トランスフォームコンポーネントの追加
    auto transform = obj->AddComponent<CTransformComponent>();

    // 位置設定
    transform->Position = pos;

    obj->SetGroupName("TriggerObject");

    //-----------------------------------------
    // ゴーストオブジェクトの生成
    //-----------------------------------------

    // ゴーストオブジェクトコンポーネントの追加
    auto ghost = obj->AddComponent<CGhostObjectComponent>();

    // ゴーストの衝突フィルター
        ghost->SetCollisionFilter(
            COLLISION_GROUP_GHOST,
            COLLISION_GROUP_PLAYER |
            COLLISION_GROUP_STAGE |
            COLLISION_GROUP_ENEMY   
        );

    D3DXVECTOR3 size = obj->GetTriggerSize();

    switch (obj->GetTriggerShape())
    {
    case CTriggerObject::TriggerShape::Box:
    {
        ghost->CreateBox(size);
        break;

        // SphereやCapsuleを後で追加
    }
    }

    //-----------------------------------------
    // TriggerObject管理リストに追加
    //-----------------------------------------
    m_TriggerObjects.push_back(obj);

    return obj;
}

//=============================================================================
// 保存
//=============================================================================
void CTriggerObjectManager::SaveToJson(const char* filename)
{
    json j;

    // シーンの取得
    auto scene = CManager::Instance()->GetSceneManager()->GetScene();

    // トリガーオブジェクトの取得
    auto triggers = scene->FindGameObjects<CTriggerObject>("TriggerObject");

    for (auto trigger : triggers)
    {
        json data;

        // 保存
        trigger->SaveToJson(data);

        j.push_back(data);
    }

    std::ofstream file(filename);

    if (!file.is_open())
    {
        return;
    }

    file << std::setw(4) << j;

    file.close();
}

//=============================================================================
// 読み込み
//=============================================================================
void CTriggerObjectManager::LoadFromJson(const char* filename)
{
    std::ifstream file(filename);

    if (!file.is_open())
    {
        return;
    }

    json j;

    file >> j;

    file.close();

    //---------------------------------
    // シーン内の古いオブジェクト削除
    //---------------------------------

    // シーンの取得
    auto scene = CManager::Instance()->GetSceneManager()->GetScene();

    // トリガーオブジェクトの取得
    auto triggers = scene->FindGameObjects<CTriggerObject>("TriggerObject");

    for (auto trigger : triggers)
    {
        trigger->Destroy();
    }

    //---------------------------------
    // 新規生成
    //---------------------------------

    for (const auto& data : j)
    {
        auto type = static_cast<CTriggerObject::TriggerObjectType>(data["type"]);

        if (auto obj = CreateTrigger(type, INIT_VEC3))
        {
            // トリガーオブジェクト側の読み込み処理
            obj->LoadFromJson(data);

            // 初期化処理
            obj->Init();
        }
    }
}

//=============================================================================
// オブジェクト削除
//=============================================================================
void CTriggerObjectManager::RemoveTrigger(CTriggerObject* object)
{
    if (!object)
    {
        return;
    }

    auto it = std::find(m_TriggerObjects.begin(), m_TriggerObjects.end(), object);

    if (it != m_TriggerObjects.end())
    {
        m_TriggerObjects.erase(it);
    }

    object->Destroy();
}