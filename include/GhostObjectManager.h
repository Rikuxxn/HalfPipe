//=============================================================================
//
// ゴーストオブジェクトマネージャー処理 [GhostObjectManager.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _GHOST_OBJECT_MANAGER_H_// このマクロ定義がされていなかったら
#define _GHOST_OBJECT_MANAGER_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include <BulletCollision/CollisionDispatch/btGhostObject.h>


//*****************************************************************************
// ゴーストオブジェクトマネージャークラス
//*****************************************************************************
class CGhostObjectManager
{
public:
    CGhostObjectManager();
    ~CGhostObjectManager();

    // 登録
    void AddGhost(btGhostObject* ghost);

    // 削除
    void RemoveGhost(btGhostObject* ghost);


    // ゴースト一覧取得
    const std::vector<btGhostObject*>& GetGhosts(void) const;

private:

    std::vector<btGhostObject*> m_Ghosts;
};

#endif