//=============================================================================
//
// レイキャストコンポーネント処理 [RaycastComponent.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _RAYCAST_COMPONENT_H_// このマクロ定義がされていなかったら
#define _RAYCAST_COMPONENT_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Component.h"

// 前方宣言
class CGameObject;

//*****************************************************************************
// レイキャストコンポーネントクラス
//*****************************************************************************
class CRaycastComponent : public CComponent
{
public:
    CRaycastComponent();
    virtual ~CRaycastComponent() = default;

public:

    virtual HRESULT Init    (void) override;
    virtual void    Uninit  (void) override;
    virtual void    Update  (void) override;

public:

    //=========================================================================
    // レイキャスト
    //=========================================================================
    /**
    * @param origin     : 原点
    * @param direction  : 方向
    * @param distance   : 距離
    * @return           : ヒットしたかどうか
    */
    bool Raycast(const D3DXVECTOR3& origin, const D3DXVECTOR3& direction, float distance);

    // ヒット情報取得
    CGameObject* GetHitObject(void) const { return m_pHitObject; }

    // 位置
    const D3DXVECTOR3& GetHitPosition(void) const { return m_HitPosition; }

    // 法線
    const D3DXVECTOR3& GetHitNormal(void) const { return m_HitNormal; }

    // 距離
    float GetHitDistance(void) const { return m_HitDistance; }

    // 当たったか
    bool HasHit(void) const { return m_bHit; }

private:

    CGameObject*    m_pHitObject;       // ヒットオブジェクト
    D3DXVECTOR3     m_HitPosition;      // 位置
    D3DXVECTOR3     m_HitNormal;        // 法線
    float           m_HitDistance;      // 距離
    bool            m_bHit;             // 当たったかどうか
};

#endif