//=============================================================================
//
// タイムシステム処理 [TimeSystem.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _TIME_SYSTEM_H_// このマクロ定義がされていなかったら
#define _TIME_SYSTEM_H_// 2重インクルード防止のマクロ定義


//*****************************************************************************
// タイムシステムクラス
//*****************************************************************************
class CTimeSystem
{
private:

    int   m_FPS;        // FPS値
    float m_DeltaTime;  // デルタタイム

    DWORD m_PrevTime;   // 前回の時間

    float m_FPSTimer;
    int   m_FrameCount;

public:

    CTimeSystem();
    ~CTimeSystem() = default;

    void Update(void);

    int   GetFPS        (void) const { return m_FPS; }
    float GetDeltaTime  (void) const { return m_DeltaTime; }
};


#endif