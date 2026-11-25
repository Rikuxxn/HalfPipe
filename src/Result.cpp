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
#include <Player.h>
#include <NetWorkManager.h>
#include <CharacterComponent.h>
#include <RigidBodyComponent.h>
#include <CollisionGroup.h>
#include <UIComponent.h>
#include <ScoreRenderComponent.h>
#include "UIAnimationManager.h"
#include <UIFactory.h>


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


	//============================================================
	// Player生成(クライアント)
	//============================================================
	{
		auto network = CManager::Instance()->GetNetworkManager();

		if (network && network->IsPlayerScoresReceived())
		{
			// 順位計算
			CalculateRanking();

			// 順位順にプレイヤー・スコア生成
			for (const auto& ranking : m_Rankings)
			{
				SpawnPlayer(ranking.playerId, ranking.rank);

				SpawnScore(ranking);
			}
		}
	}

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
	// シーンの終了処理
	CScene::Uninit();

	// サウンドマネージャーの取得
	auto sound = CManager::Instance()->GetSoundManager();

	sound->StopBGM();

	// ステージマネージャの初期化
	auto stage = CManager::Instance()->GetStageManager();
	stage->Reset();

	// トリガーオブジェクトマネージャの初期化
	auto trigger = CManager::Instance()->GetTriggerObjectManager();
	trigger->Reset();
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
	pCamera->SetPosV({ 0.0f, -1175.0f, 4308.0f });
	pCamera->SetPosR({ 0.0f, -1175.0f, 4100.0f });
	pCamera->SetRot({ 0.0f, 0.0f, 0.0f });
}

//=============================================================================
// プレイヤー生成
//=============================================================================
void CResult::SpawnPlayer(uint8_t playerId, uint8_t rank)
{
	auto network = CManager::Instance()->GetNetworkManager();

	if (!network)
	{
		return;
	}

	if (playerId >= Network::MAX_PLAYERS)
	{
		return;
	}

	// 同じIDのプレイヤーが存在するなら生成しない
	if (FindPlayerById(playerId))
	{
		return;
	}

	// プレイヤー生成
	auto player = CreateObject<CPlayer>();

	if (!player)
	{
		return;
	}

	player->SetTag("Player");

	player->SetPlayerId(playerId);

	const bool isLocal = (playerId == network->GetLocalPlayerId());

	player->SetLocalPlayer(isLocal);

	// 自分のプレイヤーはローカル入力で操作する
	// 相手のプレイヤーはネットワークの状態で動かす
	player->SetNetworkControlled(!isLocal);

	// トランスフォームコンポーネントの追加
	auto transform = player->AddComponent<CTransformComponent>();

	transform->Position = m_RankPositions[rank - 1];
	transform->Rotation = { 0.0f, D3DXToRadian(180.0f), 0.0f };

	// キャラクター
	auto character = player->AddComponent<CCharacterComponent>();

	// IDによって、色を変える
	switch (playerId)
	{
	case 0:
		character->Load("data/MOTION/motion_player.txt");
		break;
	case 1:
		character->Load("data/MOTION/motion_player_blue.txt");
		break;
	case 2:
		character->Load("data/MOTION/motion_player_green.txt");
		break;
	case 3:
		character->Load("data/MOTION/motion_player_yellow.txt");
		break;
	}

	player->ResultInit();

	auto rigid = player->AddComponent<CRigidBodyComponent>();

	if (rigid)
	{
		rigid->SetCollisionFilter(
			COLLISION_GROUP_PLAYER,
			COLLISION_GROUP_STAGE |
			COLLISION_GROUP_GHOST |
			COLLISION_GROUP_ENEMY |
			COLLISION_GROUP_CAMERA
		);

		rigid->SetColliderOffset({ 0.0f, 50.0f, 0.0f });

		rigid->CreateCapsule(15.5f, 45.5f, 0.8f);

		rigid->SetDynamic(false);

		// 回転同期しない
		rigid->SetSyncRotation(false);

		// 回転因子の設定
		rigid->SetAngularFactor(btVector3(0.0f, 0.0f, 0.0f));

		rigid->Transform();
	}
}

//=============================================================================
// スコア生成
//=============================================================================
void CResult::SpawnScore(const PlayerRanking& ranking)
{
	// スコアゲームオブジェクトの生成
	auto scoreObject = CreateObject<CGameObject>();

	scoreObject->SetTag("Score");

	// UIコンポーネント
	auto ui = scoreObject->AddComponent<CUIComponent>();

	// スコア描画コンポーネント
	auto scoreRender = scoreObject->AddComponent<CScoreRenderComponent>();

	ui->SetPosition(m_ScorePositions[ranking.rank - 1]);
	ui->SetScale({ 2.0f, 2.0f });

	scoreRender->SetLayer(RenderLayer::UI);
	scoreRender->SetTexture("data/TEXTURE/Num_01.png");
	scoreRender->SetDigitSize(0.02f, 0.04f);
	scoreRender->SetCountSpeed(150.0f);
	scoreRender->SetScore(ranking.score);
}

//=============================================================================
// PlayerIDからプレイヤーを検索
//=============================================================================
CPlayer* CResult::FindPlayerById(uint8_t playerId)
{
	auto players = FindGameObjects<CPlayer>();

	for (auto player : players)
	{
		if (!player)
		{
			continue;
		}

		if (player->GetPlayerId() == playerId)
		{
			return player;
		}
	}

	return nullptr;
}

//=============================================================================
// 順位計算
//=============================================================================
void CResult::CalculateRanking(void)
{
	auto network = CManager::Instance()->GetNetworkManager();

	if (!network || !network->IsPlayerScoresReceived())
	{
		return;
	}

	auto scores = network->GetPlayerScores();

	m_Rankings.clear();

	for (const auto& score : scores)
	{
		PlayerRanking ranking{};

		ranking.playerId = score.playerId;
		ranking.score = score.score;

		m_Rankings.push_back(ranking);
	}

	// スコアの高い順
	std::sort(
		m_Rankings.begin(),
		m_Rankings.end(),
		[](const PlayerRanking& a, const PlayerRanking& b)
		{
			if (a.score != b.score)
				return a.score > b.score;

			return a.playerId < b.playerId;
		}
	);

	// 順位設定
	for (size_t i = 0; i < m_Rankings.size(); ++i)
	{
		m_Rankings[i].rank = static_cast<uint8_t>(i + 1);
	}
}

//=============================================================================
// デバイスリセット通知
//=============================================================================
void CResult::OnDeviceReset(void)
{
	// ライトの再設定処理
	CLightManager::Instance()->RestoreLights();
}