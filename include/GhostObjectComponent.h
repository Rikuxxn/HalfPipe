//=============================================================================
//
// ゴーストオブジェクトコンポーネント処理 [GhostObjectComponent.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _GHOST_OBJECT_COMPONENT_H_// このマクロ定義がされていなかったら
#define _GHOST_OBJECT_COMPONENT_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Component.h"
#include "unordered_set"
#include <BulletCollision/CollisionDispatch/btGhostObject.h>


//*****************************************************************************
// ゴーストオブジェクトコンポーネントクラス
//*****************************************************************************
class CGhostObjectComponent : public CComponent
{
public:
    CGhostObjectComponent();
    virtual ~CGhostObjectComponent();

    void ReleaseGhostObject(void);
    void Update(void);
    void Transform(void);
    void UpdateFromScale(const D3DXVECTOR3& scale);

    void CreateBox(const D3DXVECTOR3& size);
    void CreateCylinder(const D3DXVECTOR3& size);
    //void CreateSphere(const D3DXVECTOR3& size);
    //void CreateCapsule(const D3DXVECTOR3& size);

    // フィルターの設定
    void SetCollisionFilter(short group, short mask)
    {
        m_CollisionGroup = group;
        m_CollisionMask = mask;
    }

    btGhostObject* GetGhost(void)
    {
        return m_Ghost;
    }

private:

    btGhostObject*      m_Ghost;            // ゴーストオブジェクト
    btCollisionShape*   m_Shape;            // コリジョンシェイプ
    short               m_CollisionGroup;   // コリジョングループ
    short               m_CollisionMask;    // コリジョンマスク

};

#endif