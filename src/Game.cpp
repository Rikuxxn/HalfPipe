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



//=============================================================================
// コンストラクタ
//=============================================================================
CGame::CGame()
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
		//auto network = CManager::Instance()->GetNetworkManager();

		//if (network)
		{
			//for (uint8_t i = 0; i < Network::MAX_PLAYERS; ++i)
			{
				auto player = CreateObject<CPlayer>();

				player->SetTag("Player");

				//player->SetPlayerId(static_cast<uint8_t>(i));

				auto transform = player->AddComponent<CTransformComponent>();

				//transform->Position =
				//{
				//	static_cast<float>(/*i * 60*/ - 90),
				//	0.0f,
				//	0.0f
				//};

				transform->Position =
				{
					-400.0f,
					580.0f,
					-1330.0f
				};


				auto character = player->AddComponent<CCharacterComponent>();

				character->Load("data/MOTION/motion_player.txt");

				player->Init();

				//// 自動移動ポイントの読み込み
				//player->LoadMovePoint("data/MovePointInfo.json");

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

					rigid->SetDynamic(true);

					// 回転同期しない
					rigid->SetSyncRotation(false);

					// 回転因子の設定
					rigid->SetAngularFactor(btVector3(0.0f, 0.0f, 0.0f));
				}

				player->SetNetworkControlled(true);
			}
		}
	}

	// スコアの生成
	{
		auto scoreObject = CreateObject<CGameObject>();

		scoreObject->SetTag("Score");

		// UIコンポーネント
		auto ui = scoreObject->AddComponent<CUIComponent>();

		// 表示位置
		UITransform transform = ui->GetTransform();

		transform.Position = { 0.8f, 0.15f };
		transform.Scale = { 0.2f, 0.2f };

		// スコア描画コンポーネント
		auto score = scoreObject->AddComponent<CScoreRenderComponent>();

		score->SetLayer(RenderLayer::UI);
		score->SetTexture("data/TEXTURE/num_01.png");
		score->SetDigitSize(40.0f, 60.0f);
		score->SetCountSpeed(100.0f);
		score->SetScore(0);
	}

	// カメラの取得
	CCamera* pCamera = CManager::Instance()->GetCamera();

	// カメラの初期位置を設定しておく
	pCamera->SetCamMode(CCamera::MODE_EDIT);

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

#ifdef _DEBUG
	// テスト : スコア加算
	if (pKeyboard->GetTrigger(DIK_I))
	{
		auto score = FindGameObject("Score");

		if (score)
		{
			auto scoreRender = score->GetComponent<CScoreRenderComponent>();

			// スコアを加算する
			scoreRender->AddScore(100);
		}
	}
#endif

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

	// シーンの更新処理
	CScene::Update();

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

				transform->Position = { state.posX, state.posY,	state.posZ };

				transform->Rotation.y = state.rotY;

				player->UpdateNetworkMotion(state.isMoving != 0);
			}
		}
	}

	if (pFade->GetFade() == CFade::FADE_NONE &&
		(pJoypad->GetTrigger(CInputJoypad::JOYKEY_A) || pKeyboard->GetTrigger(DIK_RETURN)))
	{
		// リザルト画面に移行
		pFade->StartFadeOut(Mode::Result);
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
CPauseManager* CGame::GetPauseManager(void)
{
	return m_PauseManager.get();
}