//=============================================================================
//
// ジャンプタイミングUI処理 [JumpTiming.cpp]
// Author : SOICHIRO SASAKI
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "JumpTiming.h"
#include "Manager.h"
#include "SceneManager.h"
#include "Scene.h"
#include "UIComponent.h"
#include "SpriteRenderComponent.h"
#include "UIFactory.h"
#include "UIAnimationManager.h"
#include "UIAnimationComponent.h"
#include "InputManager.h"
#include "Input.h"
#include "TimeSystem.h"
#include "imguimaneger.h"




//=============================================================================
// コンストラクタ
//=============================================================================
CJumpTiming::CJumpTiming():
	m_ButtonA		(nullptr),
	m_ButtonB		(nullptr),
	m_fTimingCount	(0.0f),
	m_Timing		(TIMING_MAX),
	m_TimingState	(TimingState::Inactive),
	m_String		("None")
{

}

//=============================================================================
// デストラクタ
//=============================================================================
CJumpTiming::~CJumpTiming()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CJumpTiming::Init(void)
{
	SetTag("jumpTiming");

	////---------------------------------------------------
	// アニメーション付きUIの生成
	//---------------------------------------------------
	auto scene = CManager::Instance()->GetSceneManager()->GetScene();
	if (!scene)
	{
		return E_FAIL;
	}

	// UIアニメーションデータの読み込み
	auto uiManager = CManager::Instance()->GetUIAnimationManager();

	if (uiManager)
	{
		uiManager->Load("data/UIAnimation/ButtonA.json");
	}

	// 登録済みのUIデータを取得
	UIAnimationData* dataA = uiManager->GetData("ButtonA");

	// UI生成
	m_ButtonA = CUIFactory::Create(scene, *dataA);

	// 最初は非表示
	auto spriteA = m_ButtonA->GetComponent<CSpriteRenderComponent>();

	if (spriteA)
	{
		spriteA->SetIsVisible(false);
	}

	if (uiManager)
	{
		uiManager->Load("data/UIAnimation/TimingCircle.json");
	}

	// 登録済みのUIデータを取得
	UIAnimationData* dataB = uiManager->GetData("TimingCircle");

	// UI生成
	m_ButtonB = CUIFactory::Create(scene, *dataB);

	// 最初は非表示
	auto spriteB = m_ButtonB->GetComponent<CSpriteRenderComponent>();

	if (spriteB)
	{
		spriteB->SetIsVisible(false);
	}


	return S_OK;
}

//=============================================================================
// 更新処理
//=============================================================================
void CJumpTiming::Update(void)
{
	CInputKeyboard* pKeyboard = CManager::Instance()->GetInput()->GetKeyboard();

	// デルタタイムの取得
	float deltaTime = CManager::Instance()->GetTimeSystem()->GetDeltaTime();

	// テスト : Gキーでタイミング判定開始
	if (pKeyboard->GetTrigger(DIK_G))
	{
		StartTiming();
	}

	// 判定中でなければ処理しない
	if (m_TimingState != TimingState::Waiting)
	{
		return;
	}

	auto AnimationB = m_ButtonB->GetComponent<CUIAnimationComponent>();
	
	if (!AnimationB)
	{
		return;
	}

	if (pKeyboard->GetTrigger(DIK_SPACE))
	{
		// タイミングごと
		if (m_fTimingCount < -deltaTime*5)m_Timing = TIMING_BAD;
		else if (m_fTimingCount < -deltaTime*3)m_Timing = TIMING_GOOD;
		else if (m_fTimingCount < 0.0f)m_Timing = TIMING_GREAT;
		else if (m_fTimingCount >= 0.0f&&m_fTimingCount < deltaTime)m_Timing = TIMING_PERFECT;
		else if(m_fTimingCount < deltaTime*3)m_Timing = TIMING_GREAT;
		else if(m_fTimingCount< deltaTime*5)m_Timing = TIMING_GOOD;
		else m_Timing = TIMING_BAD;
		
		// アニメーション停止
		AnimationB->Stop();

		// 判定終了
		m_TimingState = TimingState::Completed;

		return;
	}

	m_fTimingCount-= deltaTime;

	// カウントが0以下ならBADとして判定を終了
	if (m_fTimingCount <= -1.0f)
	{
		m_Timing = TIMING_BAD;
		m_TimingState = TimingState::Completed;

		AnimationB->Stop();
	}
}

//=============================================================================
// タイミング判定開始処理
//=============================================================================
void CJumpTiming::StartTiming(void)
{
	// すでに判定中なら開始しない
	if (m_TimingState == TimingState::Waiting)
	{
		return;
	}

	// 判定結果を初期化
	m_Timing = TIMING_MAX;

	// 判定中にする
	m_TimingState = TimingState::Waiting;

	// カウントを初期化
	m_fTimingCount = 3.0f;

	//// ボタンAを表示
	//if (m_ButtonA)
	//{
	//	auto spriteA = m_ButtonA->GetComponent<CSpriteRenderComponent>();

	//	if (spriteA)
	//	{
	//		spriteA->SetIsVisible(true);
	//	}
	//}

	//// ボタンBを表示してアニメーション再生
	//if (m_ButtonB)
	//{
	//	auto spriteB = m_ButtonB->GetComponent<CSpriteRenderComponent>();

	//	auto animationB = m_ButtonB->GetComponent<CUIAnimationComponent>();

	//	if (spriteB)
	//	{
	//		spriteB->SetIsVisible(true);
	//	}

	//	if (animationB)
	//	{
	//		animationB->PlayAnimation("Scale_X");
	//		animationB->PlayAnimation("Scale_Y");
	//	}
	//}
}

//=============================================================================
// タイミング判定終了処理
//=============================================================================
void CJumpTiming::EndTiming(void)
{
	// すでに完了中なら開始しない
	if (m_TimingState == TimingState::Completed)
	{
		return;
	}

	// 判定結果を初期化
	m_Timing = TIMING_MAX;

	//// 判定完了中にする
	//m_TimingState = TimingState::Completed;

	//// カウントを初期化
	//m_fTimingCount = 3.0f;

	// ボタンAを表示
	if (m_ButtonA)
	{
		auto spriteA = m_ButtonA->GetComponent<CSpriteRenderComponent>();

		if (spriteA)
		{
			spriteA->SetIsVisible(false);
		}
	}

	// ボタンBを表示してアニメーション再生
	if (m_ButtonB)
	{
		auto spriteB = m_ButtonB->GetComponent<CSpriteRenderComponent>();

		if (spriteB)
		{
			spriteB->SetIsVisible(false);
		}
	}
}

//=============================================================================
// タイミング判定結果を取得
//=============================================================================
bool CJumpTiming::ConsumeTimingResult(TIMING& result)
{
	// 判定が終了していなければ取得できない
	if (m_TimingState != TimingState::Completed)
	{
		return false;
	}

	// 判定結果を取得
	result = m_Timing;

	// 取得済みにする
	m_TimingState = TimingState::Inactive;

	return true;
}

#ifdef _DEBUG
//=============================================================================
// Imguiのデバッグ描画処理
//=============================================================================
void CJumpTiming::DrawDebugGUI(void)
{
	switch (m_Timing)
	{
	case TIMING_PERFECT:
		m_String = "PERFECT";
		break;
	case TIMING_GREAT:
		m_String = "GREAT";
		break;
	case TIMING_GOOD:
		m_String = "GOOD";
		break;
	case TIMING_BAD:
		m_String = "BAD";
		break;
	case TIMING_MAX:
		m_String = "NONE";
		break;
	}

	if (ImGui::TreeNode("JumpTiming Info"))
	{
		if (ImGui::Button(m_String, ImVec2(100, 40)))
		{

		}
		ImGui::TreePop();
	}
}
#endif

