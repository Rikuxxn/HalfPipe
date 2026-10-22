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
#include <XModelRenderComponent.h>
#include "imguimaneger.h"
#include "RigidBodyComponent.h"
#include <CollisionGroup.h>
#include "XModelResource.h"

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

//=============================================================================
// コンストラクタ
//=============================================================================
CBasketBlock::CBasketBlock()
{

}

//=============================================================================
// デストラクタ
//=============================================================================
CBasketBlock::~CBasketBlock()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CBasketBlock::Init(void)
{
	// シーンの取得
	auto scene = CManager::Instance()->GetSceneManager()->GetScene();

	if (!scene)
	{
		return E_FAIL;
	}

	// オブジェクトの生成
	auto obj = scene->CreateObject<CPointCircleBlock>();

	if (!obj)
	{
		return E_FAIL;
	}

	// トランスフォームコンポーネントの追加
	auto transform = obj->AddComponent<CTransformComponent>();

	auto thisTransform = this->GetComponent<CTransformComponent>();

	// 位置設定
	transform->Position = { thisTransform->Position.x,  thisTransform->Position.y, thisTransform->Position.z };

	// モデルレンダーコンポーネントの追加
	CXModelRenderComponent* model = nullptr;

	// タグの設定
	obj->SetTag("PointCircle");

	//-----------------------------------------
	// モデル描画
	//-----------------------------------------

	// モデルレンダーコンポーネントの追加
	model = obj->AddComponent<CXModelRenderComponent>();

	// 描画順の設定
	model->SetLayer(RenderLayer::Model);

	// モデルのロード
	model->Load("data/MODELS/PointCircle.x");

	//-----------------------------------------
	// 剛体生成
	//-----------------------------------------

	obj->SetGroupName("PointCircles");

	// リジッドボディコンポーネントの追加
	auto rigid = obj->AddComponent<CRigidBodyComponent>();

	// Stage用のCollision設定
	rigid->SetCollisionFilter(
		COLLISION_GROUP_STAGE,
		COLLISION_GROUP_STAGE |
		COLLISION_GROUP_PLAYER |
		COLLISION_GROUP_GHOST |
		COLLISION_GROUP_ENEMY |
		COLLISION_GROUP_CAMERA
	);

	rigid->SetDynamic(false);

	auto size = VEC3_DEFAULT;

	if (model)
	{
		size = model->GetModelSize();
	}
	else
	{
		size = obj->GetColliderSize();
	}

	// コライダーの生成
	if (!model)
	{
		return E_FAIL;
	}

	auto resource = model->GetModel();

	if (resource)
	{
		auto triangleMesh = resource->CreateBulletTriangleMesh();

		rigid->CreateTriangleMesh(triangleMesh);
	}

	return S_OK;
}

//=============================================================================
// 更新処理
//=============================================================================
void CBasketBlock::Update(void)
{
	// ステージオブジェクトの更新
	CStageObject::Update();

}

CRightBlock::CRightBlock()
{

}

CRightBlock::~CRightBlock()
{

}

HRESULT CRightBlock::Init(void)
{

	return S_OK;
}

void CRightBlock::Update(void)
{
	// ステージオブジェクトの更新
	CStageObject::Update();

}

CSignBoardBlock::CSignBoardBlock()
{

}

CSignBoardBlock::~CSignBoardBlock()
{

}

//=============================================================================
// 初期化
//=============================================================================
HRESULT CSignBoardBlock::Init(void)
{

	return S_OK;
}

//=============================================================================
// 広告看板ブロックの更新
//=============================================================================
void CSignBoardBlock::Update(void)
{
	// ステージオブジェクトの更新
	CStageObject::Update();

}

//=============================================================================
// 保存
//=============================================================================
void CSignBoardBlock::SaveToJson(json& j)
{
	CStageObject::SaveToJson(j);

	j["sign_textures"] = json::array();

	for (const auto& info : m_SignTextures)
	{
		j["sign_textures"].push_back(
			{
				{"face", info.FaceIndex},
				{"texture", info.TexturePath}
			}
		);
	}
}

//=============================================================================
// 読み込み
//=============================================================================
void CSignBoardBlock::LoadFromJson(const json& j)
{
	CStageObject::LoadFromJson(j);

	m_SignTextures.clear();

	if (!j.contains("sign_textures") ||
		!j["sign_textures"].is_array())
	{
		return;
	}

	for (const auto& item : j["sign_textures"])
	{
		SignTextureInfo info;

		info.FaceIndex = item.value("face", 0);
		info.TexturePath = item.value("texture", "");

		m_SignTextures.push_back(info);
	}

	ApplySignTextures();
}

//=============================================================================
// カスタムGUI
//=============================================================================
void CSignBoardBlock::DrawCustomUI(void)
{
	if (!ImGui::CollapsingHeader("SignBoardTexture"))
	{
		return;
	}

	if (ImGui::Button("Add Settings"))
	{
		m_SignTextures.emplace_back();
	}

	ImGui::Separator();

	for (int i = 0; i < static_cast<int>(m_SignTextures.size()); )
	{
		auto& info = m_SignTextures[i];

		ImGui::PushID(i);

		ImGui::Text("Set Texture %d", i + 1);

		ImGui::InputInt("Subset Num", &info.FaceIndex);

		char path[256] = {};

		strncpy_s(
			path,
			sizeof(path),
			info.TexturePath.c_str(),
			_TRUNCATE
		);

		if (ImGui::InputText("TexturePath", path, sizeof(path)))
		{
			info.TexturePath = path;
		}

		if (ImGui::Button("Apply"))
		{
			ApplySignTextures();
		}

		ImGui::SameLine();

		if (ImGui::Button("Delete"))
		{
			m_SignTextures.erase(
				m_SignTextures.begin() + i
			);

			ApplySignTextures();

			ImGui::PopID();
			continue;
		}

		ImGui::Separator();

		ImGui::PopID();

		++i;
	}
}

//=============================================================================
// テクスチャ設定を適用
//=============================================================================
void CSignBoardBlock::ApplySignTextures(void)
{
	auto renderer = GetComponent<CXModelRenderComponent>();

	if (!renderer)
	{
		return;
	}

	renderer->ClearFaceTextures();

	for (const auto& info : m_SignTextures)
	{
		if (info.FaceIndex < 0)
		{
			continue;
		}

		if (info.TexturePath.empty())
		{
			continue;
		}

		renderer->SetFaceTexture(
			info.FaceIndex,
			info.TexturePath
		);
	}
}


//=============================================================================
// コンストラクタ
//=============================================================================
CRiftPoleBlock::CRiftPoleBlock()
{

}

//=============================================================================
// デストラクタ
//=============================================================================
CRiftPoleBlock::~CRiftPoleBlock()
{

}

//=============================================================================
// 初期化
//=============================================================================
HRESULT CRiftPoleBlock::Init(void)
{

	return S_OK;
}

//=============================================================================
// 更新処理
//=============================================================================
void CRiftPoleBlock::Update(void)
{
	// ステージオブジェクトの更新
	CStageObject::Update();
}


//=============================================================================
// 得点サークルのコンストラクタ
//=============================================================================
CPointCircleBlock::CPointCircleBlock()
{

}

//=============================================================================
// 得点サークルのデストラクタ
//=============================================================================
CPointCircleBlock::~CPointCircleBlock()
{

}

//=============================================================================
// 得点サークルの初期化処理
//=============================================================================
HRESULT CPointCircleBlock::Init(void)
{
	return S_OK;
}

//=============================================================================
// 得点サークルの更新処理
//=============================================================================
void CPointCircleBlock::Update(void)
{
	// ステージオブジェクトの更新
	CStageObject::Update();

}

//=============================================================================
// コリジョンエンター
//=============================================================================
void CPointCircleBlock::OnCollisionEnter(CGameObject* other)
{
	if (other->CompareTag("Ball"))
	{

	}
}

//=============================================================================
// ゲートバナーのコンストラクタ
//=============================================================================
CGateBannerBlock::CGateBannerBlock()
{

}

//=============================================================================
// ゲートバナーのデストラクタ
//=============================================================================
CGateBannerBlock::~CGateBannerBlock()
{

}

//=============================================================================
// ゲートバナーの初期化処理
//=============================================================================
HRESULT CGateBannerBlock::Init(void)
{
	return S_OK;
}

//=============================================================================
// ゲートバナーの更新処理
//=============================================================================
void CGateBannerBlock::Update(void)
{
	// ステージオブジェクトの更新
	CStageObject::Update();

}

//=============================================================================
// 保存
//=============================================================================
void CGateBannerBlock::SaveToJson(json& j)
{
	CStageObject::SaveToJson(j);

	j["sign_textures"] = json::array();

	for (const auto& info : m_SignTextures)
	{
		j["sign_textures"].push_back(
			{
				{"face", info.FaceIndex},
				{"texture", info.TexturePath}
			}
		);
	}
}

//=============================================================================
// 読み込み
//=============================================================================
void CGateBannerBlock::LoadFromJson(const json& j)
{
	CStageObject::LoadFromJson(j);

	m_SignTextures.clear();

	if (!j.contains("sign_textures") ||
		!j["sign_textures"].is_array())
	{
		return;
	}

	for (const auto& item : j["sign_textures"])
	{
		SignTextureInfo info;

		info.FaceIndex = item.value("face", 0);
		info.TexturePath = item.value("texture", "");

		m_SignTextures.push_back(info);
	}

	ApplySignTextures();
}

//=============================================================================
// カスタムGUI
//=============================================================================
void CGateBannerBlock::DrawCustomUI(void)
{
	if (!ImGui::CollapsingHeader("SignBoardTexture"))
	{
		return;
	}

	if (ImGui::Button("Add Settings"))
	{
		m_SignTextures.emplace_back();
	}

	ImGui::Separator();

	for (int i = 0; i < static_cast<int>(m_SignTextures.size()); )
	{
		auto& info = m_SignTextures[i];

		ImGui::PushID(i);

		ImGui::Text("Set Texture %d", i + 1);

		ImGui::InputInt("Subset Num", &info.FaceIndex);

		char path[256] = {};

		strncpy_s(
			path,
			sizeof(path),
			info.TexturePath.c_str(),
			_TRUNCATE
		);

		if (ImGui::InputText("TexturePath", path, sizeof(path)))
		{
			info.TexturePath = path;
		}

		if (ImGui::Button("Apply"))
		{
			ApplySignTextures();
		}

		ImGui::SameLine();

		if (ImGui::Button("Delete"))
		{
			m_SignTextures.erase(
				m_SignTextures.begin() + i
			);

			ApplySignTextures();

			ImGui::PopID();
			continue;
		}

		ImGui::Separator();

		ImGui::PopID();

		++i;
	}
}

//=============================================================================
// テクスチャ設定を適用
//=============================================================================
void CGateBannerBlock::ApplySignTextures(void)
{
	auto renderer = GetComponent<CXModelRenderComponent>();

	if (!renderer)
	{
		return;
	}

	renderer->ClearFaceTextures();

	for (const auto& info : m_SignTextures)
	{
		if (info.FaceIndex < 0)
		{
			continue;
		}

		if (info.TexturePath.empty())
		{
			continue;
		}

		renderer->SetFaceTexture(
			info.FaceIndex,
			info.TexturePath
		);
	}
}

