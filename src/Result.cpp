//=============================================================================
//
// リザルト処理 [Result.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Result.h"
#include "Input.h"
#include "Manager.h"
#include "Camera.h"
#include "Input.h"
#include "InputManager.h"
#include "Fade.h"
#include "SceneManager.h"
#include "LightManager.h"
//#include "Ui.h"
#include "Parameter.h"
#include "TransformComponent.h"
#include "SpriteRenderComponent.h"
#include "GameObject.h"
#include "StageManager.h"
#include "StageEditor.h"
#include "TriggerObjectManager.h"
#include "TriggerObjectEditor.h"
//#include "SkyCube.h"
//#include "Cloud.h"

//*****************************************************************************
// 静的メンバ変数宣言
//*****************************************************************************
int CResult::m_clearRankIndex	= 0;			// クリア時のランクインデックス
int CResult::m_soundCount		= 0;			// 音の発生数
int CResult::m_insightCount		= 0;			// 発見された回数
int CResult::m_treasureCount	= 0;			// 宝の数


//*****************************************************************************
// 名前空間
//*****************************************************************************
namespace ResultWorld
{
	const D3DXVECTOR3	DOME_POS{ D3DXVECTOR3(0.0f, -50.0f, 0.0f) };
	const D3DXVECTOR3	PLAYER_POS{ 0.0f, 110.0f, 0.0f };
	const D3DXVECTOR3	PLAYER_ROT{ 0.0f, 180.0f, 0.0f };
	constexpr int		DOME_RADIUS		= 2800;
	constexpr float		BACK_ANCHOR_X	= 0.29f;
	constexpr float		BACK_ANCHOR_Y	= 0.75f;
	constexpr float		BACK_WIDTH		= 0.2f;
	constexpr float		BACK_HEIGHT		= 0.5f;
	constexpr float		COUNT_ANCHOR_X	= 0.2f;
	constexpr float		COUNT1_ANCHOR_Y = 0.2f;
	constexpr float		COUNT2_ANCHOR_Y = 0.46f;
	constexpr float		COUNT3_ANCHOR_Y = 0.72f;
	constexpr float		COUNT_WIDTH		= 0.08f;
	constexpr float		COUNT_HEIGHT	= 0.1f;

}

namespace ResultCamera
{
	const CameraParam PARAM =
	{
		{64.0f, 170.8f, 50.6f},
		{-41.4f, 152.2f, -112.4f},
		{0.10f, 0.57f, 0.0f},
		0.0f
	};
}

namespace ResultLight
{
	const LightParam LIGHTS[] =
	{
		{ D3DLIGHT_DIRECTIONAL, {1.0f, 0.65f, 0.4f, 1.0f}, {-0.3f, -0.8f, 0.2f}, {0,300,0} },
		{ D3DLIGHT_DIRECTIONAL, {1.0f, 0.65f, 0.4f, 1.0f}, {0,-0.8f,-0.2f},   {0,300,0} },
		{ D3DLIGHT_DIRECTIONAL, {0.3f,0.35f,0.5f,1.0f}, {0,1,0},          {0,0,0}   },
		{ D3DLIGHT_DIRECTIONAL, {0.5f,0.4f,0.4f,1.0f},  {0.3f,-0.2f,-0.3f},{0,0,0}  }
	};
}

namespace ResultUI
{
	// 位置
	namespace Pos
	{
		// 「埋蔵金の数」UI
		constexpr float TREASURE_COUNT_X = 0.18f;
		constexpr float TREASURE_COUNT_Y = 0.12f;

		// 「音の発生数」UI
		constexpr float SOUND_COUNT_X = 0.18f;
		constexpr float SOUND_COUNT_Y = 0.37f;

		// 「発見された回数」UI
		constexpr float INSIGHT_COUNT_X = 0.22f;
		constexpr float INSIGHT_COUNT_Y = 0.64f;

		// ランクUI
		constexpr float RANK_X = 0.33f;
		constexpr float RANK_Y = 0.88f;

		// 「評価」UI
		constexpr float EVALUATION_X = 0.3f;
		constexpr float EVALUATION_Y = 0.9f;
	}

	// 大きさ(幅、高さ)
	namespace Size
	{
		// 「埋蔵金の数」UI
		constexpr float TREASURE_COUNT_W = 0.11f;
		constexpr float TREASURE_COUNT_H = 0.05f;

		// 「音の発生数」UI
		constexpr float SOUND_COUNT_W = 0.11f;
		constexpr float SOUND_COUNT_H = 0.05f;

		// 「発見された回数」UI
		constexpr float INSIGHT_COUNT_W = 0.13f;
		constexpr float INSIGHT_COUNT_H = 0.06f;

		// ランクUI
		constexpr float RANK_W = 0.045f;
		constexpr float RANK_H = 0.07f;

		// 「評価」UI
		constexpr float EVALUATION_W = 0.11f;
		constexpr float EVALUATION_H = 0.06f;
	}

	// 色
	const D3DXCOLOR COLOR = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
}

//=============================================================================
// コンストラクタ
//=============================================================================
CResult::CResult()
{
	// 値のクリア
	m_pBlockManager = nullptr;	// ブロックマネージャーへのポインタ
	m_timer			= 0;		// 表示タイマー
}
//=============================================================================
// デストラクタ
//=============================================================================
CResult::~CResult()
{
	// なし
}
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CResult::Init(void)
{
	// 名前空間リザルトルUIの使用
	using namespace ResultUI;

	// マウスカーソルを表示する
	CManager::Instance()->GetInput()->GetMouse()->SetCursorVisibility(true);

	//// スカイキューブの生成
	//CSkyCube::Create();

	//// 雲の生成
	//CCloud::Create();

	// ステージマネージャの初期化
	auto stage = CManager::Instance()->GetStageManager();
	stage->Init();
	//stage->GetStageEditor()->Init();

	// トリガーオブジェクトマネージャの初期化
	auto trigger = CManager::Instance()->GetTriggerObjectManager();
	trigger->Init();
	//trigger->GetTriggerObjectEditor()->Init();

	//// 背景の生成
	//CBackground::Create(nullptr, 
	//	ResultWorld::BACK_ANCHOR_X, ResultWorld::BACK_ANCHOR_Y, ResultWorld::BACK_WIDTH, ResultWorld::BACK_HEIGHT);

	//// ダミープレイヤーの生成
	//CDummyPlayer::Create(ResultWorld::PLAYER_POS, ResultWorld::PLAYER_ROT, CDummyPlayer::DUSH);

	//// 宝獲得数の表示
	//CCount::Create(
	//	ResultWorld::COUNT_ANCHOR_X, ResultWorld::COUNT1_ANCHOR_Y, ResultWorld::COUNT_WIDTH, ResultWorld::COUNT_HEIGHT, m_treasureCount);

	//// 音発生数の表示
	//CCount::Create(
	//	ResultWorld::COUNT_ANCHOR_X, ResultWorld::COUNT2_ANCHOR_Y, ResultWorld::COUNT_WIDTH, ResultWorld::COUNT_HEIGHT, m_soundCount);

	//// 発見された回数の表示
	//CCount::Create(
	//	ResultWorld::COUNT_ANCHOR_X, ResultWorld::COUNT3_ANCHOR_Y, ResultWorld::COUNT_WIDTH, ResultWorld::COUNT_HEIGHT, m_insightCount);

	//// 「埋蔵金の数」UI生成
	//auto treasureCount = CUITexture::Create("data/TEXTURE/ui_treasurecount.png", 
	//	Pos::TREASURE_COUNT_X, Pos::TREASURE_COUNT_Y, ResultUI::COLOR, Size::TREASURE_COUNT_W, Size::TREASURE_COUNT_H);

	//// 「音の発生数」UI生成
	//auto soundCount = CUITexture::Create("data/TEXTURE/ui_soundcount.png", 
	//	Pos::SOUND_COUNT_X, Pos::SOUND_COUNT_Y, ResultUI::COLOR, Size::SOUND_COUNT_W, Size::SOUND_COUNT_H);

	//// 「発見された回数」UI生成
	//auto insight = CUITexture::Create("data/TEXTURE/ui_insightcount.png", 
	//	Pos::INSIGHT_COUNT_X, Pos::INSIGHT_COUNT_Y, ResultUI::COLOR, Size::INSIGHT_COUNT_W, Size::INSIGHT_COUNT_H);

	//// ランクUI生成
	//auto rank = CUITexture::Create("data/TEXTURE/ui_resultRank.png", 
	//	Pos::RANK_X, Pos::RANK_Y, ResultUI::COLOR, Size::RANK_W, Size::RANK_H);

	//// 「評価」UI生成
	//auto evaluation = CUITexture::Create("data/TEXTURE/ui_evaluation.png", 
	//	Pos::EVALUATION_X, Pos::EVALUATION_Y, ResultUI::COLOR, Size::EVALUATION_W, Size::EVALUATION_H);

	//// 「埋蔵金の数」UI登録
	//CUIManager::GetInstance()->AddUI("TreasureCount", treasureCount);

	//// 「音の発生数」UI登録
	//CUIManager::GetInstance()->AddUI("SoundCount", soundCount);

	//// 「発見された回数」UI登録
	//CUIManager::GetInstance()->AddUI("Insight", insight);

	//// ランクUI登録
	//CUIManager::GetInstance()->AddUI("Rank", rank);

	//// 「評価」UI登録
	//CUIManager::GetInstance()->AddUI("Evaluation", evaluation);

	//// UIの初期設定
	//rank->Hide();
	//rank->SetUVDirtyUse(true);

	// UI表示遅延時間の設定
	m_timer = DELAY_TIME;

	//// カメラの取得
	//CCamera* pCamera = CManager::Instance()->GetCamera();

	//// カメラの初期位置を設定しておく
	//pCamera->SetCamParameter(
	//	ResultCamera::PARAM.eye,
	//	ResultCamera::PARAM.at,
	//	ResultCamera::PARAM.rot,
	//	ResultCamera::PARAM.distance);

	//// 音の取得
	//CSound* pSound = CManager::GetSound();

	//// リザルトSEの再生
	//if (pSound)
	//{
	//	pSound->Play(CSound::SOUND_LABEL_RESULTSE);
	//}

	// タイトルロゴオブジェクトの生成
	auto logo = CreateObject();

	// トランスフォームコンポーネントに追加
	auto transform = logo->AddComponent<CTransformComponent>();

	// 位置の設定
	transform->Position = { 0.5f,0.5f,0.0f };

	// スプライトレンダーコンポーネントに追加
	auto sprite = logo->AddComponent<CSpriteRenderComponent>();

	//// 初期化処理
	//sprite->Init();

	// 描画順の設定
	sprite->SetLayer(RenderLayer::UI);

	// テクスチャの設定
	sprite->SetTexture("data/TEXTURE/ui_mission_failure.png");

	// サイズの設定
	sprite->SetSize(500.0f, 300.0f);

	return S_OK;
}
//=============================================================================
// 終了処理
//=============================================================================
void CResult::Uninit(void)
{
	// ステージマネージャの初期化
	auto stage = CManager::Instance()->GetStageManager();
	stage->Reset();

	// トリガーオブジェクトマネージャの初期化
	auto trigger = CManager::Instance()->GetTriggerObjectManager();
	trigger->Reset();

	// シーンの終了処理
	CScene::Uninit();

	//// ブロックマネージャーの破棄
	//if (m_pBlockManager != nullptr)
	//{
	//	m_pBlockManager->Uninit();

	//	delete m_pBlockManager;
	//	m_pBlockManager = nullptr;
	//}
}
//=============================================================================
// 更新処理
//=============================================================================
void CResult::Update(void)
{
	// シーンの更新処理
	CScene::Update();

	//m_timer--;

	//// ランクUIの取得
	//auto rank = CUIManager::GetInstance()->GetUI("Rank");

	//if (m_timer <= 0)
	//{
	//	m_timer = 0;

	//	// ランクUIを表示する
	//	rank->Show();
	//}

	//// ブロックマネージャーの更新処理
	//m_pBlockManager->Update();

	CInputKeyboard* pKeyboard = CManager::Instance()->GetInput()->GetKeyboard();
	CInputJoypad* pJoypad = CManager::Instance()->GetInput()->GetJoypad();		// ゲームパッドの入力取得
	CFade* pFade = CManager::Instance()->GetFade();

	//if (pFade->GetFade() == CFade::FADE_NONE && 
	//	(pKeyboard->GetAnyKeyTrigger() || pJoypad->GetTrigger(CInputJoypad::JOYKEY_A) ||
	//		pKeyboard->GetTrigger(DIK_RETURN)))
	//{
	//	// タイトル画面に移行
	//	CManager::Instance()->GetSceneManager()->SetMode(Mode::Title);
	//}

#ifdef _DEBUG
	if (pFade->GetFade() == CFade::FADE_NONE && pKeyboard->GetTrigger(DIK_RETURN))
	{
		// タイトル画面に移行
		pFade->StartFadeOut(Mode::Title);
	}
#endif
}
//=============================================================================
// 描画処理
//=============================================================================
void CResult::Draw(void)
{
	//// カメラの取得
	//CCamera* pCamera = CManager::Instance()->GetCamera();

	//if (!pCamera->GetMode() == CCamera::MODE_EDIT)
	//{
	//	// カメラの設定
	//	pCamera->SetCamParameter(
	//		ResultCamera::PARAM.eye,
	//		ResultCamera::PARAM.at,
	//		ResultCamera::PARAM.rot,
	//		ResultCamera::PARAM.distance);
	//}

	// シーンの描画処理
	CScene::Draw();
}
//=============================================================================
// ライト設定処理
//=============================================================================
void CResult::ResetLight(void)
{
	//// ライトを削除しておく
	//CLight::Uninit();

	//// ライトの設定
	//for (const auto& light : ResultLight::LIGHTS)
	//{
	//	CLight::AddLight(light.type, light.color, light.dir, light.pos);
	//}
}
//=============================================================================
// デバイスリセット通知
//=============================================================================
void CResult::OnDeviceReset(void)
{
	// ライトの再設定処理
	CLightManager::Instance()->RestoreLights();
}