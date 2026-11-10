//=============================================================================
//
// ゲーム処理 [Game.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Game.h"
#include "Manager.h"
#include "Result.h"
#include "Input.h"
#include "InputManager.h"
#include "Fade.h"
#include "SceneManager.h"
#include "TransformComponent.h"
#include "SpriteRenderComponent.h"
#include "GameObject.h"
#include "PauseManager.h"
#include "LightManager.h"
#include "StageManager.h"
#include "StageEditor.h"
#include "TriggerObjectManager.h"
#include "TriggerObjectEditor.h"
#include "Sky.h"
#include "GameTime.h"
#include "SoundManager.h"
#include "CharacterComponent.h"
#include "RigidBodyComponent.h"
#include "RaycastComponent.h"
#include "CollisionGroup.h"
#include "Player.h"
#include "NetWorkManager.h"
#include "UIComponent.h"
#include "UIAnimationComponent.h"
#include "JumpTiming.h"
#include "Camera.h"
#include "JumpTimingRating.h"
#include "ScoreRenderComponent.h"
#include "UIAnimationManager.h"
#include "UIFactory.h"
#include "UIAnimationComponent.h"
#include "TimeSystem.h"


//=============================================================================
// コンストラクタ
//=============================================================================
CGame::CGame() :
	m_pPointUI		(nullptr),
	m_nReadyCount	(0),
	m_fCount		(3.0f),
	m_IsGameStart	(false)
{
	// 値のクリア
}
//=============================================================================
// デストラクタ
//=============================================================================
CGame::~CGame()
{
	// なし
}
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CGame::Init(void)
{
	// ポーズマネージャーの生成
	m_PauseManager = std::make_unique<CPauseManager>();

	// ポーズマネージャーの初期化処理
	m_PauseManager->Init(this);

	// ステージマネージャの初期化
	auto stage = CManager::Instance()->GetStageManager();
	stage->Init();

	// トリガーオブジェクトマネージャの初期化
	auto trigger = CManager::Instance()->GetTriggerObjectManager();
	trigger->Init();

	// ステージオブジェクトの読み込み
	CManager::Instance()->GetStageManager()->LoadFromJson("data/StageInfo/Game_Stage.json");

	// トリガーオブジェクトの読み込み
	CManager::Instance()->GetTriggerObjectManager()->LoadFromJson("data/TriggerInfo/Game_Trigger.json");

	// ゲーム時間の取得
	auto time = CManager::Instance()->GetGameTime();

	// ゲーム時間をリセット
	time->Reset();

	// 時間を任意の時間で停止する
	time->Stop();
	//time->Start();
	time->SetTime(30.0f);

	// 空の生成
	m_pSky = std::make_unique<CSky>();
	m_pSky->Init();

	// ジャンプタイミングUIの生成
	auto jump = CreateObject<CJumpTiming>();
	jump->Init();

	// ジャンプタイミングの評価UIの生成
	auto jumpTimingUI = CreateObject<CJumpTimingRating>();
	jumpTimingUI->Init();

	//============================================================
	// Player生成(クライアント)
	//============================================================
	{
		auto network = CManager::Instance()->GetNetworkManager();

		if (network)
		{
			// サーバーから受け取った参加プレイヤー一覧を取得
			const auto& playerIds = network->GetActivePlayerIds();

			for (uint8_t id : playerIds)
			{
				SpawnPlayer(id);

				m_nReadyCount++;
			}
		}
	}

	// スコアの生成
	{
		auto scoreObject = CreateObject<CGameObject>();

		scoreObject->SetTag("Score");

		// UIコンポーネント
		auto ui = scoreObject->AddComponent<CUIComponent>();

		// スコア描画コンポーネント
		auto score = scoreObject->AddComponent<CScoreRenderComponent>();

		ui->SetPosition({ 0.54f, 0.13f });
		ui->SetScale({ 2.0f, 2.0f });

		score->SetLayer(RenderLayer::UI);
		score->SetTexture("data/TEXTURE/Num_01.png");
		score->SetDigitSize(0.02f, 0.04f);
		score->SetCountSpeed(100.0f);
		score->SetScore(0);
	}

	//-----------------------------------------------------
	// JSONに保存したUI (「点」 UI)
	//-----------------------------------------------------
	{
		// UIアニメーションデータの読み込み
		auto uiManager = CManager::Instance()->GetUIAnimationManager();

		if (uiManager)
		{
			uiManager->Load("data/UIAnimation/Point.json");
		}

		// 登録済みのUIデータを取得
		UIAnimationData* data = uiManager->GetData("Point");

		// UI生成
		m_pPointUI = CUIFactory::Create(this, *data);

		// 最初は非表示
		auto sprite = m_pPointUI->GetComponent<CSpriteRenderComponent>();

		if (sprite)
		{
			sprite->SetIsVisible(true);
		}
	}

	// カメラの取得
	CCamera* pCamera = CManager::Instance()->GetCamera();

#ifdef _DEBUG
	// カメラの初期位置を設定しておく
	pCamera->SetCamMode(CCamera::MODE_THIRD_PERSON);
#else
	// カメラの初期位置を設定しておく
	pCamera->SetCamMode(CCamera::MODE_THIRD_PERSON);
#endif

	return S_OK;
}
//=============================================================================
// 終了処理
//=============================================================================
void CGame::Uninit(void)
{
	// ステージマネージャの初期化
	auto stage = CManager::Instance()->GetStageManager();
	stage->Reset();

	// トリガーオブジェクトマネージャの初期化
	auto trigger = CManager::Instance()->GetTriggerObjectManager();
	trigger->Reset();

	// シーンの終了処理
	CScene::Uninit();

	//// ジョイパッドの取得
	//CInputJoypad* pJoypad = CManager::GetInputJoypad();

	//// 振動停止
	//pJoypad->StopVibration();
}
//=============================================================================
// 更新処理
//=============================================================================
void CGame::Update(void)
{
	CFade* pFade = CManager::Instance()->GetFade();
	CInputKeyboard* pKeyboard = CManager::Instance()->GetInput()->GetKeyboard();
	CInputJoypad* pJoypad = CManager::Instance()->GetInput()->GetJoypad();

	// PでポーズON/OFF
	if (pKeyboard->GetTrigger(DIK_P) || pJoypad->GetTrigger(CInputJoypad::JOYKEY_START))
	{
		bool pause = !m_PauseManager->IsEnable();

		m_PauseManager->SetEnable(!m_PauseManager->IsEnable());
	
		// BGMの一時停止・再開 
		auto soundManager = CManager::Instance()->GetSoundManager(); 
		
		if (pause)
		{ 
			// ポーズ開始 
			soundManager->PauseAll(); 
		} 
		else 
		{ 
			// ポーズ解除 
			soundManager->ResumeAll();
		}
	}

	//---------------------------------
	// ポーズ中
	//---------------------------------

	if (m_PauseManager && m_PauseManager->IsEnable())
	{
		// ポーズマネージャーの更新
		m_PauseManager->Update();

		return;
	}

	// ネットワークの取得
	auto network = CManager::Instance()->GetNetworkManager();

	//============================================================
	// LocalPlayer判定
	//============================================================
	{
		if (network && network->IsConnected())
		{
			const uint8_t localPlayerId = network->GetLocalPlayerId();

			// WelcomePacketをまだ受信していない
			if (localPlayerId != 255)
			{
				auto players = FindGameObjects<CPlayer>();

				for (auto player : players)
				{
					if (!player)
					{
						continue;
					}

					player->SetLocalPlayer(player->GetPlayerId() == localPlayerId);
				}
			}
		}
	}

	////=============================================================================
	//// 参加プレイヤーの生成確認
	////=============================================================================
	//auto network = CManager::Instance()->GetNetworkManager();

	//if (network && network->IsConnected())
	//{
	//	const auto& playerIds = network->GetActivePlayerIds();

	//	for (uint8_t id : playerIds)
	//	{
	//		if (!FindPlayerById(id))
	//		{
	//			SpawnPlayer(id);
	//		}
	//	}
	//}

	// シーンの更新処理
	CScene::Update();

	// デルタタイムの取得
	float dt = CManager::Instance()->GetTimeSystem()->GetDeltaTime();

	if (network && network->IsGameFinish())
	{
		m_fCount -= dt;

		if (pFade->GetFade() == CFade::FADE_NONE && m_fCount <= 0.0f)
		{
			m_fCount = 0.0f;

			// リザルト画面に移行
			pFade->StartFadeOut(Mode::Result);
		}

		//return;
	}

	// 人数がそろって、一定時間経過したら
	//if (m_nReadyCount >= Network::MAX_PLAYERS && !m_IsGameStart)
	//{
	//	m_fCount -= dt;

	//	if (m_fCount <= 0.0f)
	//	{
	//		m_fCount = 3.0f;
	//		m_IsGameStart = true;

	//		// プレイヤーの自動移動を開始
	//		auto players = FindGameObjects<CPlayer>();

	//		for (auto player : players)
	//		{
	//			if (!player)
	//			{
	//				continue;
	//			}

	//			if (!player->IsLocalPlayer())
	//			{
	//				continue;
	//			}

	//			// 自動移動開始
	//			player->SetIsAutoMove(true);
	//		}
	//	}
	//}

	//============================================================
	// Client：Serverから受信したPlayer状態を反映
	//============================================================
	{
		if (network && network->IsConnected())
		{
			auto players = FindGameObjects<CPlayer>();

			for (auto player : players)
			{
				if (!player)
				{
					continue;
				}

				// 自分のPlayerは自分で操作する
				if (player->IsLocalPlayer())
				{
					continue;
				}

				uint8_t playerId = player->GetPlayerId();

				if (playerId >= Network::MAX_PLAYERS)
				{
					continue;
				}

				Network::PlayerState state{};

				if (!network->GetPlayerState(playerId, state))
				{
					continue;
				}

				auto transform = player->GetComponent<CTransformComponent>();

				if (!transform)
				{
					continue;
				}

				transform->Position		= { state.posX, state.posY,	state.posZ };
				transform->Rotation		= { state.rotX, state.rotY,	state.rotZ };
				transform->Quaternion	= { state.quatX, state.quatY, state.quatZ, state.quatW };
				transform->UseQuaternion = true;

				auto rigid = player->GetComponent<CRigidBodyComponent>();

				if (rigid)
				{
					rigid->Transform();
				}

				player->UpdateNetworkMotion(state.isMoving != 0);
			}
		}
	}
}
//=============================================================================
// 描画処理
//=============================================================================
void CGame::Draw(void)
{
	// シーンの描画処理
	CScene::Draw();

//#ifdef _DEBUG
//	// グリッドの描画
//	m_pGrid->Draw();
//#endif
//	// ポーズ中だったら
//	if (m_isPaused)
//	{
//		// ポーズマネージャーの描画処理
//		m_pPauseManager->Draw();
//	}
}
//=============================================================================
// デバイスリセット通知
//=============================================================================
void CGame::OnDeviceReset(void)
{
	// ライトの再設定処理
	CLightManager::Instance()->RestoreLights();
}
//=============================================================================
// ポーズの設定
//=============================================================================
void CGame::SetEnablePause(bool bPause)
{
	if (m_PauseManager)
	{
		m_PauseManager->SetEnable(bPause);
	}

	if (bPause)
	{
		//// 音を一時停止
		//CManager::GetSound()->PauseAll();
	}
	else
	{
		//// 音を再開
		//CManager::GetSound()->ResumeAll();
	}
}

//=============================================================================
// プレイヤー生成
//=============================================================================
void CGame::SpawnPlayer(uint8_t playerId)
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

	transform->Position =
	{
		-400.0f,
		580.0f,
		-1330.0f
	};

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

	player->Init();

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
// PlayerIDからプレイヤーを検索
//=============================================================================
CPlayer* CGame::FindPlayerById(uint8_t playerId)
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

CPauseManager* CGame::GetPauseManager(void)
{
	return m_PauseManager.get();
}