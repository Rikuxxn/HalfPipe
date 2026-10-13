//=============================================================================
//
// UIアニメーションエディター [UIAnimationEditor.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _UI_ANIMATION_EDITOR_H_// このマクロ定義がされていなかったら
#define _UI_ANIMATION_EDITOR_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "UIAnimationData.h"

// 前方宣言
class CUIAnimationManager;
struct UIAnimationData;
struct UIAnimationClip;


//*****************************************************************************
// UIアニメーションエディタークラス
//*****************************************************************************
class CUIAnimationEditor
{
public:
    CUIAnimationEditor() = default;
    ~CUIAnimationEditor() = default;

    // 初期化
    HRESULT Init(CUIAnimationManager* manager);

    // エディタの描画
    void Update(void);

private:

    // アニメーション一覧
    void DrawAnimationList(void);

    // アニメーションの基本情報
    bool DrawAnimationSettings(UIAnimationData& data);

    // クリップ一覧
    void DrawClipList(UIAnimationData& data);

    // 選択中クリップの編集
    bool  DrawClipSettings(UIAnimationClip& clip);

    // 保存・読み込みボタン
    void DrawSaveAndLoadButtons(void);

private:

    // アニメーションマネージャー
    CUIAnimationManager* m_Manager = nullptr;

    // 生成したUIを個別に管理
    std::unordered_map<std::string, CGameObject*> m_GeneratedObjects;

    // 選択中のアニメーション名
    std::string m_SelectedAnimationName;

    // 選択中のクリップ番号
    int m_SelectedClipIndex = -1;

    // 新規アニメーション用の名前
    char m_NewAnimationName[128] = {};

    // 読み込み用のファイルパス
    char m_LoadFilePath[256] = "data/UI/";

    // エディタの表示状態
    bool m_IsOpen = true;
};

#endif