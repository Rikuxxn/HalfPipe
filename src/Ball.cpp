//=============================================================================
//
// ボール処理 [Ball.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Ball.h"
#include "TransformComponent.h"
#include "XModelRenderComponent.h"
#include "RigidBodyComponent.h"
#include "CollisionGroup.h"
#include "Manager.h"
#include "TimeSystem.h"


//=============================================================================
// コンストラクタ
//=============================================================================
CBall::CBall() : 
    m_LifeTime      (3.0f),     // ボールの寿命（秒）
    m_ElapsedTime   (0.0f),     // 経過時間
	m_IsThrown      (false)     // ボールを投げたか
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CBall::Init(void)
{
    // トランスフォーム
    auto transform = AddComponent<CTransformComponent>();

    if (!transform)
    {
        return E_FAIL;
    }

    // モデル描画
    auto model = AddComponent<CXModelRenderComponent>();

    if (!model)
    {
        return E_FAIL;
    }

    // 描画順の設定
    model->SetLayer(RenderLayer::Model);

    // ボールモデルの読み込み
    if (FAILED(model->Load("data/MODELS/tomas.x")))
    {
        return E_FAIL;
    }

    // タグ
    SetTag("Ball");

    // グループ
    SetGroupName("Balls");

    // リジッドボディコンポーネントの追加
    auto rigid = AddComponent<CRigidBodyComponent>();

    if (!rigid)
    {
        return E_FAIL;
    }

    // 動的剛体として生成
    rigid->SetDynamic(true);

    // 衝突フィルター
    rigid->SetCollisionFilter(
        COLLISION_GROUP_PLAYER,
        COLLISION_GROUP_STAGE |
        COLLISION_GROUP_PLAYER |
        COLLISION_GROUP_ENEMY
    );

    auto size = VEC3_DEFAULT;

    if (model)
    {
        size = model->GetModelSize();
    }

    // モデルサイズから一番大きい辺を使う
    float radius = std::max({ size.x, size.y, size.z }) * 0.5f;

    // スフィアコライダーの生成
    rigid->CreateSphere(radius, 1.0f);

    m_IsThrown = false;

	return S_OK;
}

//=============================================================================
// 更新処理
//=============================================================================
void CBall::Update(void)
{
    // ゲームオブジェクトの更新
    CGameObject::Update();

    // 投擲されていない場合は何もしない
    if (!m_IsThrown)
    {
        return;
    }

    // デルタタイムの取得
    float deltaTime = CManager::Instance()->GetTimeSystem()->GetDeltaTime();

    // 経過時間を加算
    m_ElapsedTime += deltaTime;

    // 寿命を超えたら削除
    if (m_ElapsedTime >= m_LifeTime)
    {
        // ボール削除
        this->Destroy();
    }
}

//=============================================================================
// 投擲処理
//=============================================================================
void CBall::Throw(const D3DXVECTOR3& velocity)
{
    // リジッドボディコンポーネントの取得
    auto rigid = GetComponent<CRigidBodyComponent>();

    if (!rigid)
    {
        return;
    }

    // 物理速度の設定
    rigid->SetPhysicsVelocity(velocity);

    // タイマー初期化
    m_ElapsedTime = 0.0f;

    m_IsThrown = true;
}
