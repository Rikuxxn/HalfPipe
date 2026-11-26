//=============================================================================
//
// チュートリアルの状態処理 [TutorialState.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _TUTORIAL_STATE_H_// このマクロ定義がされていなかったら
#define _TUTORIAL_STATE_H_//2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "State.h"

//*****************************************************************************
// 前方宣言
//*****************************************************************************
class CTutorial;
class CGameObject;


//*****************************************************************************
// 待機状態
//*****************************************************************************
class CTutorialWaitState :public StateBase<CTutorial>
{
public:

	void OnStart(CTutorial* /*pTutorial*/) override;
	void OnUpdate(CTutorial* /*pTutorial*/) override;
	void OnExit(CTutorial* /*pTutorial*/) override;

private:

	float m_fTime = 0.0f;

	CGameObject* m_Text;
};

//*****************************************************************************
// スタート状態
//*****************************************************************************
class CTutorialStartState :public StateBase<CTutorial>
{
public:

	void OnStart(CTutorial* /*pTutorial*/) override;
	void OnUpdate(CTutorial* /*pTutorial*/) override;
	void OnExit(CTutorial* /*pTutorial*/) override {}

private:
	float m_fTime = 0.0f;

	// 背景 UI
	CGameObject* m_pBackUI;

};

//*****************************************************************************
// 着地タイミング説明状態
//*****************************************************************************
class CLandingTimingState :public StateBase<CTutorial>
{
public:

	void OnStart(CTutorial* /*pTutorial*/) override;
	void OnUpdate(CTutorial* /*pTutorial*/) override;
	void OnExit(CTutorial* /*pTutorial*/) override;

private:
	float m_fTime = 0.0f;

	// 背景 UI
	CGameObject* m_pBackUI;

	CGameObject* m_Text;
};

//*****************************************************************************
// 溜め練習状態
//*****************************************************************************
class CAccumulationPracticeState :public StateBase<CTutorial>
{
public:

	void OnStart(CTutorial* /*pTutorial*/) override;
	void OnUpdate(CTutorial* /*pTutorial*/) override;
	void OnExit(CTutorial* /*pTutorial*/) override {}

private:
	float m_fTime = 0.0f;

	// 背景 UI
	CGameObject* m_pBackUI;
};

//*****************************************************************************
// ジャンプタイミング説明状態
//*****************************************************************************
class CJumpTimingState :public StateBase<CTutorial>
{
public:

	void OnStart(CTutorial* /*pTutorial*/) override;
	void OnUpdate(CTutorial* /*pTutorial*/) override;
	void OnExit(CTutorial* /*pTutorial*/) override;

private:
	float m_fTime = 0.0f;

	// 背景 UI
	CGameObject* m_pBackUI;

	CGameObject* m_Text;
};

//*****************************************************************************
// ジャンプ練習状態
//*****************************************************************************
class CJumpPracticeState :public StateBase<CTutorial>
{
public:

	void OnStart(CTutorial* /*pTutorial*/) override;
	void OnUpdate(CTutorial* /*pTutorial*/) override;
	void OnExit(CTutorial* /*pTutorial*/) override {}

private:
	float m_fTime = 0.0f;
};

#endif