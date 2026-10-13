//=============================================================================
//
// トランスフォームコンポーネント処理 [TransformComponent.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _TRANSFORM_COMPONENT_H_// このマクロ定義がされていなかったら
#define _TRANSFORM_COMPONENT_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Component.h"

//*****************************************************************************
// トランスフォームコンポーネントクラス
//*****************************************************************************
class CTransformComponent : public CComponent
{
public:
    D3DXVECTOR3     Position;
    D3DXVECTOR3     Rotation;
    D3DXVECTOR3     Scale;

    D3DXQUATERNION Quaternion;
    bool UseQuaternion = false;// クオータニオンを使用するか

private:

    CTransformComponent* m_Parent;

public:
    CTransformComponent()
    {
        Position    = { 0, 0, 0 };
        Rotation    = { 0, 0, 0 };
        Scale       = { 1, 1, 1 };
        Quaternion  = { 0, 0, 0, 1 };
    
        m_Parent = nullptr;
    }

public:

    void SetRotation(const D3DXVECTOR3& rotation);

    void SetParent(CTransformComponent* parent)
    {
        m_Parent = parent;
    }

    CTransformComponent* GetParent(void) const
    {
        return m_Parent;
    }

    D3DXMATRIX GetWorldMatrix(void) const;
};

#endif
