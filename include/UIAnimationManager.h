//=============================================================================
//
// UIアニメーションマネージャー [UIAnimationManager.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _UI_ANIMATION_MANAGER_H_// このマクロ定義がされていなかったら
#define _UI_ANIMATION_MANAGER_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "UIAnimationData.h"

//*****************************************************************************
// UIアニメーションマネージャークラス
//*****************************************************************************
class CUIAnimationManager
{
public:
    CUIAnimationManager() = default;
    ~CUIAnimationManager();

    // 登録済みデータの保存
    bool Save(const std::string& animationName,
        const std::string& filePath);
    
    // 読み込み・登録
    bool Load(const std::string& filePath);

    // データ検索
    UIAnimationData* GetData(const std::string& animationName);

    // データ一覧取得
    std::vector<std::string> GetAnimationNames(void) const;

    // データ登録
    bool Register(const UIAnimationData& data);

    // データ削除
    bool Remove(const std::string& animationName);

private:
    std::unordered_map<std::string, UIAnimationData> m_Datas;
};

#endif