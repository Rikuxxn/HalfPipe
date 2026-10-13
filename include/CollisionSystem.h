//=============================================================================
//
// 衝突判定システム処理 [CollisionSystem.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _COLLISION_SYSTEM_H_// このマクロ定義がされていなかったら
#define _COLLISION_SYSTEM_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "unordered_set"

// 前方宣言
class CGameObject;

using HitMap = std::unordered_map<CGameObject*, std::unordered_set<CGameObject*>>;

//*****************************************************************************
// 衝突判定システムクラス
//*****************************************************************************
class CCollisionSystem
{
public:

    void Update(void);

    void Clear(void);

private:

    void CollectCollision       (HitMap& currentHits);
    void CollectTrigger         (HitMap& currentHits);

    void UpdateCollisionEvents  (const HitMap& currentHits);
    void UpdateTriggerEvents    (const HitMap& triggerHits);

private:

    // 前フレームの衝突状態(Collision用)
    HitMap m_PreviousHits;

    // Trigger用
    HitMap m_PreviousTriggerHits;
};

#endif