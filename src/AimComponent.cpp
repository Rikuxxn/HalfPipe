//=============================================================================
//
// エイムコンポーネント処理 [AimComponent.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "AimComponent.h"
#include "AimTargetComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "Scene.h"
#include "3DSpriteRenderComponent.h"
#include "MathConstants.h"
#include "Manager.h"
#include "SceneManager.h"
#include "XModelRenderComponent.h"
#include "MathUtility.h"


// 名前空間の使用
using namespace MathConst;


//=============================================================================
// コンストラクタ
//=============================================================================
CAimComponent::CAimComponent() :
    m_pTarget       (nullptr),
    m_pAimMarker    (nullptr),
    m_AimPosition   (INIT_VEC3),
    m_TargetCenter  (INIT_VEC3),
    m_TargetRotation({ 0, 0, 0, 1 }),
    m_AimRadius     (100.0f),
    m_AimSpeed      (500.0f),
    m_IsTargetSet   (false)
{

}

//=============================================================================
// デストラクタ
//=============================================================================
CAimComponent::~CAimComponent()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CAimComponent::Init(void)
{
    // エイム表示の生成
    CreateAimMarker();
    // 予測線の生成
    CreatePredictionLine();

    return S_OK;
}

//=============================================================================
// 更新処理
//=============================================================================
void CAimComponent::Update(void)
{
    CAimTargetComponent* nearest = FindNearestTarget();

    // 対象が変わったときだけ設定
    if (nearest != m_pTarget)
    {
        SetTarget(nearest);
    }

    UpdateAimMarker();
    UpdatePredictionLine();
}

//=============================================================================
// エイム表示の更新
//=============================================================================
void CAimComponent::UpdateAimMarker(void)
{
    if (!m_pAimMarker)
    {
        return;
    }

    auto transform = m_pAimMarker->GetComponent<CTransformComponent>();

    if (!transform)
    {
        return;
    }

    // エイム位置に追従
    transform->Position = m_AimPosition;
}

//=============================================================================
// ターゲットの設定
//=============================================================================
void CAimComponent::SetTarget(CAimTargetComponent* target)
{
    if (m_pTarget == target)
    {
        return;
    }

    m_pTarget = target;

    if (!m_pTarget)
    {
        return;
    }

    m_TargetCenter = m_pTarget->GetAimCenter();
    m_TargetRotation = m_pTarget->GetAimRotation();
    m_AimRadius = m_pTarget->GetAimRadius();

    // ランダムな位置に照準を配置
    SetRandomAimPosition();

    m_IsTargetSet = true;
}

//=============================================================================
// エイム位置の設定
//=============================================================================
void CAimComponent::SetAimPosition(const D3DXVECTOR3& position)
{
    m_AimPosition = position;

    ClampAimPosition();
}

//=============================================================================
// エイム位置をランダムに設定
//=============================================================================
void CAimComponent::SetRandomAimPosition(void)
{
    // ターゲットの円の内部からランダムな位置を選択
    float randomAngle = (static_cast<float>(rand()) / RAND_MAX) * D3DX_PI * F_DOUBLE;

    // 円の内部に均等に分布させるため、半径に平方根を使用
    float randomRadius = m_AimRadius * sqrtf(static_cast<float>(rand()) / RAND_MAX);

    // ターゲットのローカル座標でランダム位置を計算
    D3DXVECTOR3 localOffset(
        cosf(randomAngle) * randomRadius,
        0.0f,
        sinf(randomAngle) * randomRadius
    );

    // ターゲットの回転行列を作成
    D3DXMATRIX rotationMatrix;

    D3DXMatrixRotationQuaternion(&rotationMatrix, &m_TargetRotation);

    // ローカル座標からワールド座標へ変換
    D3DXVECTOR3 worldOffset;

    D3DXVec3TransformNormal(&worldOffset, &localOffset, &rotationMatrix);

    // ターゲット中心からランダム位置へ配置
    m_AimPosition = m_TargetCenter + worldOffset;
}

//=============================================================================
// 照準移動
//=============================================================================
void CAimComponent::MoveAim(const D3DXVECTOR3& moveDirection, float deltaTime)
{
    if (!m_IsTargetSet)
    {
        return;
    }

    // Basket の回転行列
    D3DXMATRIX rotationMatrix;

    D3DXMatrixRotationQuaternion(&rotationMatrix,&m_TargetRotation);

    // ローカル座標での移動方向をワールド座標へ変換
    D3DXVECTOR3 worldMoveDirection;

    D3DXVec3TransformNormal(&worldMoveDirection, &moveDirection, &rotationMatrix);

    // 移動
    m_AimPosition += worldMoveDirection * m_AimSpeed * deltaTime;

    // 移動可能範囲を制限
    ClampAimPosition();
}

//=============================================================================
// エイムの表示
//=============================================================================
void CAimComponent::VisibleAim(void)
{
    // 3D板ポリゴン取得
    auto sprite = m_pAimMarker->GetComponent<C3DSpriteRenderComponent>();

    if (sprite)
    {
        sprite->SetIsVisible(true);
    }

    // 予測線
    for (int nCount = 0; nCount < m_pPointObj.size(); nCount++)
    {
        auto pointObj = m_pPointObj[nCount];

        if (!pointObj)
        {
            return;
        }

        // モデルコンポーネントの取得
        auto model = pointObj->GetComponent<CXModelRenderComponent>();

        if (!model)
        {
            return;
        }
        model->SetVisible(true);
    }

}

//=============================================================================
// エイムの非表示
//=============================================================================
void CAimComponent::InvisibleAim(void)
{
    // 3D板ポリゴン取得
    auto sprite = m_pAimMarker->GetComponent<C3DSpriteRenderComponent>();

    if (sprite)
    {
        sprite->SetIsVisible(false);
    }

    // 予測線
    for (int nCount = 0; nCount < m_pPointObj.size(); nCount++)
    {
        auto pointObj = m_pPointObj[nCount];

        if (!pointObj)
        {
            return;
        }

        // モデルコンポーネントの取得
        auto model = pointObj->GetComponent<CXModelRenderComponent>();

        if (!model)
        {
            return;
        }
        model->SetVisible(false);
    }

}

//=============================================================================
// エイム表示の生成
//=============================================================================
void CAimComponent::CreateAimMarker(void)
{
    if (!m_pOwner)
    {
        return;
    }

    CScene* scene = m_pOwner->GetScene();

    if (!scene)
    {
        return;
    }

    // エイム表示用オブジェクトを生成
    m_pAimMarker = scene->CreateObject<CGameObject>();
    
    if (!m_pAimMarker)
    {
        return;
    }

    m_pAimMarker->SetTag("AimMarker");

    // Transform
    auto transform = m_pAimMarker->AddComponent<CTransformComponent>();

    // 3D板ポリゴン描画
    auto sprite = m_pAimMarker->AddComponent<C3DSpriteRenderComponent>();

    if (sprite)
    {
        sprite->SetSize(80.0f, 80.0f);
        sprite->SetColor(D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));
        sprite->SetIsVisible(true);
    }

    // 初期位置
    if (transform)
    {
        transform->Position = m_AimPosition;
    }
}

//=============================================================================
// エイム範囲制限
//=============================================================================
void CAimComponent::ClampAimPosition(void)
{
    // ワールド座標からターゲットのローカル座標へ変換
    D3DXQUATERNION inverseRotation;
    D3DXQuaternionInverse(&inverseRotation, &m_TargetRotation);

    D3DXVECTOR3 offset = m_AimPosition - m_TargetCenter;

    // 逆回転から回転行列を作成
    D3DXMATRIX inverseRotationMatrix;
    D3DXMatrixRotationQuaternion(&inverseRotationMatrix, &inverseRotation);

    // オフセットをローカル座標へ変換
    D3DXVECTOR3 localOffset;
    D3DXVec3TransformNormal(&localOffset, &offset, &inverseRotationMatrix);

    float distance = D3DXVec3Length(&localOffset);

    if (distance > m_AimRadius && distance > 0.0001f)
    {
        D3DXVec3Normalize(&localOffset, &localOffset);
        localOffset *= m_AimRadius;
    }

    // ターゲットの回転行列を作成
    D3DXMATRIX rotationMatrix;
    D3DXMatrixRotationQuaternion(&rotationMatrix, &m_TargetRotation);

    // ローカル座標からワールド座標へ戻す
    D3DXVECTOR3 worldOffset;
    D3DXVec3TransformNormal(&worldOffset, &localOffset, &rotationMatrix);

    m_AimPosition = m_TargetCenter + worldOffset;
}

//=============================================================================
// 最も近い照準ターゲットを取得
//=============================================================================
CAimTargetComponent* CAimComponent::FindNearestTarget(void)
{
    // オーナーの取得
    if (!m_pOwner)
    {
        return nullptr;
    }

    // 自分のTransformを取得
    auto transform = m_pOwner->GetComponent<CTransformComponent>();
    if (!transform)
    {
        return nullptr;
    }

    // シーンを取得
    CScene* scene = m_pOwner->GetScene();
    if (!scene)
    {
        return nullptr;
    }

    CAimTargetComponent* nearestTarget = nullptr;
    float nearestDistanceSq = FLT_MAX;

    // シーン内の全オブジェクトを取得
    for (CGameObject* object : scene->GetAllObjects())
    {
        if (!object)
        {
            continue;
        }

        // 照準ターゲットを持っているか確認
        auto target = object->GetComponent<CAimTargetComponent>();

        if (!target)
        {
            continue;
        }

        // ターゲットの中心位置を取得
        D3DXVECTOR3 targetPosition = target->GetAimCenter();

        // 距離の二乗を計算
        D3DXVECTOR3 difference = targetPosition - transform->Position;
        float distanceSq = D3DXVec3LengthSq(&difference);

        // 最も近いターゲットを更新
        if (distanceSq < nearestDistanceSq)
        {
            nearestDistanceSq = distanceSq;
            nearestTarget = target;
        }
    }

    return nearestTarget;
}

//=============================================================================
// 予測線の生成
//=============================================================================
void CAimComponent::CreatePredictionLine(void)
{
    // シーンの取得
    auto scene = CManager::Instance()->GetSceneManager()->GetScene();

    if (scene)
    {
        for (int nCount = 0; nCount < 40; nCount++)
        {
            // ゲームオブジェクトの生成
            auto point = scene->CreateObject();

            // タグの設定
            point->SetTag("MovePoint");

            // トランスフォームコンポーネントの追加
            auto transform = point->AddComponent<CTransformComponent>();

            transform->Position = m_AimPosition;
            //transform->Scale.x = 0.75f + 0.1f * nCount;
            //transform->Scale.y = 0.75f + 0.1f * nCount;
            //transform->Scale.z = 0.75f + 0.1f * nCount;

            // モデルレンダーコンポーネントの追加
            auto model = point->AddComponent<CXModelRenderComponent>();

            // 描画順の設定
            model->SetLayer(RenderLayer::Model);

            // モデルのロード
            model->Load("data/MODELS/MovePoint.x");

            // ゲームオブジェクトの追加
            m_pPointObj.push_back(point);
        }
    }
}

//=============================================================================
// 予測線の更新
//=============================================================================
void CAimComponent::UpdatePredictionLine(void)
{
    // プレイヤーのトランスフォームコンポーネント
    auto transform = m_pOwner->GetComponent<CTransformComponent>();

    if (!transform)
    {
        return;
    }

    D3DXVECTOR3 PlayerPos = transform->Position;
    PlayerPos.y = PlayerPos.y + 50.0f;

    D3DXVECTOR3 dirA = PlayerPos - m_AimPosition;
    D3DXVECTOR3 dirB = m_AimPosition - PlayerPos;
    D3DXVECTOR3 SplineStart = D3DXVECTOR3(PlayerPos.x + dirA.x * 5.0f, PlayerPos.y, PlayerPos.z + dirA.z * 5.0f);
    D3DXVECTOR3 SplineEnd = D3DXVECTOR3(m_AimPosition.x, m_AimPosition.y + dirB.y*5.0f, m_AimPosition.z);

    for (int nCount = 0; nCount < m_pPointObj.size(); nCount++)
    {
        auto pointObj = m_pPointObj[nCount];

        if (!pointObj)
        {
            return;
        }

        // モデルコンポーネントの取得
        auto ModelTransform = pointObj->GetComponent<CTransformComponent>();

        if (!ModelTransform)
        {
            return;
        }
        ModelTransform->Position = MathUtil::Spline(SplineStart, transform->Position, m_AimPosition, SplineEnd, 1.0f / m_pPointObj.size() * nCount);
    }
}