//=============================================================================
//
// ジャンプタイミングの評価UI処理 [JumpTimimgRating.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "JumpTimingRating.h"
#include "Manager.h"
#include "SceneManager.h"
#include "Scene.h"
#include "UIComponent.h"
#include "SpriteRenderComponent.h"
#include "UIFactory.h"
#include "UIAnimationManager.h"
#include "UIAnimationComponent.h"
#include "JumpTiming.h"
#include "TimeSystem.h"


//=============================================================================
// コンストラクタ
//=============================================================================
CJumpTimingRating::CJumpTimingRating() :
	m_Rating			(nullptr),				// タイミング評価UIオブジェクト
	m_VisibleState		(VisibleState::None),	// 表示状態
	m_InvisibleTimer	(2.0f)					// 非表示までの時間
{

}

//=============================================================================
// デストラクタ
//=============================================================================
CJumpTimingRating::~CJumpTimingRating()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CJumpTimingRating::Init(void)
{
	// タグの設定
	SetTag("JumpTimingRating");

	// シーンの取得
	auto scene = CManager::Instance()->GetSceneManager()->GetScene();

	if (!scene)
	{
		return E_FAIL;
	}

	// UIアニメーションデータの読み込み
	auto uiManager = CManager::Instance()->GetUIAnimationManager();

	if (uiManager)
	{
		uiManager->Load("data/UIAnimation/Timing.json");
	}

	// 登録済みのUIデータを取得
	UIAnimationData* data = uiManager->GetData("Timing");

	// ジャンプタイミングの評価UIオブジェクトの生成
	m_Rating = CUIFactory::Create(scene, *data);

	auto ui = m_Rating->GetComponent<CUIComponent>();

	if (ui)
	{
		ui->SetUVFrame(0, 4);
	}

	// 最初は非表示
	auto sprite = m_Rating->GetComponent<CSpriteRenderComponent>();

	if (sprite)
	{
		sprite->SetIsVisible(false);
	}

	return S_OK;
}

//=============================================================================
// 更新処理
//=============================================================================
void CJumpTimingRating::Update(void)
{
	// シーンの取得
	auto scene = CManager::Instance()->GetSceneManager()->GetScene();

	if (!scene)
	{
		return;
	}

	// ジャンプタイミングオブジェクトの取得
	auto jumpTiming = scene->FindGameObject<CJumpTiming>("jumpTiming");

	if (!jumpTiming)
	{
		return;
	}

	// デルタタイムの取得
	float deltaTime = CManager::Instance()->GetTimeSystem()->GetDeltaTime();

	// タイミングの取得
	CJumpTiming::TIMING timing = jumpTiming->GetTiming();

	// タイミング判定状態の取得
	CJumpTiming::TimingState timingState = jumpTiming->GetTimingState();

	// UIコンポーネントの取得
	auto ui = m_Rating->GetComponent<CUIComponent>();

	// 2Dレンダーコンポーネントの取得
	auto sprite = m_Rating->GetComponent<CSpriteRenderComponent>();

	switch (m_VisibleState)
	{
	case VisibleState::None:

		if (timingState == CJumpTiming::TimingState::Completed)
		{
			m_VisibleState = VisibleState::Visible;
		}

		break;
	case VisibleState::Visible:
		// 表示(今いらないから消してる)
		//sprite->SetIsVisible(true);
		m_InvisibleTimer = 2.0f;
		m_VisibleState = VisibleState::Count;

		break;
	case VisibleState::Count:

		m_InvisibleTimer -= deltaTime;

		if (sprite->IsVisible() && m_InvisibleTimer <= 0.0f)
		{
			m_InvisibleTimer = 0.0f;

			m_VisibleState = VisibleState::Invisible;
		}

		break;
	case VisibleState::Invisible:
		// 非表示
		sprite->SetIsVisible(false);

		m_VisibleState = VisibleState::None;

		break;
	}

	switch (timing)
	{
	case CJumpTiming::TIMING::TIMING_PERFECT:
		ui->SetUVFrame(3, 4);
		break;
	case CJumpTiming::TIMING::TIMING_GREAT:
		ui->SetUVFrame(2, 4);
		break;
	case CJumpTiming::TIMING::TIMING_GOOD:
		ui->SetUVFrame(1, 4);
		break;
	case CJumpTiming::TIMING::TIMING_BAD:
		ui->SetUVFrame(0, 4);
		break;
	}
}

//=============================================================================
// 表示開始処理
//=============================================================================
void CJumpTimingRating::StartRatingVisible(void)
{
	// 2Dレンダーコンポーネントの取得
	auto sprite = m_Rating->GetComponent<CSpriteRenderComponent>();

	if (!sprite)
	{
		return;
	}

	// 表示
	sprite->SetIsVisible(true);

	// TODO : いずれアニメーションを付ける
}

//=============================================================================
// 表示終了処理
//=============================================================================
void CJumpTimingRating::EndRatingVisible(void)
{
	// 2Dレンダーコンポーネントの取得
	auto sprite = m_Rating->GetComponent<CSpriteRenderComponent>();

	if (!sprite)
	{
		return;
	}

	// 非表示
	sprite->SetIsVisible(false);

	// TODO : いずれアニメーションを付ける

}
