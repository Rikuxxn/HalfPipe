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

    enum class TimingState
    {
        Inactive,   // 判定していない
        Waiting,    // 入力待ち
        Completed   // 判定終了
    };

    CJumpTiming();
    virtual ~CJumpTiming();

    HRESULT Init(void) override;
    void    Update(void) override;

    // タイミング判定の開始処理
    void StartTiming(void);

    // タイミング処理の終了処理
    void EndTiming(void);

    TIMING GetTiming(void) { return m_Timing; }

    TimingState GetTimingState(void) const { return m_TimingState; }

    // 判定結果を取得する。
    // 判定が確定していればtrueを返し、取得済み状態にする。
    bool ConsumeTimingResult(TIMING& result);

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
    TimingState m_TimingState;
    const char* m_String;
};

#endif
