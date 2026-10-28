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

// 前方宣言
class CAimTargetComponent;

//*****************************************************************************
// エイムコンポーネントクラス
//*****************************************************************************
class CAimComponent : public CComponent
{
public:

    CAimComponent();
    ~CAimComponent();
    
    HRESULT Init    (void) override;
    void    Update  (void) override;

    void SetTarget(CAimTargetComponent* target);
    
    // エイム位置の設定
    void SetAimPosition(const D3DXVECTOR3& position);

    // エイム位置をランダムに設定
    void SetRandomAimPosition(void);

    D3DXVECTOR3 GetAimPosition(void) const { return m_AimPosition; }

    // 照準移動
    void MoveAim(const D3DXVECTOR3& moveDirection, float deltaTime);

    // エイム表示処理
    void VisibleAim(void);

    // エイム非表示処理
    void InvisibleAim(void);

private:

    // エイム表示の生成
    void CreateAimMarker(void);

    // エイム表示の更新
    void UpdateAimMarker(void);

    // エイム範囲制限
    void ClampAimPosition(void);

    // 最も近い照準ターゲットを取得
    CAimTargetComponent* FindNearestTarget(void);

private:
    CAimTargetComponent*    m_pTarget;          // エイム対象
    CGameObject*            m_pAimMarker;       // エイム表示用オブジェクト
    D3DXVECTOR3             m_AimPosition;      // 照準位置(ワールド)
    D3DXVECTOR3             m_TargetCenter;     // エイム範囲の中心
    D3DXQUATERNION          m_TargetRotation;   // 対象の回転
    float                   m_AimRadius;        // エイム範囲
    float                   m_AimSpeed;         // エイムスピード
    bool                    m_IsTargetSet;      // ターゲットが設定されているか

};

#endif