//
// チュートリアルの状態処理 [TutorialState.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "TutorialState.h"
#include "Manager.h"
#include "Scene.h"
#include "SceneManager.h"
#include <UITextComponent.h>
#include <UIComponent.h>
#include <SpriteRenderComponent.h>
#include "GameObject.h"
#include <UIAnimationData.h>
#include "UIAnimationManager.h"
#include <UIFactory.h>
#include <UIAnimationComponent.h>
#include "TimeSystem.h"
#include <Input.h>
#include "InputManager.h"
#include <Player.h>
#include "AutoMoveComponent.h"


//=============================================================================
// 待機状態の開始処理
//=============================================================================
void CTutorialWaitState::OnStart(CTutorial* /*pTutorial*/)
{
	m_fTime = 3.0f;

	// シーン取得
	auto scene = CManager::Instance()->GetSceneManager()->GetScene();

	if (!scene)
	{
		return;
	}

	m_Text = scene->CreateObject();

	auto uiText = m_Text->AddComponent<CUIComponent>();
	auto uiSprite = m_Text->AddComponent<CSpriteRenderComponent>();

	auto& uiTransform = uiText->GetTransform();
	uiTransform.Position = { 0.5f,0.5f };
	uiTransform.Scale = { 1.1f,1.1f };

	auto text = m_Text->AddComponent<CUITextComponent>();

	text->SetText("<orange>チュートリアルを始めます</orange>");
	text->ShowAll();
	text->SetTypingSpeed(0.05f);
}
//=============================================================================
// 待機状態の更新処理
//=============================================================================
void CTutorialWaitState::OnUpdate(CTutorial* /*pTutorial*/)
{
	m_fTime -= CManager::Instance()->GetTimeSystem()->GetDeltaTime();

	CInputKeyboard* pKeyboard = CManager::Instance()->GetInput()->GetKeyboard();
	CInputJoypad* pJoypad = CManager::Instance()->GetInput()->GetJoypad();

	if (m_fTime > 0.0f)
	{
		return;
	}

	m_fTime = 0.0f;

	if (pKeyboard->GetTrigger(DIK_RETURN) || pJoypad->GetTrigger(CInputJoypad::JOYKEY_A))
	{
		// スタート状態へ移行
		m_pMachine->ChangeState<CTutorialStartState>();
	}
}
//=============================================================================
// 待機状態の終了処理
//=============================================================================
void CTutorialWaitState::OnExit(CTutorial* /*pTutorial*/)
{
	if (m_Text)
	{
		m_Text->Destroy();
	}
}

//=============================================================================
// スタート状態の開始処理
//=============================================================================
void CTutorialStartState::OnStart(CTutorial* /*pTutorial*/)
{
	m_fTime = 1.5f;

	// シーン取得
	auto scene = CManager::Instance()->GetSceneManager()->GetScene();

	if (!scene)
	{
		return;
	}

	// プレイヤーの取得
	auto player = scene->FindGameObject<CPlayer>();

	if (player)
	{
		auto autoMove = player->GetComponent<CAutoMoveComponent>();

		if (autoMove)
		{
			autoMove->StartAutoMove();
		}
	}
}
//=============================================================================
// スタート状態の更新処理
//=============================================================================
void CTutorialStartState::OnUpdate(CTutorial* /*pTutorial*/)
{
	m_fTime -= CManager::Instance()->GetTimeSystem()->GetDeltaTime();

	CInputKeyboard* pKeyboard = CManager::Instance()->GetInput()->GetKeyboard();
	CInputJoypad* pJoypad = CManager::Instance()->GetInput()->GetJoypad();

	if (m_fTime > 0.0f)
	{
		return;
	}

	m_fTime = 0.0f;

	// 着地タイミング説明状態へ移行
	m_pMachine->ChangeState<CLandingTimingState>();
}

//=============================================================================
// 着地タイミング説明状態の開始処理
//=============================================================================
void CLandingTimingState::OnStart(CTutorial* /*pTutorial*/)
{
	m_fTime = 2.0f;

	// シーン取得
	auto scene = CManager::Instance()->GetSceneManager()->GetScene();

	if (!scene)
	{
		return;
	}

	//-----------------------------------------------------
	// JSONに保存したUI (Back UI)
	//-----------------------------------------------------
	{
		// UIアニメーションデータの読み込み
		auto uiManager = CManager::Instance()->GetUIAnimationManager();

		if (uiManager)
		{
			uiManager->Load("data/UIAnimation/TutorialBack.json");
		}

		// 登録済みのUIデータを取得
		UIAnimationData* data = uiManager->GetData("Back");

		// UI生成
		m_pBackUI = CUIFactory::Create(scene, *data);

		// 最初は非表示
		auto sprite = m_pBackUI->GetComponent<CSpriteRenderComponent>();

		if (sprite)
		{
			sprite->SetIsVisible(true);
		}

		// UIアニメーションコンポーネントの取得
		auto anim = m_pBackUI->GetComponent<CUIAnimationComponent>();

		// アニメーション再生
		if (anim)
		{
			anim->Stop();
			anim->PlayAnimation("FadeIn");
		}
	}

	m_Text = scene->CreateObject();

	auto uiText = m_Text->AddComponent<CUIComponent>();
	auto uiSprite = m_Text->AddComponent<CSpriteRenderComponent>();

	auto& uiTransform = uiText->GetTransform();
	uiTransform.Position = { 0.5f,0.5f };
	uiTransform.Scale = { 1.1f,1.1f };

	auto text = m_Text->AddComponent<CUITextComponent>();

	text->SetText("着地するタイミングで スペース または Aボタン を長押しすると \n"
		"溜め状態になります。");
	text->ShowAll();
	text->SetTypingSpeed(0.05f);

	// プレイヤーの取得
	auto player = scene->FindGameObject<CPlayer>();

	if (player)
	{
		auto autoMove = player->GetComponent<CAutoMoveComponent>();

		if (autoMove)
		{
			autoMove->StopAutoMove();
		}
	}
}
//=============================================================================
// 着地タイミング説明状態の更新処理
//=============================================================================
void CLandingTimingState::OnUpdate(CTutorial* /*pTutorial*/)
{
	m_fTime -= CManager::Instance()->GetTimeSystem()->GetDeltaTime();

	CInputKeyboard* pKeyboard = CManager::Instance()->GetInput()->GetKeyboard();
	CInputJoypad* pJoypad = CManager::Instance()->GetInput()->GetJoypad();

	if (m_fTime > 0.0f)
	{
		return;
	}

	m_fTime = 0.0f;

	if (pKeyboard->GetTrigger(DIK_RETURN) || pJoypad->GetTrigger(CInputJoypad::JOYKEY_A))
	{
		// 溜め練習状態へ移行
		m_pMachine->ChangeState<CAccumulationPracticeState>();
	}
}
//=============================================================================
// 着地タイミング説明状態の終了処理
//=============================================================================
void CLandingTimingState::OnExit(CTutorial* /*pTutorial*/)
{
	// 最初は非表示
	auto sprite = m_pBackUI->GetComponent<CSpriteRenderComponent>();

	if (sprite)
	{
		sprite->SetIsVisible(true);
	}

	// UIアニメーションコンポーネントの取得
	auto anim = m_pBackUI->GetComponent<CUIAnimationComponent>();

	// アニメーション再生
	if (anim)
	{
		anim->PlayAnimation("FadeOut");
	}

	if (m_Text)
	{
		m_Text->Destroy();
	}
}

//=============================================================================
// 溜め練習状態の開始処理
//=============================================================================
void CAccumulationPracticeState::OnStart(CTutorial* /*pTutorial*/)
{
	m_fTime = 4.0f;

	// シーン取得
	auto scene = CManager::Instance()->GetSceneManager()->GetScene();

	if (!scene)
	{
		return;
	}

	// プレイヤーの取得
	auto player = scene->FindGameObject<CPlayer>();

	if (player)
	{
		auto autoMove = player->GetComponent<CAutoMoveComponent>();

		if (autoMove)
		{
			autoMove->StartAutoMove();
		}
	}
}

//=============================================================================
// 溜め練習状態の更新処理
//=============================================================================
void CAccumulationPracticeState::OnUpdate(CTutorial* /*pTutorial*/)
{
	m_fTime -= CManager::Instance()->GetTimeSystem()->GetDeltaTime();

	CInputKeyboard* pKeyboard = CManager::Instance()->GetInput()->GetKeyboard();
	CInputJoypad* pJoypad = CManager::Instance()->GetInput()->GetJoypad();

	if (m_fTime > 0.0f)
	{
		return;
	}

	m_fTime = 0.0f;

	// ジャンプタイミング説明状態へ移行
	m_pMachine->ChangeState<CJumpTimingState>();
}

//=============================================================================
// ジャンプタイミング説明状態の開始処理
//=============================================================================
void CJumpTimingState::OnStart(CTutorial* /*pTutorial*/)
{
	m_fTime = 2.0f;

	// シーン取得
	auto scene = CManager::Instance()->GetSceneManager()->GetScene();

	if (!scene)
	{
		return;
	}

	//-----------------------------------------------------
	// JSONに保存したUI (Back UI)
	//-----------------------------------------------------
	{
		// UIアニメーションデータの読み込み
		auto uiManager = CManager::Instance()->GetUIAnimationManager();

		if (uiManager)
		{
			uiManager->Load("data/UIAnimation/TutorialBack.json");
		}

		// 登録済みのUIデータを取得
		UIAnimationData* data = uiManager->GetData("Back");

		// UI生成
		m_pBackUI = CUIFactory::Create(scene, *data);

		// 最初は非表示
		auto sprite = m_pBackUI->GetComponent<CSpriteRenderComponent>();

		if (sprite)
		{
			sprite->SetIsVisible(true);
		}

		// UIアニメーションコンポーネントの取得
		auto anim = m_pBackUI->GetComponent<CUIAnimationComponent>();

		// アニメーション再生
		if (anim)
		{
			anim->Stop();
			anim->PlayAnimation("FadeIn");
		}
	}

	m_Text = scene->CreateObject();

	auto uiText = m_Text->AddComponent<CUIComponent>();
	auto uiSprite = m_Text->AddComponent<CSpriteRenderComponent>();

	auto& uiTransform = uiText->GetTransform();
	uiTransform.Position = { 0.5f,0.5f };
	uiTransform.Scale = { 1.1f,1.1f };

	auto text = m_Text->AddComponent<CUITextComponent>();

	text->SetText("飛ぶタイミングで スペース または Aボタン を離すと \n"
		"ジャンプします。溜めの長さと話すタイミングで高さは変動します。");
	text->ShowAll();
	text->SetTypingSpeed(0.05f);

	// プレイヤーの取得
	auto player = scene->FindGameObject<CPlayer>();

	if (player)
	{
		auto autoMove = player->GetComponent<CAutoMoveComponent>();

		if (autoMove)
		{
			autoMove->StopAutoMove();
		}
	}
}

//=============================================================================
// ジャンプタイミング説明状態の更新処理
//=============================================================================
void CJumpTimingState::OnUpdate(CTutorial* /*pTutorial*/)
{
	m_fTime -= CManager::Instance()->GetTimeSystem()->GetDeltaTime();

	CInputKeyboard* pKeyboard = CManager::Instance()->GetInput()->GetKeyboard();
	CInputJoypad* pJoypad = CManager::Instance()->GetInput()->GetJoypad();

	if (m_fTime > 0.0f)
	{
		return;
	}

	m_fTime = 0.0f;

	if (pKeyboard->GetTrigger(DIK_RETURN) || pJoypad->GetTrigger(CInputJoypad::JOYKEY_A))
	{
		// ジャンプ練習状態へ移行
		m_pMachine->ChangeState<CJumpPracticeState>();
	}
}
//=============================================================================
// ジャンプタイミング説明状態の終了処理
//=============================================================================
void CJumpTimingState::OnExit(CTutorial* /*pTutorial*/)
{
	// 最初は非表示
	auto sprite = m_pBackUI->GetComponent<CSpriteRenderComponent>();

	if (sprite)
	{
		sprite->SetIsVisible(true);
	}

	// UIアニメーションコンポーネントの取得
	auto anim = m_pBackUI->GetComponent<CUIAnimationComponent>();

	// アニメーション再生
	if (anim)
	{
		anim->Stop();
		anim->PlayAnimation("FadeOut");
	}

	if (m_Text)
	{
		m_Text->Destroy();
	}
}

//=============================================================================
// ジャンプ練習状態の開始処理
//=============================================================================
void CJumpPracticeState::OnStart(CTutorial* /*pTutorial*/)
{
	m_fTime = 4.0f;

	// シーン取得
	auto scene = CManager::Instance()->GetSceneManager()->GetScene();

	if (!scene)
	{
		return;
	}

	// プレイヤーの取得
	auto player = scene->FindGameObject<CPlayer>();

	if (player)
	{
		auto autoMove = player->GetComponent<CAutoMoveComponent>();

		if (autoMove)
		{
			autoMove->StartAutoMove();
		}
	}
}

//=============================================================================
// ジャンプ練習状態の更新処理
//=============================================================================
void CJumpPracticeState::OnUpdate(CTutorial* /*pTutorial*/)
{
	m_fTime -= CManager::Instance()->GetTimeSystem()->GetDeltaTime();

	CInputKeyboard* pKeyboard = CManager::Instance()->GetInput()->GetKeyboard();
	CInputJoypad* pJoypad = CManager::Instance()->GetInput()->GetJoypad();

	if (m_fTime > 0.0f)
	{
		return;
	}

	m_fTime = 0.0f;

	//// ジャンプタイミング説明状態へ移行
	//m_pMachine->ChangeState<CJumpTimingState>();
}
