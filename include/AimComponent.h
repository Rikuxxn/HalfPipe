//=============================================================================
//
// エイムコンポーネント処理 [AimComponent.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _AIM_COMPONENT_H_// このマクロ定義がされていなかったら
#define _AIM_COMPONENT_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Component.h"

//*****************************************************************************
// エイムコンポーネントクラス
//*****************************************************************************
class CAimComponent : public CComponent
{
public:

    CAimComponent();
    ~CAimComponent();

    void SetTarget(const D3DXVECTOR3& center, const D3DXQUATERNION& rotation, float radius);
    void SetAimPosition(const D3DXVECTOR3& position)
    {
        m_AimPosition = position;

        ClampAimPosition();
    }

    D3DXVECTOR3 GetAimPosition(void) const { return m_AimPosition; }

    // 照準移動
    void MoveAim(const D3DXVECTOR3& moveDirection, float deltaTime);

private:

    // エイム範囲制限
    void ClampAimPosition(void);

private:
    D3DXVECTOR3     m_AimPosition;      // 照準位置(ワールド)
    D3DXVECTOR3     m_TargetCenter;     // エイム範囲の中心
    D3DXQUATERNION  m_TargetRotation;   // 対象の回転
    float           m_AimRadius;        // エイム範囲
    float           m_AimSpeed;         // エイムスピード

};

#endif