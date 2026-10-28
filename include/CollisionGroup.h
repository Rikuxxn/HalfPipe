//=============================================================================
//
// コリジョングループ処理 [CollisionGroup.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _COLLISION_GROUP_H_// このマクロ定義がされていなかったら
#define _COLLISION_GROUP_H_// 2重インクルード防止のマクロ定義

enum CollisionGroup
{
    COLLISION_GROUP_PLAYER  = 1 << 0,
    COLLISION_GROUP_STAGE   = 1 << 1,
    COLLISION_GROUP_GHOST   = 1 << 2,
    COLLISION_GROUP_ENEMY   = 1 << 3,
    COLLISION_GROUP_CAMERA  = 1 << 4,
    COLLISION_GROUP_BALL    = 1 << 5,
};

#endif