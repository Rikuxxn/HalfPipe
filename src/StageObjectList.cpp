//=============================================================================
//
// ステージオブジェクトリスト処理 [StageObjectList.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "StageObjectList.h"
#include "Manager.h"
#include "TransformComponent.h"
#include "ParticleEmitterComponent.h"
#include "SceneManager.h"
#include "Scene.h"
#include "MatrixUtility.h"

// 名前空間の使用
using namespace MatrixUtil;


CFloorBlock::CFloorBlock()
{

}

CFloorBlock::~CFloorBlock()
{

}

HRESULT CFloorBlock::Init(void)
{
	//// レンダーターゲットの取得
	//m_pRenderTarget = CRenderTargetManager::GetInstance().Get("RankingScreen");

	//if (!m_pRenderTarget)
	//{
	//	return E_FAIL;
	//}

	////------------------------------------
	//// マテリアルに貼る
	////------------------------------------

	//SetTexture(1, m_pRenderTarget->GetTexture());

	//// デバイスリソース管理へ登録
	//CDeviceResourceManager::GetInstance().Register(this);

	return S_OK;
}

void CFloorBlock::Update(void)
{
	// ステージオブジェクトの更新
	CStageObject::Update();
}

void CFloorBlock::OnCollisionEnter(CGameObject* other)
{
	if (other->CompareTag("Player"))
	{
		//int n = 0;
	}
}



CWallBlock::CWallBlock()
{

}

CWallBlock::~CWallBlock()
{

}

HRESULT CWallBlock::Init(void)
{

	return S_OK;
}

void CWallBlock::Update(void)
{
	// ステージオブジェクトの更新
	CStageObject::Update();


}

CTorchBlock::CTorchBlock() :
	m_SoundHandle(CSoundManager::INVALID_HANDLE)
{

}

CTorchBlock::~CTorchBlock()
{
	//=====================================================================
	// サウンド破棄
	//=====================================================================
	auto soundManager = CManager::Instance()->GetSoundManager();

	if (soundManager &&
		m_SoundHandle != CSoundManager::INVALID_HANDLE)
	{
		soundManager->Stop(m_SoundHandle);

		m_SoundHandle = CSoundManager::INVALID_HANDLE;
	}

	//=====================================================================
	// パーティクルオブジェクト削除
	//=====================================================================
	if (m_ParticleObject)
	{
		m_ParticleObject->Destroy();
		m_ParticleObject = nullptr;
	}
}

HRESULT CTorchBlock::Init(void)
{
	//=========================================================================
	// 3DループSE再生
	//=========================================================================

	// サウンドマネージャー取得
	auto soundManager = CManager::Instance()->GetSoundManager();

	if (soundManager)
	{
		auto transform = GetComponent<CTransformComponent>();

		if (transform)
		{
			// 3Dサウンド再生
			m_SoundHandle = soundManager->Play3D(CSoundManager::SOUND_LABEL_FIRE, transform->Position);
		}
	}

	// シーンの取得
	auto scene = CManager::Instance()->GetSceneManager()->GetScene();

	if (!scene)
	{
		return S_OK;
	}

	// 3D炎パーティクルオブジェクトの生成
	m_ParticleObject = scene->CreateObject();

	m_ParticleObject->SetTag("Particle3D");

	// 松明の位置
	auto transform = GetComponent<CTransformComponent>();

	// パーティクルの位置
	auto particle3DTransform = m_ParticleObject->AddComponent<CTransformComponent>();

	// 松明の先端位置
	D3DXVECTOR3 localTipPosition(0.0f, 30.0f, 0.0f);

	// ローカルからワールドへ
	particle3DTransform->Position = LocalToWorld(
		localTipPosition,
		transform->Position,
		transform->Rotation,
		transform->Scale
	);

	auto fire3D = m_ParticleObject->AddComponent<CParticleEmitterComponent>();

	fire3D->SetLayer(RenderLayer::Particle3D);

	// 放出空間の設定
	fire3D->Space = ParticleSpace::Space3D;

	// エミッターのセッターにテクスチャを設定
	fire3D->SetTexture("data/TEXTURE/smoke.jpg");

	// シェーダーの設定
	fire3D->SetShader("data/Shader/Instancing/ParticleInstancing.fx");

	fire3D->col = { 1.0f, 0.7f, 0.2f, 1.0f };
	fire3D->EmitMode = ParticleEmitMode::Loop;
	fire3D->Type = ParticleType::Fire;
	fire3D->SpawnShape = ParticleSpawnShape::Point;
	fire3D->Radius = 3.0f;
	fire3D->EmitRate = 1;
	fire3D->LifeTimeMin = 20;
	fire3D->LifeTimeMax = 40;
	fire3D->SpeedMin = 0.5f;
	fire3D->SpeedMax = 2.0f;
	fire3D->SizeMin = 20;
	fire3D->SizeMax = 40;

	return S_OK;
}

void CTorchBlock::Update(void)
{
	// ステージオブジェクトの更新
	CStageObject::Update();

	auto soundManager = CManager::Instance()->GetSoundManager();

	auto transform = GetComponent<CTransformComponent>();

	if (soundManager && transform &&
		m_SoundHandle != CSoundManager::INVALID_HANDLE)
	{
		soundManager->UpdateSoundPosition(m_SoundHandle, transform->Position);
	}

	if (!transform || !m_ParticleObject)
	{
		return;
	}

	auto particleTransform = m_ParticleObject->GetComponent<CTransformComponent>();

	if (!particleTransform)
	{
		return;
	}

	// 松明の先端位置
	D3DXVECTOR3 localTipPosition(0.0f, 30.0f, 0.0f);

	// ローカルからワールドへ
	particleTransform->Position = LocalToWorld(
		localTipPosition,
		transform->Position,
		transform->Rotation,
		transform->Scale
	);
}

CHalfPipeBlock::CHalfPipeBlock()
{

}

CHalfPipeBlock::~CHalfPipeBlock()
{

}

HRESULT CHalfPipeBlock::Init(void)
{

	return S_OK;
}

void CHalfPipeBlock::Update(void)
{
	// ステージオブジェクトの更新
	CStageObject::Update();

}
