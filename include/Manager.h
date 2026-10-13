//=============================================================================
//
// マネージャー処理 [Manager.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _MANAGER_H_// このマクロ定義がされていなかったら
#define _MANAGER_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
//#include "Sound.h"
//#include "PauseManager.h"

// 前方宣言
class CRenderer;
class CInputManager;
class CSceneManager;
class CTextureManager;
class CCamera;
class CFade;
class CDebugRenderer;
class CSoundManager;
class CParticleRenderer2D;
class CParticleRenderer3D;
class CShaderManager;
class CTimeSystem;
class CStageManager;
class CTriggerObjectManager;
class CDebugDrawer;
class CModelInstancingRenderer;
class CCollisionSystem;
class CGhostObjectManager;
class btGhostPairCallback;
class CGameTime;
class CNetworkManager;
class CUIAnimationManager;
class CUIAnimationEditor;


//*****************************************************************************
// マネージャークラス
//*****************************************************************************
class CManager
{
private:

	static std::unique_ptr<CManager> m_Instance;

private:

	std::unique_ptr<CRenderer>							 m_pRenderer;
	std::unique_ptr<CInputManager>						 m_pInputManager;
	std::unique_ptr<CSceneManager>						 m_pSceneManager;
	std::unique_ptr<CTextureManager>					 m_pTextureManager;
	std::unique_ptr<CCamera>							 m_pCamera;
	std::unique_ptr<CFade>								 m_pFade;
	std::unique_ptr<CDebugRenderer>						 m_pDebugRenderer;
	std::unique_ptr<CSoundManager>						 m_pSoundManager;
	std::unique_ptr<CParticleRenderer2D>				 m_pParticleRenderer2D;
	std::unique_ptr<CParticleRenderer3D>				 m_pParticleRenderer3D;
	std::unique_ptr<CShaderManager>						 m_pShaderManager;
	std::unique_ptr<CTimeSystem>						 m_pTimeSystem;
	std::unique_ptr<CStageManager>						 m_pStageManager;
	std::unique_ptr<CTriggerObjectManager>				 m_pTriggerObjectManager;
	std::unique_ptr<CUIAnimationManager>				 m_pUIAnimationManager;
	std::unique_ptr<CUIAnimationEditor>					 m_pUIAnimationEditor;
	std::unique_ptr<CDebugDrawer>						 m_pDebugDrawer;
	std::unique_ptr<CModelInstancingRenderer>			 m_pModelInstancingRenderer;
	std::unique_ptr<CCollisionSystem>					 m_pCollisionSystem;
	std::unique_ptr<CGhostObjectManager>				 m_pGhostObjectManager;
	std::unique_ptr<CGameTime>							 m_pGameTime;
	std::unique_ptr<CNetworkManager>					 m_pNetworkManager;


	// BulletPhysics
	std::unique_ptr<btBroadphaseInterface>				 m_pBroadphase;				// 衝突判定のクラスへのポインタ
	std::unique_ptr<btDefaultCollisionConfiguration>	 m_pCollisionConfiguration;	// 衝突検出の設定を管理するクラスへのポインタ
	std::unique_ptr<btCollisionDispatcher>				 m_pDispatcher;				// 実際に衝突判定処理を実行するクラスへのポインタ
	std::unique_ptr<btSequentialImpulseConstraintSolver> m_pSolver;					// 物理シミュレーションの制約ソルバーへのポインタ
	std::unique_ptr<btDiscreteDynamicsWorld>			 m_pDynamicsWorld;			// 物理世界を管理するクラスへのポインタ
	std::unique_ptr<btGhostPairCallback>				 m_pPairCache;				// Ghostペア生成を有効化

private:

	CManager();

public:

	~CManager();

public:

	static CManager* Instance(void);
	static void Create(void);
	static void Destroy(void);

public:

	HRESULT Init(HINSTANCE hInstance, HWND hWnd);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void OnDeviceReset(void);

public:

	CRenderer*				  GetRenderer				(void);
	CInputManager*			  GetInput					(void);
	CSceneManager*			  GetSceneManager			(void);
	CTextureManager*		  GetTextureManager			(void);
	CCamera*				  GetCamera					(void);
	CFade*					  GetFade					(void);
	CDebugRenderer*			  GetDebugRenderer			(void);
	CSoundManager*			  GetSoundManager			(void);
	CParticleRenderer2D*	  GetParticleRenderer2D		(void);
	CParticleRenderer3D*	  GetParticleRenderer3D		(void);
	CShaderManager*			  GetShaderManager			(void);
	CTimeSystem*			  GetTimeSystem				(void);
	CStageManager*			  GetStageManager			(void);
	CTriggerObjectManager*	  GetTriggerObjectManager	(void);
	CUIAnimationManager*	  GetUIAnimationManager		(void);
	CUIAnimationEditor*		  GetUIAnimationEditor		(void);
	CDebugDrawer*			  GetDebugDrawer			(void);
	btDiscreteDynamicsWorld*  GetPhysicsWorld			(void);
	CModelInstancingRenderer* GetModelInstancingRenderer(void);
	CCollisionSystem*		  GetCollisionSystem		(void);
	CGhostObjectManager*	  GetGhostObjectManager		(void);
	CGameTime*				  GetGameTime				(void);
	CNetworkManager*		  GetNetworkManager			(void);
};

#endif