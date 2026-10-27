//=============================================================================
//
// トリガーオブジェクトリスト処理 [TriggerObjectList.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "TriggerObjectList.h"
#include "Manager.h"
#include "SceneManager.h"
#include "Scene.h"
#include "UIAnimationData.h"
#include "UIAnimationManager.h"
#include "UIAnimationComponent.h"
#include "UIComponent.h"
#include "SpriteRenderComponent.h"
#include "UIFactory.h"


//=============================================================================
// ゴールエリアのコンストラクタ
//=============================================================================
CGoalTrigger::CGoalTrigger() :
	m_FinishUI(nullptr)				// FinishUIオブジェクト
{

}

//=============================================================================
// ゴールエリアのデストラクタ
//=============================================================================
CGoalTrigger::~CGoalTrigger()
{

}

//=============================================================================
// ゴールエリアの初期化処理
//=============================================================================
HRESULT CGoalTrigger::Init(void)
{
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
		uiManager->Load("data/UIAnimation/Finish.json");
	}

	// 登録済みのUIデータを取得
	UIAnimationData* data = uiManager->GetData("Finish");

	// FinishUIオブジェクトの生成
	m_FinishUI = CUIFactory::Create(scene, *data);

	//auto ui = m_FinishUI->GetComponent<CUIComponent>();

	//if (ui)
	//{
	//	ui->SetUVFrame(0, 4);
	//}

	// 最初は非表示
	auto sprite = m_FinishUI->GetComponent<CSpriteRenderComponent>();

	if (sprite)
	{
		sprite->SetIsVisible(false);
	}

	return S_OK;
}

//=============================================================================
// ゴールエリアの更新処理
//=============================================================================
void CGoalTrigger::Update(void)
{
	CTriggerObject::Update();
}

//=============================================================================
// ゴールエリアのコリジョンエンター
//=============================================================================
void CGoalTrigger::OnTriggerEnter(CGameObject* other)
{
	// プレイヤーがゴールしたらUIを表示
	if (other->CompareTag("Player"))
	{
		// 2Dレンダーコンポーネントの取得
		auto sprite = m_FinishUI->GetComponent<CSpriteRenderComponent>();

		// 表示する
		if (sprite)
		{
			sprite->SetIsVisible(true);
		}

		// UIアニメーションコンポーネントの取得
		auto anim = m_FinishUI->GetComponent<CUIAnimationComponent>();

		// アニメーション再生
		if (anim)
		{
			anim->Play();
		}
	}
}
