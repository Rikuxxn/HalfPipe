//=============================================================================
//
// UIアニメーションシリアライザー [UIAnimationSerializer.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _UI_ANIMATION_SERIALIZER_H_// このマクロ定義がされていなかったら
#define _UI_ANIMATION_SERIALIZER_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "UIAnimationData.h"

//*****************************************************************************
// UIアニメーションシリアライザークラス
//*****************************************************************************
class CUIAnimationSerializer
{
public:

    // 保存
    static bool Save(const std::string& filePath, const UIAnimationData& data);
    
    // 読み込み
    static bool Load(const std::string& filePath, UIAnimationData& data);
};

#endif