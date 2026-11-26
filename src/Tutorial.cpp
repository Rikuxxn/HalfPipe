//=============================================================================
//
// チュートリアル処理 [Tutorial.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Tutorial.h"
#include "Manager.h"
#include "Result.h"
#include "Input.h"
#include "InputManager.h"
#include "Fade.h"
#include "SceneManager.h"
#include "TransformComponent.h"
#include "SpriteRenderComponent.h"
#include "GameObject.h"
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
#include "ScoreRenderComponent.h"
#include "UIAnimationManager.h"
#include "UIFactory.h"
#include "UIAnimationComponent.h"
#include "TimeSystem.h"
#include <StageObjectList.h>
#include "TutorialState.h"
#include <UITextComponent.h>


//=============================================================================
// コンストラクタ
//=============================================================================
CTutorial::CTutorial() :
	m_pPointUI(nullptr),
	m_nReadyCount(0),
	m_fCount(3.0f),
	m_IsGameStart(false)
{
	// 値のクリア
}
//=============================================================================
// デストラクタ
//=============================================================================
CTutorial::~CTutorial()
{
	// なし
}
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CTutorial::Init(void)
{
	// ステージマネージャの初期化
	auto stage = CManager::Instance()->GetStageManager();
	stage->Init();

	// トリガーオブジェクトマネージャの初期化
	auto trigger = CManager::Instance()->GetTriggerObjectManager();
	trigger->Init();

	// ステージオブジェクトの読み込み
	CManager::Instance()->GetStageManager()->LoadFromJson("data/StageInfo/Tutorial_Stage.json");

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
		score->SetCountSpeed(150.0f);
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

	auto textObject = CreateObject();

	auto uiText = textObject->AddComponent<CUIComponent>();
	auto uiSprite = textObject->AddComponent<CSpriteRenderComponent>();

	auto& uiTransform = uiText->GetTransform();
	uiTransform.Position = { 0.5f,0.5f };
	uiTransform.Scale = { 1.1f,1.1f };

	auto text = textObject->AddComponent<CUITextComponent>();

	text->SetText("こんにちは。\n"
		"「<orange>aaa</orange>」sdsdfs「<dodgerblue>あああ</dodgerblue>」");
	text->ShowAll();
	text->SetTypingSpeed(0.05f);

	// インスタンスのポインタを渡す
	m_stateMachine.Start(this);

	// 初期状態のステートをセット
	m_stateMachine.ChangeState<CTutorialWaitState>();

	// カメラの取得
	CCamera* pCamera = CManager::Instance()->GetCamera();

#ifdef _DEBUG
	// カメラの初期位置を設定しておく
	pCamera->SetCamMode(CCamera::MODE_EDIT);
#else
	// カメラの初期位置を設定しておく
	pCamera->SetCamMode(CCamera::MODE_THIRD_PERSON);
#endif

	return S_OK;
}
//=============================================================================
// 終了処理
//=============================================================================
void CTutorial::Uninit(void)
{
	// シーンの終了処理
	CScene::Uninit();

	// ステージマネージャの初期化
	auto stage = CManager::Instance()->GetStageManager();
	stage->Reset();

	// トリガーオブジェクトマネージャの初期化
	auto trigger = CManager::Instance()->GetTriggerObjectManager();
	trigger->Reset();

	//// ジョイパッドの取得
	//CInputJoypad* pJoypad = CManager::GetInputJoypad();

	//// 振動停止
	//pJoypad->StopVibration();
}
//=============================================================================
// 更新処理
//=============================================================================
void CTutorial::Update(void)
{
	CFade* pFade = CManager::Instance()->GetFade();
	CInputKeyboard* pKeyboard = CManager::Instance()->GetInput()->GetKeyboard();
	CInputJoypad* pJoypad = CManager::Instance()->GetInput()->GetJoypad();

#ifdef _DEBUG
	if (pFade->GetFade() == CFade::FADE_NONE &&
		pKeyboard->GetTrigger(DIK_T))
	{
		// ゲーム画面に移行
		pFade->StartFadeOut(Mode::Game);
	}
#endif

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

	// シーンの更新処理
	CScene::Update();

	//if (network)
	//{
	//	CNetworkManager::PointCircleScoredInfo info{};

	//	while (network->PopPointCircleScored(info))
	//	{
	//		// バスケットを取得
	//		auto scene = CManager::Instance()->GetSceneManager()->GetScene();

	//		if (!scene)
	//		{
	//			continue;
	//		}

	//		// シーン内のすべてのBasketを取得
	//		auto baskets = scene->FindGameObjects<CBasketBlock>();

	//		// 投げ込んだかごからサークルを削除
	//		for (auto basket : baskets)
	//		{
	//			if (basket && basket->GetBasketId() == info.BasketId)
	//			{
	//				basket->RemovePointCircleById(info.CircleId);
	//				break;
	//			}
	//		}
	//	}
	//}

	// デルタタイムの取得
	float dt = CManager::Instance()->GetTimeSystem()->GetDeltaTime();

	if (network && network->IsGameFinish())
	{
		m_fCount -= dt;

		if (pFade->GetFade() == CFade::FADE_NONE && m_fCount <= 0.0f)
		{
			m_fCount = 0.0f;

			// ゲーム画面に移行
			pFade->StartFadeOut(Mode::Game);
		}
	}

	// スキップ
	if (network->IsJoined())
	{
		// 全員が準備完了になったら
		if (network->IsGameReady())
		{
			m_fCount -= dt;

			if (pFade->GetFade() == CFade::FADE_NONE && m_fCount <= 0.0f)
			{
				m_fCount = 0.0f;

				// ゲーム画面に移行
				pFade->StartFadeOut(Mode::Game);
			}
		}
	}

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
				transform->Rotation = { state.rotX, state.rotY,	state.rotZ };
				transform->Quaternion = { state.quatX, state.quatY, state.quatZ, state.quatW };
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
void CTutorial::Draw(void)
{
	// シーンの描画処理
	CScene::Draw();
}

//=============================================================================
// プレイヤー生成
//=============================================================================
void CTutorial::SpawnPlayer(uint8_t playerId)
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
		-133.0f,
		598.0f,
		-1998.0f
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

	player->TutorialInit();

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
CPlayer* CTutorial::FindPlayerById(uint8_t playerId)
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
