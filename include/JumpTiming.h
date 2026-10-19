//=============================================================================
//
// ジャンプタイミングUI処理 [JumpTimimg.h]
// Author : SOICHIRO SASAKI
//
//=============================================================================
#ifndef _JUMPTIMING_H_// このマクロ定義がされていなかったら
#define _JUMPTIMING_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "GameObject.h"
#include "State.h"

//*****************************************************************************
// プレイヤークラス
//*****************************************************************************
class CJumpTiming : public CGameObject
{
public:
    // タイミングの列挙型
    typedef enum
    {
        TIMING_PERFECT = 0, // 完璧
        TIMING_GREAT,       // 良い
        TIMING_GOOD,        // まぁまぁ
        TIMING_BAD,         // 悪い
        TIMING_MAX
    }TIMING;

    CJumpTiming();
    virtual ~CJumpTiming();

    HRESULT Init(void) override;
    void    Update(void) override;

    TIMING GetTiming(void) { return m_Timing; }

#ifdef _DEBUG
    // Imguiのデバッグ描画
    void DrawDebugGUI(void) override;

#endif

public:

private:


private:
    CGameObject* m_ButtonA;
    CGameObject* m_ButtonB;
    float m_fTimingCount;
    TIMING m_Timing;
    const char* m_String;
};

#endif
