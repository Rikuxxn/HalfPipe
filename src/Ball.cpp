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
#include "MathUtility.h"


//=============================================================================
// コンストラクタ
//=============================================================================
CBall::CBall() : 
    m_LifeTime      (3.0f),     // ボールの寿命（秒）
    m_ElapsedTime   (0.0f),     // 経過時間
	m_IsThrown      (false),    // ボールを投げたか
    m_HasScored     (false),
    m_Sprain        (false)
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
        COLLISION_GROUP_BALL,
        COLLISION_GROUP_STAGE |
        COLLISION_GROUP_PLAYER |
        COLLISION_GROUP_ENEMY |
        COLLISION_GROUP_GHOST
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
    // コンポーネントの取得
    auto transform = GetComponent<CTransformComponent>();
    auto rigid = GetComponent<CRigidBodyComponent>();

    if (m_Sprain)
    {
        transform->Position = MathUtil::Spline(m_SprainStart, m_PlayerPosition, m_AimPosition, m_SprainEnd, m_ElapsedTime);
        rigid->Transform();
    }

    // デルタタイムの取得
    float deltaTime = CManager::Instance()->GetTimeSystem()->GetDeltaTime();

    // 経過時間を加算
    m_ElapsedTime += deltaTime;

    if (m_ElapsedTime >= 1.0f)
    {
        m_Sprain = false;
    }

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
void CBall::Throw(const D3DXVECTOR3& AimPos, const D3DXVECTOR3& PlayerPos)
{
    // リジッドボディコンポーネントの取得
    auto rigid = GetComponent<CRigidBodyComponent>();

    if (!rigid)
    {
        return;
    }
    m_PlayerPosition = PlayerPos;
    m_AimPosition = AimPos;
    D3DXVECTOR3 dirA = m_PlayerPosition - m_AimPosition;
    D3DXVECTOR3 dirB = m_AimPosition - m_PlayerPosition;
    m_SprainStart = D3DXVECTOR3(m_PlayerPosition.x + dirA.x * 5.0f, m_PlayerPosition.y, m_PlayerPosition.z + dirA.z * 5.0f);
    m_SprainEnd = D3DXVECTOR3(AimPos.x, AimPos.y + dirB.y*5.0f, AimPos.z);

    // 物理速度の設定
    rigid->SetPhysicsVelocity(dirA);

    // タイマー初期化
    m_ElapsedTime = 0.0f;

    m_IsThrown = true;
    m_Sprain = true;
}
