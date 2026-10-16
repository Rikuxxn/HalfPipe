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
	//// ディレクショナルライトの生成
	//CreateDirectionalLight({ 0.0f, -1.0f, 0.0f }, D3DXCOLOR(1, 1, 1, 1));

	//// ディレクショナルライトの生成
	//CreateDirectionalLight({ 1.0f, 0.0f, 0.0f }, D3DXCOLOR(1, 1, 1, 1));

	//// ディレクショナルライトの生成
	//CreateDirectionalLight({ -1.0f, 0.0f, 0.0f }, D3DXCOLOR(1, 1, 1, 1));

	//// ディレクショナルライトの生成
	//CreateDirectionalLight({ 0.0f, 0.0f, 1.0f }, D3DXCOLOR(1, 1, 1, 1));

	//// ディレクショナルライトの生成
	//CreateDirectionalLight({ 0.0f, 0.0f, -1.0f }, D3DXCOLOR(1, 1, 1, 1));

	// ポーズマネージャーの生成
	m_PauseManager = std::make_unique<CPauseManager>();

	// ポーズマネージャーの初期化処理
	m_PauseManager->Init(this);

	// ステージマネージャの初期化
	auto stage = CManager::Instance()->GetStageManager();
	stage->Init();
	//stage->GetStageEditor()->Init();

	// トリガーオブジェクトマネージャの初期化
	auto trigger = CManager::Instance()->GetTriggerObjectManager();
	trigger->Init();
	//trigger->GetTriggerObjectEditor()->Init();

	// ステージオブジェクトの読み込み
	CManager::Instance()->GetStageManager()->LoadFromJson("data/StageInfo/Game_Stage.json");

	// トリガーオブジェクトの読み込み
	CManager::Instance()->GetTriggerObjectManager()->LoadFromJson("data/TriggerInfo/test_trigger.json");

	// ゲーム時間の取得
	auto time = CManager::Instance()->GetGameTime();

	// ゲーム時間をリセット
	time->Reset();

	// 空の生成
	m_pSky = std::make_unique<CSky>();
	m_pSky->Init();

	auto jump = CreateObject<CJumpTiming>();
	jump->Init();

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

	////---------------------------------------------------
	//// アニメーション付きUIの生成
	////---------------------------------------------------
	//{
	//	// タイトルロゴオブジェクトの生成
	//	auto logo = CreateObject();

	//	logo->SetTag("TitleLogo");

	//	// UIコンポーネント
	//	auto ui = logo->AddComponent<CUIComponent>();

	//	auto& uiTransform = ui->GetTransform();
	//	uiTransform.Position = { 0.5f, 0.5f };
	//	uiTransform.Alpha = 1.0f;
	//	uiTransform.Scale = { 1.0f, 1.0f };

	//	// スプライトコンポーネントに追加
	//	auto sprite = logo->AddComponent<CSpriteRenderComponent>();

	//	// 描画順の設定
	//	sprite->SetLayer(RenderLayer::UI);

	//	// テクスチャの設定
	//	sprite->SetTexture("data/TEXTURE/ui_complete.png");

	//	// サイズの設定
	//	sprite->SetSize(500.0f, 300.0f);

	//	// UIアニメーションコンポーネント
	//	auto animation = logo->AddComponent<CUIAnimationComponent>();

	//	// フェードイン
	//	UIAnimationClip fadeIn;
	//	fadeIn.Property = UIProperty::Alpha;
	//	fadeIn.From = 0.0f;
	//	fadeIn.To = 1.0f;
	//	fadeIn.Duration = 1.0f;
	//	fadeIn.Easing = CEasing::Linear;
	//	fadeIn.LoopCount = 0;
	//	fadeIn.PlaybackMode = UIPlaybackMode::PingPong;

	//	// アニメーションの追加
	//	animation->AddAnimation(fadeIn);

	//	// 下から移動
	//	UIAnimationClip moveUp;
	//	moveUp.Property = UIProperty::PositionY;
	//	moveUp.From = 0.65f;
	//	moveUp.To = 0.5f;
	//	moveUp.Duration = 1.0f;
	//	moveUp.Delay = 0.5f;
	//	moveUp.LoopCount = 0;
	//	moveUp.Easing = CEasing::EaseOutCubic;
	//	moveUp.PlaybackMode = UIPlaybackMode::PingPong;

	//	// アニメーションの追加
	//	animation->AddAnimation(moveUp);

	//	// 拡大アニメーション
	//	UIAnimationClip scaleUp;
	//	scaleUp.Property = UIProperty::ScaleX;
	//	scaleUp.From = 0.5f;
	//	scaleUp.To = 1.0f;
	//	scaleUp.Duration = 2.0f;
	//	scaleUp.Delay = 0.5f;
	//	scaleUp.LoopCount = 0;
	//	scaleUp.Easing = CEasing::EaseOutCubic;
	//	scaleUp.PlaybackMode = UIPlaybackMode::PingPong;

	//	// アニメーションの追加
	//	animation->AddAnimation(scaleUp);

	//	// アニメーション再生
	//	animation->Play();
	//}

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
// ライト更新処理
//=============================================================================
void CGame::UpdateLight(void)
{
	//float progress = m_pTime->GetProgress(); // 0.0～0.1

	//// 各時間帯のメインライト色
	//D3DXCOLOR mainColor;

	//// 時間帯ごとに補間
	//if (progress < CTime::NIGHT_START_RATE)
	//{// 夕方
	//	float t = progress / CTime::NIGHT_START_RATE;
	//	D3DXColorLerp(&mainColor, &LightParam::EVENING_COLOR, &LightParam::NIGHT_COLOR, t);
	//}
	//else if (progress < CTime::NIGHT_END_RATE)
	//{// 夜
	//	float t = (progress - CTime::NIGHT_START_RATE) / (CTime::NIGHT_END_RATE - CTime::NIGHT_START_RATE);
	//	D3DXColorLerp(&mainColor, &LightParam::NIGHT_COLOR, &LightParam::MORNING_COLOR, t);
	//}
	//else
	//{// 明け方
	//	float t = (progress - CTime::NIGHT_END_RATE) / (1.0f - CTime::NIGHT_END_RATE);
	//	D3DXColorLerp(&mainColor, &LightParam::MORNING_COLOR, &LightParam::EVENING_COLOR, t);
	//}

	//// 光の向き補間
	//D3DXVECTOR3 mainDir;

	//if (progress < LightParam::DAY_HALF_RATE)
	//{
	//	float t = progress / LightParam::DAY_HALF_RATE;
	//	D3DXVec3Lerp(&mainDir, &LightParam::DIR_EVENING, &LightParam::DIR_NIGHT, t);
	//}
	//else
	//{
	//	float t = (progress - LightParam::DAY_HALF_RATE) / LightParam::DAY_HALF_RATE;
	//	D3DXVec3Lerp(&mainDir, &LightParam::DIR_NIGHT, &LightParam::DIR_MORNING, t);
	//}

	//// 正規化
	//D3DXVec3Normalize(&mainDir, &mainDir);

	//// 再設定
	//CLight::Uninit();

	//m_pBlockManager->UpdateLight();

	//// メインライト
	//CLight::AddLight(
	//	D3DLIGHT_DIRECTIONAL,
	//	mainColor,
	//	mainDir,
	//	INIT_VEC3
	//);

	//// サブライト
	//D3DXCOLOR skyColor;

	//if (progress < LightParam::DAY_HALF_RATE)
	//{
	//	D3DXColorLerp(&skyColor, &LightParam::SKY_EVENING, &LightParam::SKY_NIGHT, 
	//		progress / LightParam::DAY_HALF_RATE);
	//}
	//else
	//{
	//	D3DXColorLerp(&skyColor, &LightParam::SKY_NIGHT, &LightParam::SKY_MORNING, 
	//		(progress - LightParam::DAY_HALF_RATE) / LightParam::DAY_HALF_RATE);
	//}

	//CLight::AddLight(
	//	D3DLIGHT_DIRECTIONAL,
	//	skyColor,
	//	D3DXVECTOR3(0.0f, -1.0f, 0.0f),
	//	INIT_VEC3
	//);

	//// 補助光
	//float warmFactor = 1.0f - fabs(progress - LightParam::DAY_HALF_RATE) * 2.0f;
	//warmFactor = std::max(0.0f, warmFactor);

	//CLight::AddLight(
	//	D3DLIGHT_DIRECTIONAL,
	//	D3DXCOLOR(
	//		LightParam::SUB_BASE_R + LightParam::SUB_WARM_R/* * warmFactor*/,
	//		LightParam::SUB_G,
	//		LightParam::SUB_B,
	//		1.0f),
	//	LightParam::SUB_DIR,
	//	INIT_VEC3
	//);
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