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
	m_String		("fuck")
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

	// テスト : スコア加算
	if (pKeyboard->GetTrigger(DIK_G))
	{
		m_Timing = TIMING_MAX;
		// 最初は非表示
		auto spriteA = m_ButtonA->GetComponent<CSpriteRenderComponent>();
		auto spriteB = m_ButtonB->GetComponent<CSpriteRenderComponent>();

		if (spriteA)spriteA->SetIsVisible(true);
		if (spriteB)spriteB->SetIsVisible(true);


		auto AnimationB = m_ButtonB->GetComponent<CUIAnimationComponent>();
		if (AnimationB)
		{
			AnimationB->Play();
		}
		m_fTimingCount = 3.0;
	}
	float deltaTime = CManager::Instance()->GetTimeSystem()->GetDeltaTime();

	auto spriteB = m_ButtonB->GetComponent<CSpriteRenderComponent>();
	if (!spriteB)return;
	auto AnimationB = m_ButtonB->GetComponent<CUIAnimationComponent>();
	if (!AnimationB)return;


	// カウントが0以下になったらreturn;
	if (m_fTimingCount <= -1.0f)
	{
		//spriteB->SetIsVisible(false);

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
		AnimationB->Stop();

		return;
	}
	m_fTimingCount-= deltaTime;
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
		m_String = "FUCK";
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

