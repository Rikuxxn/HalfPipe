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
#include "Player.h"
#include "AimComponent.h"
#include "AimTargetComponent.h"
#include <FileDialogUtils.h>
#include "TimeSystem.h"
#include <ScoreRenderComponent.h>
#include <Ball.h>
#include "3DSpriteRenderComponent.h"
#include "ColorConstants.h"
#include <GhostObjectComponent.h>


// 名前空間の使用
using namespace MatrixUtil;
using namespace ColorConst;


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
	////=====================================================================
	//// サウンド破棄
	////=====================================================================
	//auto soundManager = CManager::Instance()->GetSoundManager();

	//if (soundManager &&
	//	m_SoundHandle != CSoundManager::INVALID_HANDLE)
	//{
	//	soundManager->Stop(m_SoundHandle);

	//	m_SoundHandle = CSoundManager::INVALID_HANDLE;
	//}

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
	////=========================================================================
	//// 3DループSE再生
	////=========================================================================

	//// サウンドマネージャー取得
	//auto soundManager = CManager::Instance()->GetSoundManager();

	//if (soundManager)
	//{
	//	auto transform = GetComponent<CTransformComponent>();

	//	if (transform)
	//	{
	//		// 3Dサウンド再生
	//		m_SoundHandle = soundManager->Play3D(CSoundManager::SOUND_LABEL_FIRE, transform->Position);
	//	}
	//}

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

	//auto soundManager = CManager::Instance()->GetSoundManager();

	auto transform = GetComponent<CTransformComponent>();

	//if (soundManager && transform &&
	//	m_SoundHandle != CSoundManager::INVALID_HANDLE)
	//{
	//	soundManager->UpdateSoundPosition(m_SoundHandle, transform->Position);
	//}

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
CBasketBlock::CBasketBlock() :
	m_CenterPosition	(INIT_VEC3),	// バスケットの中心位置
	m_CenterRotation	(INIT_VEC3),	// 回転
	m_AimRadius			(400.0f),		// エイム可能な半径
	m_HeightOffset		(5.0f)			// 得点サークルの高さの間隔
{

}

//=============================================================================
// デストラクタ
//=============================================================================
CBasketBlock::~CBasketBlock()
{
	m_PointCircles.clear();
	m_TexPoly.clear();
}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CBasketBlock::Init(void)
{
	// トランスフォームコンポーネントの取得
	auto transform = GetComponent<CTransformComponent>();

	if (!transform)
	{
		return E_FAIL;
	}

	// かごの中心となるローカル座標
	D3DXVECTOR3 localCenter(0.0f, CIRCLE_OFFSET_Y, 0.0f);

	// ワールド座標に変換
	m_CenterPosition = MatrixUtil::LocalToWorld(
		localCenter,
		transform->Position,
		transform->Rotation,
		transform->Scale
	);

	// 得点サークルの高さの間隔
	const float heightOffset = 30.0f;

	// 50点：中心
	CreatePointCircle(m_CenterPosition, 1.8f, 50);

	// 30点：中間
	CreatePointCircle(m_CenterPosition, 5.6f, 30);

	// 10点：外側
	CreatePointCircle(m_CenterPosition, 9.0f, 10);

	// テクスチャ用板ポリの生成
	CreateTexPoly(m_CenterPosition, 1.3f, "data/TEXTURE/Target_50.png", DX::RED);

	// テクスチャ用板ポリの生成
	CreateTexPoly(m_CenterPosition, 4.2f, "data/TEXTURE/Target_30.png", DX::GREEN);

	// テクスチャ用板ポリの生成
	CreateTexPoly(m_CenterPosition, 7.4f, "data/TEXTURE/Target_10.png", DX::WHITE);

	//=============================================================================
	// エイム対象の設定
	//=============================================================================
	auto aimTarget = AddComponent<CAimTargetComponent>();

	if (!aimTarget)
	{
		return E_FAIL;
	}

	// 得点サークルの中心位置をローカル座標で指定
	aimTarget->SetLocalCenter(D3DXVECTOR3(0.0f, CIRCLE_OFFSET_Y, 0.0f));

	// エイム可能な半径
	aimTarget->SetAimRadius(m_AimRadius);

	return S_OK;
}

//=============================================================================
// 更新処理
//=============================================================================
void CBasketBlock::Update(void)
{
	// ステージオブジェクトの更新
	CStageObject::Update();

	// 得点サークルをかごに追従させる
	UpdatePointCircles();

	// テクスチャ用板ポリをかごに追従させる
	UpdateTexPoly();
}

//=============================================================================
// 得点サークル更新処理
//=============================================================================
void CBasketBlock::UpdatePointCircles(void)
{
	auto transform = GetComponent<CTransformComponent>();

	if (!transform)
	{
		return;
	}

	// かごの中心となるローカル座標
	D3DXVECTOR3 localCenter(0.0f, CIRCLE_OFFSET_Y, 0.0f);

	// 現在の位置・回転・拡縮からワールド座標を計算
	m_CenterPosition = MatrixUtil::LocalToWorld(
		localCenter,
		transform->Position,
		transform->Rotation,
		transform->Scale
	);

	m_CenterRotation = transform->Rotation;

	// 得点サークルの高さを設定
	const D3DXVECTOR3 offsets[] = 
	{ 
		D3DXVECTOR3(0.0f, m_HeightOffset, 0.0f),	// 50点
		D3DXVECTOR3(0.0f, 0.0f, 0.0f),				// 30点 
		D3DXVECTOR3(0.0f, -m_HeightOffset, 0.0f)	// 10点
	};
	
	// 得点サークルを更新
	for (size_t i = 0; i < m_PointCircles.size(); ++i) 
	{
		auto* circle = m_PointCircles[i]; 

		if (!circle)
		{
			continue;
		}

		auto circleTransform = circle->GetComponent<CTransformComponent>();

		if (!circleTransform)
		{
			continue;
		}

		// 中心位置に高さのオフセットを加える 
		circleTransform->Position = m_CenterPosition + offsets[i];
		circleTransform->SetRotation(m_CenterRotation);
	}
}

//=============================================================================
// テクスチャ用板ポリの更新処理
//=============================================================================
void CBasketBlock::UpdateTexPoly(void)
{
	auto transform = GetComponent<CTransformComponent>();

	if (!transform)
	{
		return;
	}

	// かごの中心となるローカル座標
	D3DXVECTOR3 localCenter(0.0f, CIRCLE_OFFSET_Y, 0.0f);

	// 現在の位置・回転・拡縮からワールド座標を計算
	m_CenterPosition = MatrixUtil::LocalToWorld(
		localCenter,
		transform->Position,
		transform->Rotation,
		transform->Scale
	);

	m_CenterRotation = transform->Rotation;

	// 得点サークルの高さを設定
	const D3DXVECTOR3 offsets[] =
	{
		D3DXVECTOR3(0.0f, m_HeightOffset, 0.0f),	// 50点
		D3DXVECTOR3(0.0f, 0.0f, 0.0f),				// 30点 
		D3DXVECTOR3(0.0f, -m_HeightOffset, 0.0f)	// 10点
	};

	// テクスチャ用板ポリを更新
	for (size_t i = 0; i < m_TexPoly.size(); ++i)
	{
		auto* texPoly = m_TexPoly[i];
		
		if (!texPoly)
		{
			continue;
		}

		auto polyTransform = texPoly->GetComponent<CTransformComponent>();

		if (!polyTransform)
		{
			continue;
		}

		// 中心位置に高さのオフセットを加える 
		polyTransform->Position = m_CenterPosition + offsets[i];
		polyTransform->SetRotation(m_CenterRotation);
	}
}

//=============================================================================
// 得点サークル生成処理
//=============================================================================
CPointCircleBlock* CBasketBlock::CreatePointCircle(
	const D3DXVECTOR3& position, float scale, int point)
{
	// シーン取得
	auto scene = CManager::Instance()->GetSceneManager()->GetScene();

	if (!scene)
	{
		return nullptr;
	}

	// 得点サークルオブジェクトの生成
	auto obj = scene->CreateObject<CPointCircleBlock>();

	if (!obj)
	{
		return nullptr;
	}

	// トランスフォームコンポーネントの取得
	auto transform = obj->AddComponent<CTransformComponent>();

	if (!transform)
	{
		return nullptr;
	}

	transform->Position = position;
	transform->Scale = D3DXVECTOR3(scale, scale, scale);

	obj->SetTag("PointCircle");
	obj->SetGroupName("PointCircles");

	// 得点を設定
	obj->SetPoint(point);

	// モデル描画
	auto model = obj->AddComponent<CXModelRenderComponent>();

	if (!model)
	{
		return nullptr;
	}

	model->SetLayer(RenderLayer::Model);
	model->Load("data/MODELS/PointCircle.x");
	model->SetVisible(false);

	//// 剛体
	//auto rigid = obj->AddComponent<CRigidBodyComponent>();

	//if (!rigid)
	//{
	//	return nullptr;
	//}

	//rigid->SetCollisionFilter(
	//	COLLISION_GROUP_STAGE,
	//	COLLISION_GROUP_STAGE |
	//	COLLISION_GROUP_PLAYER |
	//	COLLISION_GROUP_GHOST |
	//	COLLISION_GROUP_ENEMY |
	//	COLLISION_GROUP_CAMERA
	//);

	//rigid->SetDynamic(false);

	//// モデルサイズ取得
	//D3DXVECTOR3 modelSize = model->GetModelSize();
	//
	//// シリンダーコライダーの生成
	//rigid->CreateCylinder(modelSize, 1.0f);

	//// 当たり判定更新
	//rigid->UpdateFromScale(transform->Scale);

	//-----------------------------------------
	// ゴーストオブジェクトの生成
	//-----------------------------------------

	// ゴーストオブジェクトコンポーネントの追加
	auto ghost = obj->AddComponent<CGhostObjectComponent>();

	// ゴーストの衝突フィルター
	ghost->SetCollisionFilter(
		COLLISION_GROUP_GHOST,
		COLLISION_GROUP_PLAYER |
		COLLISION_GROUP_STAGE |
		COLLISION_GROUP_ENEMY |
		COLLISION_GROUP_BALL
	);

	// モデルのサイズを取得
	D3DXVECTOR3 modelSize = model->GetModelSize();

	// シリンダートリガーを生成
	ghost->CreateCylinder(modelSize);

	// 当たり判定更新
	ghost->UpdateFromScale(transform->Scale);

	// 得点サークルを保存
	m_PointCircles.push_back(obj);

	return obj;
}

//=============================================================================
// テクスチャ用板ポリ生成処理
//=============================================================================
CGameObject* CBasketBlock::CreateTexPoly(const D3DXVECTOR3 pos, float scale, const std::string texPath,
	const D3DXCOLOR col)
{
	// シーン取得
	auto scene = CManager::Instance()->GetSceneManager()->GetScene();

	if (!scene)
	{
		return nullptr;
	}

	// テクスチャ用板ポリオブジェクトの生成
	auto obj = scene->CreateObject<CPointCircleBlock>();

	if (!obj)
	{
		return nullptr;
	}

	// トランスフォームコンポーネントの取得
	auto transform = obj->AddComponent<CTransformComponent>();

	if (!transform)
	{
		return nullptr;
	}

	obj->SetTag("TexPoly");

	// 3D板ポリゴン描画
	auto sprite = obj->AddComponent<C3DSpriteRenderComponent>();

	if (sprite)
	{
		sprite->SetSize(80.0f, 80.0f);
		sprite->SetTexture(texPath);
		sprite->SetColor(col);
		sprite->SetIsVisible(true);
	}

	// 初期位置
	if (transform)
	{
		transform->Position = pos;
		transform->Scale = D3DXVECTOR3(scale, scale, scale);
		transform->Rotation = { 0.0f, 0.0f, 0.0f };
	}

	// テクスチャ用板ポリを保存
	m_TexPoly.push_back(obj);

	return obj;
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
CPointCircleBlock::CPointCircleBlock() :
	m_Point (0) // 得点
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
// スコア加算
//=============================================================================
void CPointCircleBlock::AddScore(int point)
{
	// シーンの取得
	auto scene = CManager::Instance()->GetSceneManager()->GetScene();

	if (!scene)
	{
		return;
	}

	// Scoreタグのオブジェクトを取得
	auto scoreObject = scene->FindGameObject("Score");

	if (!scoreObject || !scoreObject->CompareTag("Score"))
	{
		return;
	}

	auto score = scoreObject->GetComponent<CScoreRenderComponent>();

	if (!score)
	{
		return;
	}

	// スコア加算
	score->AddScore(point);
}

//=============================================================================
// トリガーエンター
//=============================================================================
void CPointCircleBlock::OnTriggerEnter(CGameObject* other)
{
	if (!other || !other->CompareTag("Ball"))
	{
		return;
	}

	// シーンの取得
	auto scene = CManager::Instance()->GetSceneManager()->GetScene();

	if (!scene)
	{
		return;
	}

	// ボールの得点済み状態を取得
	auto ball = scene->FindGameObject<CBall>("Ball");

	if (!ball)
	{
		return;
	}

	//// すでに得点していたら無視
	//if (ball->HasScored())
	//{
	//	return;
	//}

	//// 得点済みにする
	//ball->SetScored(true);

	// このサークルの得点を取得
	int point = GetPoint();

	// 得点加算
	AddScore(GetPoint());
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

//=============================================================================
// コンストラクタ
//=============================================================================
CRiftBlock::CRiftBlock() :
	m_MoveDirection		(INIT_VEC3),	// 進行方向
	m_currentPoint		(0),			// 現在のポイント(エディタ用)
	m_StartMovePoint	(0),			// 移動開始ポイント
	m_AutoMoveSpeed		(100.0f)		// 移動スピード
{

}

//=============================================================================
// デストラクタ
//=============================================================================
CRiftBlock::~CRiftBlock()
{

}

//=============================================================================
// 初期化
//=============================================================================
HRESULT CRiftBlock::Init(void)
{

	return S_OK;
}

//=============================================================================
// 更新処理
//=============================================================================
void CRiftBlock::Update(void)
{
	// ステージオブジェクトの更新
	CStageObject::Update();

	if (m_MovePoints.empty())
	{
		return;
	}

	auto transform = GetComponent<CTransformComponent>();
	auto rigid = GetComponent<CRigidBodyComponent>();

	if (!transform || !rigid)
	{
		return;
	}

	// ポイント番号の範囲チェック
	if (m_StartMovePoint < 0 ||
		m_StartMovePoint >= static_cast<int>(m_MovePoints.size()))
	{
		m_StartMovePoint = 0;
	}

	// 現在の目標地点
	const D3DXVECTOR3& target = m_MovePoints[m_StartMovePoint];

	// 現在位置から目標地点への方向
	D3DXVECTOR3 dir = target - transform->Position;

	float dist = D3DXVec3Length(&dir);

	// 到着判定
	if (dist <= REACH_DIST)
	{
		m_StartMovePoint = (m_StartMovePoint + 1) % static_cast<int>(m_MovePoints.size());

		// 次の目標地点を取得
		dir = m_MovePoints[m_StartMovePoint] - transform->Position;

		dist = D3DXVec3Length(&dir);
	}

	// 移動方向が有効な場合
	if (dist > 0.0001f)
	{
		D3DXVec3Normalize(&dir, &dir);

		m_MoveDirection = dir;

		// デルタタイム
		const float deltaTime = CManager::Instance()->GetTimeSystem()->GetDeltaTime();

		// 1フレームの移動距離
		const float moveDistance = m_AutoMoveSpeed * deltaTime;

		// 移動距離を制限
		const float actualDistance = (std::min)(moveDistance, dist);

		transform->Position += dir * actualDistance;

		// 進行方向から目標角度を求める
		float targetAngle = atan2f(-dir.x, -dir.z);

		// 現在の角度
		float currentAngle = transform->Rotation.y;

		// 角度差を求める（-PI ～ PI）
		float angleDiff = atan2f(
			sinf(targetAngle - currentAngle),
			cosf(targetAngle - currentAngle)
		);

		// 回転を補間
		transform->Rotation.y += angleDiff * std::min(ROTATION_SPEED * deltaTime, 1.0f);
	}

	// Bullet側の座標を同期
	rigid->Transform();
}

//=============================================================================
// リフトポールの自動移動ポイントエディターの描画
//=============================================================================
void CRiftBlock::DrawCustomUI(void)
{
	ImGui::Dummy(ImVec2(0.0f, 5.0f));

	//=========================================================
	// 開始ポイントの設定
	//=========================================================

	if (ImGui::InputInt("Start Point", &m_StartMovePoint))
	{
		SetStartPoint(m_StartMovePoint);
	}

	ImGui::Dummy(ImVec2(0.0f, 5.0f));

	// 自動移動ポイント設置エディター
	if (ImGui::TreeNode("MovePoint Editor"))
	{
		ImGui::Dummy(ImVec2(0.0f, 5.0f));

		//=========================================================
		// 自動移動ポイントの追加
		//=========================================================

		static float addPos[3] = { 0.0f,0.0f,0.0f };

		ImGui::DragFloat3("Add Point", addPos, 1.0f);

		ImGui::Dummy(ImVec2(0.0f, 5.0f));

		if (ImGui::Button("Add Move Point"))
		{
			AddMovePoint(D3DXVECTOR3(addPos[0], addPos[1], addPos[2]));
		}

		ImGui::Dummy(ImVec2(0.0f, 10.0f));

		//=========================================================
		// 自動移動ポイントリスト
		//=========================================================

		for (int i = 0; i < (int)m_MovePoints.size(); i++)
		{
			ImGui::PushID(i);

			char label[64];

			sprintf_s(label, "Point %d", i);

			if (ImGui::TreeNode(label))
			{
				ImGui::Dummy(ImVec2(0.0f, 5.0f));

				//=================================================
				// 位置
				//=================================================

				if (ImGui::DragFloat3("Pos", &m_MovePoints[i].x, 1.0f))
				{
					if (i < m_pPointObj.size() && m_pPointObj[i])
					{
						auto pointTransform = m_pPointObj[i]->GetComponent<CTransformComponent>();

						if (pointTransform)
						{
							pointTransform->Position = m_MovePoints[i];
						}
					}
				}

				ImGui::Dummy(ImVec2(0.0f, 5.0f));

				//=================================================
				// 削除
				//=================================================

				if (ImGui::Button("Delete Point"))
				{
					// モデル削除
					if (i < m_pPointObj.size())
					{
						if (m_pPointObj[i])
						{
							m_pPointObj[i]->Destroy();
						}

						m_pPointObj.erase(m_pPointObj.begin() + i);
					}

					m_MovePoints.erase(m_MovePoints.begin() + i);

					// currentPoint補正
					if (m_currentPoint >= (int)m_MovePoints.size())
					{
						m_currentPoint = std::max(0, (int)m_MovePoints.size() - 1);
					}

					ImGui::TreePop();
					ImGui::PopID();

					break;
				}

				ImGui::TreePop();
			}

			ImGui::PopID();
		}

		ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 空白を空ける

		ImGui::TreePop(); // 閉じる
	}
}

//=============================================================================
// 自動移動ポイントの追加
//=============================================================================
void CRiftBlock::AddMovePoint(const D3DXVECTOR3& position)
{
	m_MovePoints.push_back(position);

#ifdef _DEBUG
	// デバッグ用自動移動ポイントモデルの生成
	CreateMovePointObject(position);
#endif
}

//=============================================================================
// ポイントデバッグ表示用モデル生成
//=============================================================================
void CRiftBlock::CreateMovePointObject(const D3DXVECTOR3& pos)
{
	// シーンの取得
	auto scene = CManager::Instance()->GetSceneManager()->GetScene();

	if (scene)
	{
		// ゲームオブジェクトの生成
		auto point = scene->CreateObject();

		// タグの設定
		point->SetTag("RiftMovePoint");

		// トランスフォームコンポーネントの追加
		auto transform = point->AddComponent<CTransformComponent>();

		transform->Position = pos;

		//-----------------------------------------
		// モデル描画
		//-----------------------------------------

		// モデルレンダーコンポーネントの追加
		auto model = point->AddComponent<CXModelRenderComponent>();

		// 描画順の設定
		model->SetLayer(RenderLayer::Model);

		// モデルのロード
		model->Load("data/MODELS/MovePoint.x");

		// ゲームオブジェクトの追加
		m_pPointObj.push_back(point);
	}
}

//=============================================================================
// JSONでの保存
//=============================================================================
void CRiftBlock::SaveToJson(json& b)
{
	CStageObject::SaveToJson(b);

	json pointArray = json::array();

	for (const auto& point : m_MovePoints)
	{
		pointArray.push_back({ point.x,point.y,point.z });
	}

	b["points"] = pointArray;

	b["start_point"] = m_StartMovePoint;
}

//=============================================================================
// JSONでの読み込み
//=============================================================================
void CRiftBlock::LoadFromJson(const json& b)
{
	CStageObject::LoadFromJson(b);

	for (auto obj : m_pPointObj)
	{
		if (obj)
		{
			obj->Uninit();
		}
	}

	m_pPointObj.clear();
	m_MovePoints.clear();

	if (b.contains("points"))
	{
		for (const auto& p : b["points"])
		{
			D3DXVECTOR3 point(p[0], p[1], p[2]);

			m_MovePoints.push_back(point);

#ifdef _DEBUG
			// ポイントデバッグ表示用モデルの生成
			CreateMovePointObject(point);
#endif
		}
	}

	if (b.contains("start_point"))
	{
		m_StartMovePoint = b.value("start_point", 0);
	}
}

CRainGrayBlock::CRainGrayBlock()
{

}

CRainGrayBlock::~CRainGrayBlock()
{

}

HRESULT CRainGrayBlock::Init(void)
{

	return S_OK;
}

void CRainGrayBlock::Update(void)
{
	// ステージオブジェクトの更新
	CStageObject::Update();
}

CRainOrangeBlock::CRainOrangeBlock()
{

}

CRainOrangeBlock::~CRainOrangeBlock()
{

}

HRESULT CRainOrangeBlock::Init(void)
{

	return S_OK;
}

void CRainOrangeBlock::Update(void)
{
	// ステージオブジェクトの更新
	CStageObject::Update();
}

CRainRedBlock::CRainRedBlock()
{

}

CRainRedBlock::~CRainRedBlock()
{

}

HRESULT CRainRedBlock::Init(void)
{

	return S_OK;
}

void CRainRedBlock::Update(void)
{
	// ステージオブジェクトの更新
	CStageObject::Update();
}

CRainYellowBlock::CRainYellowBlock()
{

}

CRainYellowBlock::~CRainYellowBlock()
{

}

HRESULT CRainYellowBlock::Init(void)
{

	return S_OK;
}

void CRainYellowBlock::Update(void)
{
	// ステージオブジェクトの更新
	CStageObject::Update();
}

CPodiumBlock::CPodiumBlock()
{

}

CPodiumBlock::~CPodiumBlock()
{

}

HRESULT CPodiumBlock::Init(void)
{

	return S_OK;
}

void CPodiumBlock::Update(void)
{
	// ステージオブジェクトの更新
	CStageObject::Update();
}
