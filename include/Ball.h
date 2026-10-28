//=============================================================================
//
// ボール処理 [Ball.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _BALL_H_// このマクロ定義がされていなかったら
#define _BALL_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "GameObject.h"

//*****************************************************************************
// ボールクラス
//*****************************************************************************
class CBall : public CGameObject
{
public:
    CBall();
    ~CBall() = default;

    HRESULT Init(void) override;
    void Update(void) override;

    // ボールを投げる
    void Throw(const D3DXVECTOR3& velocity);

    bool HasScored(void) const { return m_HasScored; }

    void SetScored(bool scored) { m_HasScored = scored; }

private:

    // ボールの寿命（秒）
    float m_LifeTime;

    // 経過時間
    float m_ElapsedTime;

    bool m_IsThrown;

    bool m_HasScored;
};

#endif