//=============================================================================
//
// UIファクトリー処理 [UIFactory.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _UI_FACTORY_H_// このマクロ定義がされていなかったら
#define _UI_FACTORY_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "UIAnimationData.h"

// 前方宣言
class CScene;
class CGameObject;

//*****************************************************************************
// UIファクトリークラス
//*****************************************************************************
class CUIFactory
{
public:

    // UI生成
    static CGameObject* Create(CScene* scene, const UIAnimationData& data);

    // 基本設定の反映
    static void ApplyTransform(
        CGameObject* object,
        const UIAnimationData& data);

    // アニメーション設定の反映
    static void ApplyAnimations(
        CGameObject* object,
        const UIAnimationData& data);
};

#endif