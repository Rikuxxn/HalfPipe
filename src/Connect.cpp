//=============================================================================
//
// タイトル処理 [Title.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Connect.h"
#include "Input.h"
#include "Manager.h"
#include "Parameter.h"
#include "InputManager.h"
#include "Camera.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "SpriteRenderComponent.h"
#include "TextureManager.h"
#include "SceneManager.h"
#include "Fade.h"
#include "LightComponent.h"
#include "CharacterComponent.h"
#include "Player.h"
#include "ParticleEmitterComponent.h"
#include "MeshRenderComponent.h"
#include "MeshResource.h"
#include "XModelRenderComponent.h"
#include "RigidBodyComponent.h"
#include "StageManager.h"
#include "TriggerObjectManager.h"
#include "Sky.h"
#include "ModelManager.h"
#include "GameTime.h"
#include "EditCamera.h"
#include "ThirdPersonCamera.h"
#include "CollisionGroup.h"
#include "SoundManager.h"
#include "StageEditor.h"
#include "TriggerObjectEditor.h"
#include "NetWorkManager.h"



//=============================================================================
// コンストラクタ
//=============================================================================
CConnect::CConnect() : 
	m_JoinRequestSent	(false),
	m_PlayerSpawned		(false)
{
	// 値のクリア
}
//=============================================================================
// デストラクタ
//=============================================================================
CConnect::~CConnect()
{
	// なし
}
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CConnect::Init(void)
{
	// マウスカーソルを表示する
	CManager::Instance()->GetInput()->GetMouse()->SetCursorVisibility(true);

	//-----------------------------------------------------
	// マネージャーの初期化
	//-----------------------------------------------------
	{
		// ステージマネージャの初期化
		auto stage = CManager::Instance()->GetStageManager();
		stage->Init();

		// トリガーオブジェクトマネージャの初期化
		auto trigger = CManager::Instance()->GetTriggerObjectManager();
		trigger->Init();

		// ステージオブジェクトの読み込み
		CManager::Instance()->GetStageManager()->LoadFromJson("data/StageInfo/Connect_Stage.json");

		//// トリガーオブジェクトの読み込み
		//CManager::Instance()->GetTriggerObjectManager()->LoadFromJson("data/TriggerInfo/test_trigger.json");
	}

	// ゲーム時間の取得
	auto time = CManager::Instance()->GetGameTime();

	// ゲーム時間をリセット
	time->Reset();

	// 時間を任意の時間で停止する
	time->Stop();
	time->SetTime(25.0f);

	// 空の生成
	m_pSky = std::make_unique<CSky>();
	m_pSky->Init();

	//// 2Dパーティクルオブジェクトの生成
	//auto particle2D = CreateObject();

	//particle2D->SetTag("Particle2D");

	//auto particle2DTransform = particle2D->AddComponent<CTransformComponent>();

	//particle2DTransform->Position = { 500.0f, 200.0f, 0.0f };

	//auto emitter2D = particle2D->AddComponent<CParticleEmitterComponent>();

	//// 描画順の設定
	//emitter2D->SetLayer(RenderLayer::Particle2D);

	//// 放出空間の設定
	//emitter2D->Space = ParticleSpace::Space2D;

	//// エミッターのセッターにテクスチャを設定
	//emitter2D->SetTexture("data/TEXTURE/effect000.jpg");

	//emitter2D->col = { 0.0f, 1.0f, 1.0f, 1.0f };

	//emitter2D->EmitMode =	ParticleEmitMode::Loop;

	//emitter2D->SpawnShape = ParticleSpawnShape::Box;

	//emitter2D->Radius = 10;

	//emitter2D->EmitRate = 0;

	//emitter2D->LifeTimeMin = 30;
	//emitter2D->LifeTimeMax = 60;

	//emitter2D->SpeedMin = 1;
	//emitter2D->SpeedMax = 5;

	//emitter2D->SizeMin = 10;
	//emitter2D->SizeMax = 20;



	// 3D雪パーティクルオブジェクトの生成
	auto snow = CreateObject();

	snow->SetTag("SnowParticle3D");

	auto snowTransform = snow->AddComponent<CTransformComponent>();

	snowTransform->Position = { 0.0f, 300.0f, 0.0f };

	auto snow3D = snow->AddComponent<CParticleEmitterComponent>();

	snow3D->SetLayer(RenderLayer::Particle3D);

	// 放出空間の設定
	snow3D->Space = ParticleSpace::Space3D;

	// エミッターのセッターにテクスチャを設定
	snow3D->SetTexture("data/TEXTURE/effect000.jpg");

	// シェーダーの設定
	snow3D->SetShader("data/Shader/Instancing/ParticleInstancing.fx");

	snow3D->col = { 1.0f, 1.0f, 1.0f, 1.0f };
	snow3D->BoxSize = { 600.0f, 100.0f, 600.0f };
	snow3D->EmitMode = ParticleEmitMode::Loop;

	// パーティクルの種類の設定
	snow3D->Type = ParticleType::Snow;

	snow3D->SpawnShape = ParticleSpawnShape::Box;
	snow3D->Radius = 5.0f;
	snow3D->EmitRate = 1;
	snow3D->LifeTimeMin = 100;
	snow3D->LifeTimeMax = 300;
	snow3D->SizeMin = 5;
	snow3D->SizeMax = 10;



	//// メッシュフィールドオブジェクトの生成
	//auto meshField = CreateObject();

	//meshField->SetTag("MeshField");

	//auto meshFieldTransform = meshField->AddComponent<CTransformComponent>();

	//meshFieldTransform->Position = { 0.0f, 0.0f, 0.0f };

	//auto renderer = meshField->AddComponent<CMeshRenderComponent>();

	//// 描画順の設定
	//renderer->SetLayer(RenderLayer::Mesh);

	//// メッシュ生成
	//auto mesh = std::make_shared<CMeshResource>();

	//// メッシュフィールドの生成
	//mesh->CreatePlane(800.0f, 800.0f, 100, 100);

	//// ポインタ設定
	//renderer->SetMesh(mesh);

	//// テクスチャの設定
	//renderer->SetTexture("data/TEXTURE/field100.jpg");



	// カメラの取得
	CCamera* pCamera = CManager::Instance()->GetCamera();

	// カメラの初期位置を設定しておく
	pCamera->SetCamMode(CCamera::MODE_EDIT);

	//// サウンドマネージャーの取得
	//auto sound = CManager::Instance()->GetSoundManager();

	//sound->PlayBGM(CSoundManager::SOUND_LABEL_TITLEBGM);

	return S_OK;
}
//=============================================================================
// 終了処理
//=============================================================================
void CConnect::Uninit(void)
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

	// モデルマネージャーの破棄
	CModelManager::GetInstance().Release();
}
//=============================================================================
// 更新処理
//=============================================================================
void CConnect::Update(void)
{
	auto network = CManager::Instance()->GetNetworkManager();

	CFade* pFade = CManager::Instance()->GetFade();
	CInputKeyboard* pKeyboard = CManager::Instance()->GetInput()->GetKeyboard();
	CInputJoypad* pJoypad = CManager::Instance()->GetInput()->GetJoypad();

	//if (pFade->GetFade() == CFade::FADE_NONE &&
	//	(pKeyboard->GetTrigger(DIK_RETURN) || pJoypad->GetTrigger(CInputJoypad::JOYKEY_A)))
	//{
	//	// ゲーム画面に移行
	//	pFade->StartFadeOut(Mode::Game);
	//}

#ifdef _DEBUG
	if (pFade->GetFade() == CFade::FADE_NONE &&
		pKeyboard->GetTrigger(DIK_T))
	{
		// ゲーム画面に移行
		pFade->StartFadeOut(Mode::Game);
	}
#endif

	// シーンの更新処理
	CScene::Update();

	//============================================================
	// Client：Serverから受信したPlayer状態を反映
	//============================================================
	{
		if (network && network->IsConnected())
		{
			for (uint8_t playerId = 0;
				playerId < Network::MAX_PLAYERS;
				++playerId)
			{
				Network::PlayerState state{};

				// サーバーから状態を受信していないIDは処理しない
				if (!network->GetPlayerState(playerId, state))
				{
					continue;
				}

				// Snapshotに含まれるPlayerIDと配列添字を確認
				if (state.playerId != playerId)
				{
					continue;
				}

				// 自分のプレイヤーは参加確定後の処理で生成する
				if (network->IsJoined() &&
					playerId == network->GetLocalPlayerId())
				{
					continue;
				}

				// まだ存在しないプレイヤーを生成
				SpawnPlayer(playerId);

				// 生成されたプレイヤーを取得
				CPlayer* player = FindPlayerById(playerId);

				if (!player)
				{
					continue;
				}

				// 自分のプレイヤーはローカル側で制御
				if (player->IsLocalPlayer())
				{
					continue;
				}

				auto transform = player->GetComponent<CTransformComponent>();

				if (!transform)
				{
					continue;
				}

				// 相手の位置・回転を反映
				transform->Position =
				{
					state.posX,
					state.posY,
					state.posZ
				};

				transform->Rotation.y = state.rotY;

				player->UpdateNetworkMotion(state.isMoving != 0);
			}
		}
	}

	if (!network)
	{
		return;
	}

	// サーバー接続前は参加要求を送らない
	if (!network->IsConnected())
	{
		// ここで「サーバー接続中...」を表示
		return;
	}

	//============================================================
	// 切断したプレイヤーを削除
	//============================================================
	if (network)
	{
		uint8_t playerId = 255;

		while (network->ConsumePlayerLeft(playerId))
		{
			CPlayer* player = FindPlayerById(playerId);

			if (player)
			{
				player->Destroy();
			}
		}
	}

	// 参加確定後はゲームシーンへの遷移を待つ
	if (network->IsJoined())
	{
		// 参加確定直後にプレイヤーを生成
		if (!m_PlayerSpawned)
		{
			// 自分のプレイヤーを生成
			const uint8_t localPlayerId = network->GetLocalPlayerId();

			SpawnPlayer(localPlayerId);

			// 生成済みにする 
			m_PlayerSpawned = true;
		}

		// 最大人数がそろったら遷移
		if (network->IsGameReady())
		{
			//if (pFade->GetFade() == CFade::FADE_NONE)
			//{
			//	// ゲーム画面に移行
			//	pFade->StartFadeOut(Mode::Game);
			//}
		}

		return;
	}

	// 参加要求は1回だけ送信
	if (!m_JoinRequestSent)
	{
		if (pKeyboard->GetTrigger(DIK_RETURN) ||
			pJoypad->GetTrigger(CInputJoypad::JOYKEY_A))
		{
			D3DXVECTOR3 spawnPos =
				GetSpawnPosition(network->GetLocalPlayerId());

			network->SendJoinRequest(
				spawnPos.x,
				spawnPos.y,
				spawnPos.z);

			m_JoinRequestSent = true;
		}
	}
}
//=============================================================================
// 描画処理
//=============================================================================
void CConnect::Draw(void)
{
	// シーンの描画処理
	CScene::Draw();

	//// カメラの取得
	//CCamera* pCamera = CManager::Instance()->GetCamera();

	//// カメラの設定
	//pCamera->SetPosV({ 0.0f, 680.0f, -2050.0f });
	//pCamera->SetPosR({ 0.0f, 680.0f, -2350.0f });
	//pCamera->SetRot({ 0.0f, 0.0f, 0.0f });
}

//=============================================================================
// PlayerIDからプレイヤーを検索
//=============================================================================
CPlayer* CConnect::FindPlayerById(uint8_t playerId)
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
// プレイヤー生成
//=============================================================================
void CConnect::SpawnPlayer(uint8_t playerId)
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
		static_cast<float>(playerId * 60.0f - 30.0f),
		730.0f,
		-2250.0f
	};

	// キャラクター
	auto character = player->AddComponent<CCharacterComponent>();

	character->Load("data/MOTION/motion_player.txt");

	player->ConnectInit();

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