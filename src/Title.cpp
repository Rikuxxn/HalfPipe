//=============================================================================
//
// タイトル処理 [Title.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Title.h"
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
#include "LightManager.h"
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
#include "RaycastComponent.h"
#include "UIAnimationManager.h"
#include "UIFactory.h"
#include "UIAnimationComponent.h"
#include "TimeSystem.h"


//*****************************************************************************
// 名前空間
//*****************************************************************************
namespace TitleWorld
{
	const D3DXVECTOR3 PLAYER_POS{ 300.0f, 110.0f, 450.0f };
	const D3DXVECTOR3 PLAYER_ROT{ 0.0f, 90.0f, 0.0f };
}

namespace Particle
{
	const D3DXCOLOR COLOR{ 0.8f, 0.8f, 0.8f, 0.8f };
	constexpr int NUM = 1;
}

//=============================================================================
// コンストラクタ
//=============================================================================
CTitle::CTitle()
{
	// 値のクリア
}
//=============================================================================
// デストラクタ
//=============================================================================
CTitle::~CTitle()
{
	// なし
}
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CTitle::Init(void)
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
		CManager::Instance()->GetStageManager()->LoadFromJson("data/StageInfo/Title_Stage.json");

		//// トリガーオブジェクトの読み込み
		//CManager::Instance()->GetTriggerObjectManager()->LoadFromJson("data/TriggerInfo/test_trigger.json");
	}

	// ゲーム時間の取得
	auto time = CManager::Instance()->GetGameTime();

	// ゲーム時間をリセット
	time->Reset();

	// 任意の時間で停止する
	time->Stop();
	time->SetTime(20.0f);

	// 空の生成
	m_pSky = std::make_unique<CSky>();
	m_pSky->Init();

	//-----------------------------------------------------
	// JSONに保存したUIの使い方 (Press UI)
	//-----------------------------------------------------
	{
		// UIアニメーションデータの読み込み
		auto uiManager = CManager::Instance()->GetUIAnimationManager();

		if (uiManager)
		{
			uiManager->Load("data/UIAnimation/Title_Press.json");
		}

		// 登録済みのUIデータを取得
		UIAnimationData* data = uiManager->GetData("Press");

		// UI生成
		m_pPressUI = CUIFactory::Create(this, *data);

		// 最初は非表示
		auto sprite = m_pPressUI->GetComponent<CSpriteRenderComponent>();

		if (sprite)
		{
			sprite->SetIsVisible(true);
		}
	}

	//// プレイヤーの生成
	//{
	//	auto player = CreateObject<CPlayer>();

	//	// タグの設定
	//	player->SetTag("Player");

	//	auto playerTransform = player->AddComponent<CTransformComponent>();

	//	playerTransform->Position = { 0.0f, 0.0f, 0.0f };

	//	auto character = player->AddComponent<CCharacterComponent>();
	//	character->Load("data/MOTION/motion_player.txt");

	//	// 初期化処理
	//	player->Init();

	//	// リジッドボディコンポーネントの追加
	//	auto rigid = player->AddComponent<CRigidBodyComponent>();

	//	if (rigid)
	//	{
	//		// Stage用のCollision設定
	//		rigid->SetCollisionFilter(
	//			COLLISION_GROUP_PLAYER,
	//			COLLISION_GROUP_STAGE |
	//			COLLISION_GROUP_GHOST |
	//			COLLISION_GROUP_ENEMY |
	//			COLLISION_GROUP_CAMERA
	//		);

	//		// オフセット
	//		rigid->SetColliderOffset({ 0.0f, 50.0f, 0.0f });

	//		// カプセルコライダーの生成
	//		rigid->CreateCapsule(15.5f, 45.5f, 1.0f);

	//		// 動的
	//		rigid->SetDynamic(true);

	//		// 回転同期しない
	//		rigid->SetSyncRotation(false);

	//		// 回転因子の設定
	//		rigid->SetAngularFactor(btVector3(0.0f, 0.0f, 0.0f));
	//	}

	//	// レイキャストコンポーネントの追加
	//	player->AddComponent<CRaycastComponent>();
	//}

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


	// カメラの取得
	CCamera* pCamera = CManager::Instance()->GetCamera();

#ifdef _DEBUG
	// カメラの初期位置を設定しておく
	pCamera->SetCamMode(CCamera::MODE_EDIT);
#else
	// カメラの初期位置を設定しておく
	pCamera->SetCamMode(CCamera::MODE_FIXED);
#endif

	// サウンドマネージャーの取得
	auto sound = CManager::Instance()->GetSoundManager();

	sound->PlayBGM(CSoundManager::SOUND_LABEL_TITLEBGM);

	return S_OK;
}
//=============================================================================
// 終了処理
//=============================================================================
void CTitle::Uninit(void)
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
void CTitle::Update(void)
{
	// シーンの更新処理
	CScene::Update();

	CFade* pFade = CManager::Instance()->GetFade();
	CInputKeyboard* pKeyboard = CManager::Instance()->GetInput()->GetKeyboard();
	CInputJoypad* pJoypad = CManager::Instance()->GetInput()->GetJoypad();

//	// カメラの取得
//	CCamera* pCamera = CManager::Instance()->GetCamera();
//
//#ifdef _DEBUG
//	if (pCamera->GetMode() == CCamera::MODE_EDIT && 
//		pKeyboard->GetTrigger(DIK_C) == true)
//	{
//		// 三人称カメラ
//		pCamera->SetCamMode(CCamera::MODE_THIRD_PERSON);
//	}
//	else if (pCamera->GetMode() == CCamera::MODE_THIRD_PERSON &&
//		pKeyboard->GetTrigger(DIK_C) == true)
//	{
//		// エディターカメラ
//		pCamera->SetCamMode(CCamera::MODE_EDIT);
//	}
//#endif

#ifdef _DEBUG
	if (pFade->GetFade() == CFade::FADE_NONE && pKeyboard->GetTrigger(DIK_RETURN))
	{
		// 接続画面に移行
		pFade->StartFadeOut(Mode::Connect);
	}
#else
	if (pFade->GetFade() == CFade::FADE_NONE &&
		(pJoypad->GetAnyTrigger() || pKeyboard->GetAnyKeyTrigger()))
	{
		// 接続画面に移行
		pFade->StartFadeOut(Mode::Connect);
	}
#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void CTitle::Draw(void)
{
	// シーンの描画処理
	CScene::Draw();

	//// カメラの取得
	//CCamera* pCamera = CManager::Instance()->GetCamera();

	//// カメラの設定
	//pCamera->SetPosV({ -605.0f, -835.0f, 4608.0f });
	//pCamera->SetPosR({ 7.0f, -835.0f, 4100.0f });
	//pCamera->SetRot({ 0.0f, -0.88f, 0.0f });
}
//=============================================================================
// ライト設定処理
//=============================================================================
void CTitle::ResetLight(void)
{
	// ディレクショナルライトの生成
	CreateDirectionalLight({ 0.0f, -1.0f, 0.0f }, D3DXCOLOR(1, 1, 1, 1));

	// ディレクショナルライトの生成
	CreateDirectionalLight({ 0.0f, 1.0f, 0.0f }, D3DXCOLOR(1, 1, 1, 1));

	// ディレクショナルライトの生成
	CreateDirectionalLight({ 1.0f, 0.0f, 0.0f }, D3DXCOLOR(1, 1, 1, 1));

	// ディレクショナルライトの生成
	CreateDirectionalLight({ -1.0f, 0.0f, 0.0f }, D3DXCOLOR(1, 1, 1, 1));

	// ディレクショナルライトの生成
	CreateDirectionalLight({ 0.0f, 0.0f, 1.0f }, D3DXCOLOR(1, 1, 1, 1));

	// ディレクショナルライトの生成
	CreateDirectionalLight({ 0.0f, 0.0f, -1.0f }, D3DXCOLOR(1, 1, 1, 1));
}
//=============================================================================
// デバイスリセット通知
//=============================================================================
void CTitle::OnDeviceReset(void)
{
	// ライトの再設定処理
	CLightManager::Instance()->RestoreLights();
}