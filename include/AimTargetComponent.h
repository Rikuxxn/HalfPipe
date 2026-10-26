//=============================================================================
//
// エイム対象コンポーネント処理 [AimTargetComponent.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _AIM_TARGET_COMPONENT_H_// このマクロ定義がされていなかったら
#define _AIM_TARGET_COMPONENT_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Component.h"

//*****************************************************************************
// エイム対象コンポーネントクラス
//*****************************************************************************
class CAimTargetComponent : public CComponent
{
public:
    CAimTargetComponent();
    ~CAimTargetComponent() override = default;

    // エイム対象の中心位置を取得
    D3DXVECTOR3 GetAimCenter(void) const;

    // エイム対象の回転を取得
    D3DXQUATERNION GetAimRotation(void) const;

    // エイム可能な半径を取得
    float GetAimRadius(void) const;

    // エイム可能な半径を設定
    void SetAimRadius(float radius);

    void SetLocalCenter(const D3DXVECTOR3& center);

private:
    D3DXVECTOR3 m_LocalCenter;
    float       m_AimRadius;
};

#endif