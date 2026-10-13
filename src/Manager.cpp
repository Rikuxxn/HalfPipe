//=============================================================================
//
// マネージャー処理 [Manager.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Manager.h"
#include "Renderer.h"
#include "Game.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "Input.h"
#include "Camera.h"
#include "Fade.h"
#include "Title.h"
#include "TextureManager.h"
#include "DebugRenderer.h"
#include "PauseManager.h"
#include "SoundManager.h"
#include "ParticleRenderer2D.h"
#include "ParticleRenderer3D.h"
#include "ShaderManager.h"
#include "imguimaneger.h"
#include "TimeSystem.h"
#include "DebugManager.h"
#include "StageManager.h"
#include "TriggerObjectManager.h"
#include "UIAnimationManager.h"
#include "UIAnimationEditor.h"
#include "DebugDrawer.h"
#include "ModelInstancingRenderer.h"
#include "ShadowMap.h"
#include "CollisionSystem.h"
#include "GhostObjectManager.h"
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include "GameTime.h"
#include "StageEditor.h"
#include "TriggerObjectEditor.h"
#include "RigidBodyComponent.h"
#include "GhostObjectComponent.h"
#include "NetWorkManager.h"


//*****************************************************************************
// 静的メンバ変数宣言
//*****************************************************************************
std::unique_ptr<CManager> CManager::m_Instance = nullptr;


namespace PhysicsWorld
{
	const btVector3 GRAVITY{ 0, -980.0f, 0 };
	constexpr int	MAX_SUBSTEPS	= 10;
	constexpr float FIXED_TIME_STEP = 0.016f;
}


//=============================================================================
// コンストラクタ
//=============================================================================
CManager::CManager()
{
	// 値のクリア

}
//=============================================================================
// デストラクタ
//=============================================================================
CManager::~CManager()
{

}

//=============================================================================
// インスタンス取得
//=============================================================================
CManager* CManager::Instance(void)
{
	return m_Instance.get();
}

//=============================================================================
// 生成
//=============================================================================
void CManager::Create(void)
{
	if (!m_Instance)
	{
		m_Instance.reset(new CManager());
	}
}

//=============================================================================
// 破棄
//=============================================================================
void CManager::Destroy(void)
{
	m_Instance.reset();
}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CManager::Init(HINSTANCE hInstance, HWND hWnd)
{
	// レンダラーの生成
	m_pRenderer = std::make_unique<CRenderer>();;

	// シェーダーマネージャーの生成
	m_pShaderManager = std::make_unique<CShaderManager>();

	// レンダラーの初期化処理
	if (FAILED(m_pRenderer->Init(hWnd, TRUE)))
	{
		return E_FAIL;
	}

	// タイムシステムの生成
	m_pTimeSystem = std::make_unique<CTimeSystem>();

	// パーティクルレンダラー2Dの生成
	m_pParticleRenderer2D = std::make_unique<CParticleRenderer2D>();

	// パーティクルレンダラー2Dの初期化処理
	if (FAILED(m_pParticleRenderer2D->Init()))
	{
		return E_FAIL;
	}

	// パーティクルレンダラー3Dの生成
	m_pParticleRenderer3D = std::make_unique<CParticleRenderer3D>();

	// パーティクルレンダラー3Dの初期化処理
	if (FAILED(m_pParticleRenderer3D->Init()))
	{
		return E_FAIL;
	}

	// モデルインスタンシングレンダラーの生成
	m_pModelInstancingRenderer = std::make_unique<CModelInstancingRenderer>();

	// 初期化
	if (FAILED(m_pModelInstancingRenderer->Init()))
	{
		return E_FAIL;
	}

	//// パーティクルシステムの生成
	//m_pParticleSystem = std::make_unique<CParticleSystem>();

#ifdef _DEBUG

	// グリッドの描画
	m_pDebugRenderer = std::make_unique<CDebugRenderer>();

	// グリッドの初期化処理
	if (FAILED(m_pDebugRenderer->Init()))
	{
		return E_FAIL;
	}

#endif

	// 入力マネージャーの生成
	m_pInputManager = std::make_unique<CInputManager>();

	// 入力マネージャーの初期化処理
	if (FAILED(m_pInputManager->Init(hInstance, hWnd)))
	{
		return E_FAIL;
	}

	// サウンドの生成
	m_pSoundManager = std::make_unique<CSoundManager>();

	// サウンドの初期化処理
	if (FAILED(m_pSoundManager->Init(hWnd)))
	{
		return E_FAIL;
	}

	// Bullet物理ワールドの生成
	m_pBroadphase = std::make_unique<btDbvtBroadphase>();
	m_pCollisionConfiguration = std::make_unique <btDefaultCollisionConfiguration>();
	m_pDispatcher = std::make_unique <btCollisionDispatcher>(m_pCollisionConfiguration.get());
	m_pSolver = std::make_unique <btSequentialImpulseConstraintSolver>();

	// 物理世界の生成
	m_pDynamicsWorld = std::make_unique <btDiscreteDynamicsWorld>(m_pDispatcher.get(), m_pBroadphase.get(), m_pSolver.get(), m_pCollisionConfiguration.get());

	// ゴーストオブジェクトペアキャッシュの生成
	m_pPairCache = std::make_unique <btGhostPairCallback>();

	m_pBroadphase->getOverlappingPairCache()->setInternalGhostPairCallback(m_pPairCache.get());

	// 衝突判定システム生成
	m_pCollisionSystem = std::make_unique <CCollisionSystem>();

	// ゴーストオブジェクトマネージャーの生成
	m_pGhostObjectManager = std::make_unique <CGhostObjectManager>();

	m_pDebugDrawer = std::make_unique <CDebugDrawer>();
	m_pDynamicsWorld->setDebugDrawer(m_pDebugDrawer.get());

	// ゲームタイムの生成
	m_pGameTime = std::make_unique <CGameTime>();

	// ゲームタイムの初期化
	m_pGameTime->Init();

	// ネットワーク
	m_pNetworkManager = std::make_unique<CNetworkManager>();

	if (!m_pNetworkManager->Init())
	{
		return E_FAIL;
	}

	// デバッグ描画モード
	m_pDebugDrawer->setDebugMode(btIDebugDraw::DBG_DrawWireframe);

	// 重力を設定
	m_pDynamicsWorld->setGravity(PhysicsWorld::GRAVITY);

	// カメラの生成
	m_pCamera = std::make_unique <CCamera>();

	// カメラの初期化処理
	m_pCamera->Init();

	// テクスチャマネージャーの生成
	m_pTextureManager = std::make_unique<CTextureManager>();

	// ステージマネージャーの生成
	m_pStageManager = std::make_unique<CStageManager>();

	// トリガーオブジェクトマネージャーの生成
	m_pTriggerObjectManager = std::make_unique<CTriggerObjectManager>();

	// UIアニメーションマネージャーの生成
	m_pUIAnimationManager = std::make_unique<CUIAnimationManager>();

	// UIアニメーションエディターの生成
	m_pUIAnimationEditor = std::make_unique<CUIAnimationEditor>();

	// UIアニメーションエディターの初期化処理
	m_pUIAnimationEditor->Init(m_pUIAnimationManager.get());

	// シーンマネージャーの生成
	m_pSceneManager = std::make_unique<CSceneManager>();

	// シーンマネージャーの初期化
	m_pSceneManager->Init(CScene::Mode::Title);

	// フェードの生成
	m_pFade = std::make_unique<CFade>();

	// フェードの初期化
	m_pFade->Init();

	return S_OK;
}
//=============================================================================
// 終了処理
//=============================================================================
void CManager::Uninit(void)
{
	if (m_pNetworkManager)
	{
		m_pNetworkManager->Uninit();
	}

	// ネットワークの破棄
	m_pNetworkManager.reset();

	// シーンマネージャーの破棄
	m_pSceneManager.reset();

	// テクスチャマネージャーの破棄
	m_pTextureManager.reset();

	// シェーダーマネージャーの破棄
	m_pShaderManager.reset();

	// 入力マネージャーの破棄
	m_pInputManager.reset();

	// サウンドマネージャーの破棄
	if (m_pSoundManager)
	{
		m_pSoundManager->Uninit();
	}

	m_pSoundManager.reset();

	// カメラの破棄
	m_pCamera.reset();

	// フェードの破棄
	m_pFade.reset();

	if (m_pRenderer)
	{
		m_pRenderer->Uninit();
	}

	m_pRenderer.reset();
}
//=============================================================================
// 更新処理
//=============================================================================
void CManager::Update(void)
{
	// 入力マネージャーの更新
	m_pInputManager->Update();

	// ネットワーク受信
	m_pNetworkManager->Update();

	// カメラの更新
	m_pCamera->Update();

	// サウンドマネージャーの更新
	m_pSoundManager->Update();

	// サウンドのリスナー更新
	const D3DXVECTOR3& cameraPos = m_pCamera->GetPosR();
	const D3DXVECTOR3& cameraForward = m_pCamera->GetForward();

	m_pSoundManager->UpdateListener(cameraPos, cameraForward);

	// シーンマネージャーの更新
	m_pSceneManager->Update();

	// ステージマネージャーの更新処理
	m_pStageManager->Update();

	// トリガーオブジェクトマネージャーの更新処理
	m_pTriggerObjectManager->Update();

	// UIアニメーションエディターの更新処理
	m_pUIAnimationEditor->Update();

	// フェードの更新処理
	if (m_pFade != nullptr)
	{
		// フェードの更新処理
		m_pFade->Update();
	}

	auto scene = m_pSceneManager->GetScene();

	auto game = dynamic_cast<CGame*>(scene);

	//---------------------------------
	// ポーズ中
	//---------------------------------

	if (game && game->GetPauseManager()->IsEnable())
	{
		return;
	}

	// デルタタイムの取得
	float deltaTime = m_pTimeSystem->GetDeltaTime();

	//---------------------------------
	// ゲーム時間更新
	//---------------------------------

	m_pGameTime->Update(deltaTime);

	// シミュレーション更新
	m_pDynamicsWorld->stepSimulation((btScalar)deltaTime, PhysicsWorld::MAX_SUBSTEPS, PhysicsWorld::FIXED_TIME_STEP);

	// 衝突判定更新
	m_pCollisionSystem->Update();
}
//=============================================================================
// 描画処理
//=============================================================================
void CManager::Draw(void)
{
	// 描画開始
	m_pRenderer->Begin();

#ifdef _DEBUG

	//---------------------------------
	// ImGuiウィンドウ作成
	//---------------------------------

	CDebugManager::Instance().Draw();

	// グリッドの描画
	m_pDebugRenderer->Draw();

	// Bullet Physicsコライダーデバッグ描画(選択中のオブジェクトのみ)
	if (m_pStageManager)
	{
		auto editor = m_pStageManager->GetStageEditor();

		if (editor)
		{
			auto selected = editor->GetSelectedObject();

			if (selected)
			{
				auto rigid = selected->GetComponent<CRigidBodyComponent>();

				if (rigid)
				{
					btRigidBody* body = rigid->GetBody();

					if (body)
					{
						m_pDynamicsWorld->debugDrawObject(
							body->getWorldTransform(),
							body->getCollisionShape(),
							btVector3(0.0f, 1.0f, 0.3f));
					}
				}
			}
		}
	}

	if (m_pTriggerObjectManager)
	{
		const auto& triggerObjects = m_pTriggerObjectManager->GetTriggerObjects();

		for (auto* object : triggerObjects)
		{
			if (!object)
			{
				continue;
			}

			auto ghost = object->GetComponent<CGhostObjectComponent>();

			if (!ghost)
			{
				continue;
			}

			btGhostObject* ghostObject = ghost->GetGhost();

			if (!ghostObject)
			{
				continue;
			}

			m_pDynamicsWorld->debugDrawObject(
				ghostObject->getWorldTransform(),
				ghostObject->getCollisionShape(),
				btVector3(
					1.0f,
					0.5f,
					0.0f));
		}
	}

#endif

	// カメラの設定
	m_pCamera->SetCamera();

	//---------------------------------------
	// シャドウパス
	//---------------------------------------

	// シャドウマップの取得
	auto shadow = CManager::Instance()->GetRenderer()->GetShadowMap();

	shadow->Begin();

	m_pModelInstancingRenderer->Begin();

	m_pSceneManager->DrawShadow();

	m_pModelInstancingRenderer->EndShadow();

	shadow->End();

	//---------------------------------------
	// メイン(通常)パス
	//---------------------------------------
	
	D3DXMATRIX view;
	D3DXMATRIX proj;

	// デバイスの取得
	auto device = m_pRenderer->GetDevice();

	device->GetTransform(D3DTS_VIEW, &view);
	device->GetTransform(D3DTS_PROJECTION, &proj);

	// インスタンシング開始
	m_pModelInstancingRenderer->Begin();

	// ビュー・プロジェクションの設定
	m_pModelInstancingRenderer->SetViewProjection(view,	proj);

	// シーンマネージャーの描画
	m_pSceneManager->Draw();

	m_pModelInstancingRenderer->EndMain();

	// フェードの描画
	if (m_pFade)
	{
		m_pFade->Draw();
	}

//#ifdef _DEBUG
//
//	// ImGuiの描画終了
//	CImGuiManager::Instance().EndImguiRender();
//
//#endif

	// 描画終了
	m_pRenderer->End();
}
//=============================================================================
// デバイスリセット通知
//=============================================================================
void CManager::OnDeviceReset(void)
{
	if (m_pSceneManager)
	{
		m_pSceneManager->GetScene()->OnDeviceReset();
	}
}

// レンダラーの取得
CRenderer* CManager::GetRenderer(void)
{ 
	return m_pRenderer.get(); 
}

// 入力処理の取得
CInputManager* CManager::GetInput(void)
{ 
	return m_pInputManager.get();
}

// シーンマネージャーの取得
CSceneManager* CManager::GetSceneManager(void)
{ 
	return m_pSceneManager.get();
}

// テクスチャマネージャーの取得
CTextureManager* CManager::GetTextureManager(void)
{ 
	return m_pTextureManager.get();
}

// カメラの取得
CCamera* CManager::GetCamera(void) 
{ 
	return m_pCamera.get();
}

// フェードの取得
CFade* CManager::GetFade(void)
{ 
	return m_pFade.get();
}

// デバッグレンダラーの取得
CDebugRenderer* CManager::GetDebugRenderer(void)
{
	return m_pDebugRenderer.get();
}

// サウンドマネージャーの取得
CSoundManager* CManager::GetSoundManager(void)
{
	return m_pSoundManager.get();
}

// 2Dパーティクルレンダラーの取得
CParticleRenderer2D* CManager::GetParticleRenderer2D(void)
{
	return m_pParticleRenderer2D.get();
}

// 3Dパーティクルレンダラーの取得
CParticleRenderer3D* CManager::GetParticleRenderer3D(void)
{
	return m_pParticleRenderer3D.get();
}

// シェーダーマネージャーの取得
CShaderManager* CManager::GetShaderManager(void)
{
	return m_pShaderManager.get();
}

// タイムシステムの取得
CTimeSystem* CManager::GetTimeSystem(void)
{
	return m_pTimeSystem.get();
}

// ステージマネージャーの取得
CStageManager* CManager::GetStageManager(void)
{
	return m_pStageManager.get();
}

// トリガーオブジェクトマネージャーの取得
CTriggerObjectManager* CManager::GetTriggerObjectManager(void)
{
	return m_pTriggerObjectManager.get();
}

// UIアニメーションマネージャーの取得
CUIAnimationManager* CManager::GetUIAnimationManager(void)
{
	return m_pUIAnimationManager.get();
}

// UIアニメーションエディターの取得
CUIAnimationEditor* CManager::GetUIAnimationEditor(void)
{
	return m_pUIAnimationEditor.get();
}


// デバッグ描画の取得
CDebugDrawer* CManager::GetDebugDrawer(void)
{
	return m_pDebugDrawer.get();
}

// 物理世界の取得
btDiscreteDynamicsWorld* CManager::GetPhysicsWorld(void) 
{
	return m_pDynamicsWorld.get(); 
}

// モデルインスタンシングレンダラーの取得
CModelInstancingRenderer* CManager::GetModelInstancingRenderer(void)
{
	return m_pModelInstancingRenderer.get();
}

// コリジョンシステムの取得
CCollisionSystem* CManager::GetCollisionSystem(void)
{
	return m_pCollisionSystem.get();
}

// ゴーストオブジェクトマネージャーの取得
CGhostObjectManager* CManager::GetGhostObjectManager(void)
{
	return m_pGhostObjectManager.get();
}

// ゲームタイムの取得
CGameTime* CManager::GetGameTime(void)
{
	return m_pGameTime.get();
}

// ネットワークマネージャーの取得
CNetworkManager* CManager::GetNetworkManager(void)
{
	return m_pNetworkManager.get();
}