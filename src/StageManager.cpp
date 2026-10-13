//=============================================================================
//
// ステージマネージャー処理 [StageManager.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "StageManager.h"
#include "StageFactory.h"
#include "SceneManager.h"
#include "Scene.h"
#include "TransformComponent.h"
#include "Manager.h"
#include "StageEditor.h"
#include "Renderer.h"
#include "XModelRenderComponent.h"
#include "XModelResource.h"
#include "RigidBodyComponent.h"
#include "GhostObjectComponent.h"
#include "CollisionGroup.h"


//=============================================================================
// コンストラクタ
//=============================================================================
CStageManager::CStageManager()
{
    // 値のクリア

}

//=============================================================================
// デストラクタ
//=============================================================================
CStageManager::~CStageManager()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CStageManager::Init(void)
{
    if (!m_pStageEditor)
    {
        m_pStageEditor = std::make_unique<CStageEditor>();

        if (!m_pStageEditor)
        {
            return E_FAIL;
        }

#ifdef _DEBUG
        // 初回のギズモ生成
        m_pStageEditor->CreateGizmo();
#endif

        return m_pStageEditor->Init();
    }

#ifdef _DEBUG
    // ギズモ生成
    m_pStageEditor->CreateGizmo();
#endif
    return S_OK;
}

//=============================================================================
// リセット処理
//=============================================================================
void CStageManager::Reset(void)
{
    if (!m_pStageEditor)
    {
        return;
    }

    // ステージエディタのリセット処理
    m_pStageEditor->Uninit();
}

//=============================================================================
// 終了処理
//=============================================================================
void CStageManager::Uninit(void)
{
    m_pStageEditor.reset();
}

//=============================================================================
// 更新処理
//=============================================================================
void CStageManager::Update(void)
{
    // ステージエディタの更新処理
    m_pStageEditor->Update();
}


//=============================================================================
// オブジェクト生成
//=============================================================================
CStageObject* CStageManager::CreateObject(CStageObject::StageObjectType type, const D3DXVECTOR3& pos, bool enablePhysics)
{
    // シーンの取得
    auto scene = CManager::Instance()->GetSceneManager()->GetScene();

    // オブジェクトの生成
    auto obj = CStageFactory::Create(scene, type);

    if (!obj)
    {
        return nullptr;
    }

    // トランスフォームコンポーネントの追加
    auto transform = obj->AddComponent<CTransformComponent>();

    // 位置設定
    transform->Position = pos;

    // モデルレンダーコンポーネントの追加
    CXModelRenderComponent* model = nullptr;

    // モデルパスを取得して、モデルを生成
    const auto* info = m_pStageEditor->FindObjectInfo(type);

    if (info)
    {
        // タグの設定
        obj->SetTag(info->Name);
    }

    //-----------------------------------------
    // モデル描画
    //-----------------------------------------

    if (info && info->Category == "Model")
    {
        // モデルレンダーコンポーネントの追加
        model = obj->AddComponent<CXModelRenderComponent>();

        // 描画順の設定
        model->SetLayer(RenderLayer::Model);

        // モデルのロード
        model->Load(info->FilePath.c_str());
    }

    //-----------------------------------------
    // 剛体生成
    //-----------------------------------------

    if (enablePhysics && (info->Category == "Model" || info->Category == "Invisible"))
    {
        obj->SetGroupName("StageObject");

        // リジッドボディコンポーネントの追加
        auto rigid = obj->AddComponent<CRigidBodyComponent>();

        // Stage用のCollision設定
        rigid->SetCollisionFilter(
            COLLISION_GROUP_STAGE,
            COLLISION_GROUP_STAGE |
            COLLISION_GROUP_PLAYER |
            COLLISION_GROUP_GHOST |
            COLLISION_GROUP_ENEMY |
            COLLISION_GROUP_CAMERA
        );

        rigid->SetDynamic(false);

        auto size = VEC3_DEFAULT;

        if (model)
        {
            size = model->GetModelSize();
        }
        else
        {
            size = obj->GetColliderSize();
        }

        // コライダーの生成
        switch (obj->GetColliderShape())
        {
        case CStageObject::ColliderShape::Box:
        {
            rigid->CreateBox(size, 1.0f);
        }
        break;

        case CStageObject::ColliderShape::Sphere:
        {
            // モデルサイズから一番大きい辺を使う
            float radius = std::max({ size.x, size.y, size.z }) * 0.5f;
            rigid->CreateSphere(radius, 1.0f);
        }
        break;

        case CStageObject::ColliderShape::Capsule:
        {
            float radius = std::max(size.x, size.z) * 0.5f;
            float height = size.y - radius * 2.0f;

            if (height < 0.0f)
            {
                height = 0.0f;
            }

            rigid->CreateCapsule(radius, height, 1.0f);
        }
        break;
        case CStageObject::ColliderShape::TriangleMesh:
        {
            if (!model)
            {
                break;
            }

            auto resource = model->GetModel();

            if (resource)
            {
                auto triangleMesh = resource->CreateBulletTriangleMesh();

                rigid->CreateTriangleMesh(triangleMesh);
            }
        }
        break;
        }
    }

    return obj;
}

//=============================================================================
// 保存
//=============================================================================
void CStageManager::SaveToJson(const char* filename)
{
    json j;

    // シーンの取得
    auto scene = CManager::Instance()->GetSceneManager()->GetScene();

    // ステージオブジェクトの取得
    auto stages = scene->FindGameObjects<CStageObject>("StageObject");

    for (auto stage : stages)
    {
        json data;

        // 保存
        stage->SaveToJson(data);

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
void CStageManager::LoadFromJson(const char* filename)
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

    // ステージオブジェクトの取得
    auto stages = scene->FindGameObjects<CStageObject>("StageObject");

    for (auto stage : stages)
    {
        stage->Destroy();
    }

    //---------------------------------
    // 新規生成
    //---------------------------------

    for (const auto& data : j)
    {
        auto type = static_cast<CStageObject::StageObjectType>(data["type"]);

        if (auto obj = CreateObject(type, INIT_VEC3, true))
        {
            // ステージオブジェクト側の読み込み処理
            obj->LoadFromJson(data);

            // 初期化処理
            obj->Init();
        }
    }
}

//=============================================================================
// ステージエディタのサムネイルリリース通知
//=============================================================================
void CStageManager::ReleaseThumbnail(void)
{
    if (!m_pStageEditor)
    {
        return;
    }

    m_pStageEditor->ReleaseThumbnailRenderTarget();
}

//=============================================================================
// ステージエディタのサムネイルリセット通知
//=============================================================================
void CStageManager::ResetThumbnail(void)
{
    if (!m_pStageEditor)
    {
        return;
    }

    auto device = CManager::Instance()->GetRenderer()->GetDevice();

    m_pStageEditor->InitThumbnailRenderTarget(device);
    m_pStageEditor->GenerateThumbnails(); // キャッシュも再作成
}
