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


//=============================================================================
// コンストラクタ
//=============================================================================
CAimComponent::CAimComponent() :
    m_AimPosition   (INIT_VEC3),
    m_TargetCenter  (INIT_VEC3),
    m_TargetRotation({ 0, 0, 0, 1 }),
    m_AimRadius     (1.8f),
    m_AimSpeed      (3.0f)
{

}

//=============================================================================
// デストラクタ
//=============================================================================
CAimComponent::~CAimComponent()
{

}

//=============================================================================
// ターゲットの設定
//=============================================================================
void CAimComponent::SetTarget(const D3DXVECTOR3& center, const D3DXQUATERNION& rotation, float radius)
{
    m_TargetCenter = center;
    m_TargetRotation = rotation;
    m_AimRadius = radius;

    // 初期照準位置はBasketの中心
    m_AimPosition = center;
}

//=============================================================================
// 照準移動
//=============================================================================
void CAimComponent::MoveAim(const D3DXVECTOR3& moveDirection, float deltaTime)
{
    m_AimPosition += moveDirection * m_AimSpeed * deltaTime;

    ClampAimPosition();
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

    // ターゲットののローカル平面上で距離を計算
    // ローカルXY平面を想定
    localOffset.z = 0.0f;

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
