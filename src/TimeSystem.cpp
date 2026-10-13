//=============================================================================
//
// タイムシステム処理 [TimeSystem.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "TimeSystem.h"
#include "mmsystem.h"


//=============================================================================
// コンストラクタ
//=============================================================================
CTimeSystem::CTimeSystem() :
    m_FPS           (0),    // FPS値
    m_DeltaTime     (0.0f), // デルタタイム
    m_FPSTimer      (0.0f), // タイマー
    m_FrameCount    (0)     // カウント
{
    m_PrevTime = timeGetTime();
}

//=============================================================================
// 更新処理
//=============================================================================
void CTimeSystem::Update(void)
{
    DWORD currentTime = timeGetTime();

    DWORD elapsed = currentTime - m_PrevTime;

    m_PrevTime = currentTime;

    m_DeltaTime = elapsed / 1000.0f;

    // FPS計測
    m_FrameCount++;
    m_FPSTimer += m_DeltaTime;

    // 1秒毎に更新
    if (m_FPSTimer >= 1.0f)
    {
        m_FPS = static_cast<int>(1.0f / m_DeltaTime);
    
        m_FrameCount = 0;
        m_FPSTimer = 0.0f;
    }
}