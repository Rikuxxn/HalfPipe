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


	////============================================================
	//// Player生成(クライアント)
	////============================================================
	//{
	//	auto network = CManager::Instance()->GetNetworkManager();

	//	if (network)
	//	{
	//		// サーバーから受け取った参加プレイヤー一覧を取得
	//		const auto& playerIds = network->GetActivePlayerIds();

	//		for (uint8_t id : playerIds)
	//		{
	//			SpawnPlayer(id);
	//		}
	//	}
	//}

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
void CResult::SpawnPlayer(uint8_t playerId)
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
// デバイスリセット通知
//=============================================================================
void CResult::OnDeviceReset(void)
{
	// ライトの再設定処理
	CLightManager::Instance()->RestoreLights();
}