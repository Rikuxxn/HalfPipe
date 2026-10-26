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
#include "Parameter.h"
#include "TransformComponent.h"
#include "SpriteRenderComponent.h"
#include "GameObject.h"
#include "StageManager.h"
#include "StageEditor.h"
#include "TriggerObjectManager.h"
#include "TriggerObjectEditor.h"
#include "Sky.h"
#include "SoundManager.h"
#include "GameTime.h"


//*****************************************************************************
// 静的メンバ変数宣言
//*****************************************************************************
int CResult::m_clearRankIndex	= 0;			// クリア時のランクインデックス



//=============================================================================
// コンストラクタ
//=============================================================================
CResult::CResult()
{
	// 値のクリア
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
	// マウスカーソルを表示する
	CManager::Instance()->GetInput()->GetMouse()->SetCursorVisibility(true);

	//-----------------------------------------------------
	// マネージャー初期化
	//-----------------------------------------------------
	{
		// ステージマネージャの初期化
		auto stage = CManager::Instance()->GetStageManager();
		stage->Init();

		// トリガーオブジェクトマネージャの初期化
		auto trigger = CManager::Instance()->GetTriggerObjectManager();
		trigger->Init();

		// ステージオブジェクトの読み込み
		CManager::Instance()->GetStageManager()->LoadFromJson("data/StageInfo/Result_Stage.json");

		//// トリガーオブジェクトの読み込み
		//CManager::Instance()->GetTriggerObjectManager()->LoadFromJson("data/TriggerInfo/test_trigger.json");
	}

	// ゲーム時間の取得
	auto time = CManager::Instance()->GetGameTime();

	// ゲーム時間をリセット
	time->Reset();

	// 時間を任意の時間で停止する
	time->Stop();
	time->SetTime(30.0f);

	// 空の生成
	m_pSky = std::make_unique<CSky>();
	m_pSky->Init();

	// カメラの取得
	CCamera* pCamera = CManager::Instance()->GetCamera();

	// カメラの初期位置を設定しておく
	pCamera->SetCamMode(CCamera::MODE_FIXED);

	//// サウンドマネージャーの取得
	//auto sound = CManager::Instance()->GetSoundManager();

	//sound->PlayBGM(CSoundManager::SOUND_LABEL_RESULTBGM);

	return S_OK;
}
//=============================================================================
// 終了処理
//=============================================================================
void CResult::Uninit(void)
{
	// サウンドマネージャーの取得
	auto sound = CManager::Instance()->GetSoundManager();

	sound->StopBGM();

	// ステージマネージャの初期化
	auto stage = CManager::Instance()->GetStageManager();
	stage->Reset();

	// トリガーオブジェクトマネージャの初期化
	auto trigger = CManager::Instance()->GetTriggerObjectManager();
	trigger->Reset();

	// シーンの終了処理
	CScene::Uninit();
}
//=============================================================================
// 更新処理
//=============================================================================
void CResult::Update(void)
{
	// シーンの更新処理
	CScene::Update();

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

	if (pFade->GetFade() == CFade::FADE_NONE && 
		(pKeyboard->GetTrigger(DIK_RETURN) || pJoypad->GetTrigger(CInputJoypad::JOYKEY_A)))
	{
		// タイトル画面に移行
		pFade->StartFadeOut(Mode::Title);
	}

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
	// シーンの描画処理
	CScene::Draw();

	// カメラの取得
	CCamera* pCamera = CManager::Instance()->GetCamera();

	// カメラの設定
	pCamera->SetPosV({ 0.0f, -1235.0f, 4308.0f });
	pCamera->SetPosR({ 0.0f, -1235.0f, 4100.0f });
	pCamera->SetRot({ 0.0f, 0.0f, 0.0f });
}
//=============================================================================
// デバイスリセット通知
//=============================================================================
void CResult::OnDeviceReset(void)
{
	// ライトの再設定処理
	CLightManager::Instance()->RestoreLights();
}