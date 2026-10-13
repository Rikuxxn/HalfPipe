//=============================================================================
//
// トランスフォームコンポーネント処理 [TransformComponent.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "TransformComponent.h"


//=============================================================================
// 向きの設定
//=============================================================================
void CTransformComponent::SetRotation(const D3DXVECTOR3& rotation)
{
    Rotation = rotation;

    D3DXMATRIX matRot;

    D3DXMatrixRotationYawPitchRoll(
        &matRot,
        Rotation.y,
        Rotation.x,
        Rotation.z);

    D3DXQUATERNION dq;

    D3DXQuaternionRotationMatrix(&dq, &matRot);

    Quaternion.x = dq.x;
    Quaternion.y = dq.y;
    Quaternion.z = dq.z;
    Quaternion.w = dq.w;
}

//=============================================================================
// ワールド行列の取得
//=============================================================================
D3DXMATRIX CTransformComponent::GetWorldMatrix(void) const
{
    //---------------------------------
    // SRT
    //---------------------------------

    D3DXMATRIX matScale;
    D3DXMATRIX matRot;
    D3DXMATRIX matTrans;

    D3DXMatrixScaling(
        &matScale,
        Scale.x,
        Scale.y,
        Scale.z);

    // クオータニオンを使用する場合
    if (UseQuaternion)
    {
        D3DXMatrixRotationQuaternion(
            &matRot,
            &Quaternion);
    }
    else
    {
        D3DXMatrixRotationYawPitchRoll(
            &matRot,
            Rotation.y,
            Rotation.x,
            Rotation.z);
    }

    D3DXMatrixTranslation(
        &matTrans,
        Position.x,
        Position.y,
        Position.z);

    D3DXMATRIX local =
        matScale *
        matRot *
        matTrans;

    //---------------------------------
    // 親が居るなら合成
    //---------------------------------

    if (m_Parent)
    {
        return local * m_Parent->GetWorldMatrix();
    }

    return local;
}
