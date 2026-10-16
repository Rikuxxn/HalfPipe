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
    CJumpTiming();
    virtual ~CJumpTiming();

    HRESULT Init(void) override;
    void    Update(void) override;

#ifdef _DEBUG
    // Imguiのデバッグ描画
    void DrawDebugGUI(void) override;

#endif

public:


private:


private:
    CGameObject* m_frame;
    CGameObject* m_circleButton;

};

#endif
