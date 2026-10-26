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

    // 初期照準位置はBasketの中心
    m_AimPosition = m_TargetCenter;

    m_IsTargetSet = true;
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