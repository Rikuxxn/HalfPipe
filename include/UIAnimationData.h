//=============================================================================
//
// UIアニメーションデータ [UIAnimationData.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _UI_ANIMATION_DATA_H_// このマクロ定義がされていなかったら
#define _UI_ANIMATION_DATA_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "UIComponent.h"


//*****************************************************************************
// UIアニメーションデータ構造体
//*****************************************************************************
struct UIAnimationData
{
    // GameObjectのタグ
    std::string ObjectTag;

    // アニメーションデータ自体の名前
    std::string AnimationName;

    // テクスチャ
    std::string TexturePath;

    // スプライトサイズ(画面比率)
    float Width = 0.5f;
    float Height = 0.5f;

    // UIの初期状態
    D3DXVECTOR2 Position = { 0.5f, 0.5f };
    D3DXVECTOR2 Scale = { 1.0f, 1.0f };

    float Alpha = 1.0f;

    // アニメーションクリップ一覧
    std::vector<UIAnimationClip> Animations;
};

#endif