//=============================================================================
//
// ジャンプタイミングの評価UI処理 [JumpTimimgRating.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _JUMP_TIMING_RATING_H_// このマクロ定義がされていなかったら
#define _JUMP_TIMING_RATING_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "GameObject.h"

//*****************************************************************************
// ジャンプタイミングの評価クラス
//*****************************************************************************
class CJumpTimingRating : public CGameObject
{
public:

    enum class VisibleState
    {
        None,       // 何もなし
        Visible,    // 表示
        Count,      // 非表示までのカウント中
        Invisible   // 非表示
    };

    CJumpTimingRating();
    virtual ~CJumpTimingRating();

    HRESULT Init(void) override;
    void    Update(void) override;

    // 表示開始
    void StartRatingVisible(void);

    // 表示終了
    void EndRatingVisible(void);

private:

    CGameObject* m_Rating;          // タイミング評価UIオブジェクト

    VisibleState m_VisibleState;    // 表示状態

    float m_InvisibleTimer;         // 非表示までの時間
};

#endif