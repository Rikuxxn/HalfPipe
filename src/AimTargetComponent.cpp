//=============================================================================
//
// エイム対象コンポーネント処理 [AimTargetComponent.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "AimTargetComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "MatrixUtility.h"

// 名前空間の使用
using namespace MatrixUtil;


//=============================================================================
// コンストラクタ
//=============================================================================
CAimTargetComponent::CAimTargetComponent() :
    m_LocalCenter   (INIT_VEC3),
	m_AimRadius     (1.8f)
{

}

//=============================================================================
// エイム中心位置取得
//=============================================================================
D3DXVECTOR3 CAimTargetComponent::GetAimCenter(void) const
{
    // トランスフォームコンポーネントの取得
    auto transform = m_pOwner->GetComponent<CTransformComponent>();

    if (!transform)
    {
        return INIT_VEC3;
    }

    return LocalToWorld(
        m_LocalCenter,
        transform->Position,
        transform->Rotation,
        transform->Scale
    );
}

//=============================================================================
// エイム対象の回転取得
//=============================================================================
D3DXQUATERNION CAimTargetComponent::GetAimRotation(void) const
{
    // トランスフォームコンポーネントの取得
    auto transform = m_pOwner->GetComponent<CTransformComponent>();

    if (!transform)
    {
        return D3DXQUATERNION(0.0f, 0.0f, 0.0f, 1.0f);
    }

    // クォータニオンを使用している場合
    if (transform->UseQuaternion)
    {
        return transform->Quaternion;
    }

    // オイラー角からクォータニオンへ変換
    D3DXQUATERNION rotation;

    D3DXQuaternionRotationYawPitchRoll(
        &rotation,
        transform->Rotation.y,
        transform->Rotation.x,
        transform->Rotation.z
    );

    return rotation;
}

//=============================================================================
// エイム半径取得
//=============================================================================
float CAimTargetComponent::GetAimRadius(void) const
{
    return m_AimRadius;
}

//=============================================================================
// エイム半径設定
//=============================================================================
void CAimTargetComponent::SetAimRadius(float radius)
{
    m_AimRadius = (radius > 0.0f) ? radius : 0.0f;
}

//=============================================================================
// ローカル中心設定
//=============================================================================
void CAimTargetComponent::SetLocalCenter(const D3DXVECTOR3& center)
{
    m_LocalCenter = center;
}