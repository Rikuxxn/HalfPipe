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
	m_AimRadius			(400.0f)		// エイム可能な半径
{

}

//=============================================================================
// デストラクタ
//=============================================================================
CBasketBlock::~CBasketBlock()
{
	m_PointCircles.clear();
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

	// 50点：中心
	CreatePointCircle(m_CenterPosition, 4.0f/*, 50*/);

	// 30点：中間
	CreatePointCircle(m_CenterPosition, 6.0f/*, 30*/);

	// 10点：外側
	CreatePointCircle(m_CenterPosition, 9.0f/*, 10*/);

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

	// 得点サークルを更新
	for (auto* circle : m_PointCircles)
	{
		if (!circle)
		{
			continue;
		}

		auto circleTransform = circle->GetComponent<CTransformComponent>();

		if (!circleTransform)
		{
			continue;
		}

		circleTransform->Position = m_CenterPosition;
		circleTransform->Rotation = m_CenterRotation;
	}
}

//=============================================================================
// 得点サークル生成処理
//=============================================================================
CPointCircleBlock* CBasketBlock::CreatePointCircle(
	const D3DXVECTOR3& position, float scale)
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

	// モデル描画
	auto model = obj->AddComponent<CXModelRenderComponent>();

	if (!model)
	{
		return nullptr;
	}

	model->SetLayer(RenderLayer::Model);
	model->Load("data/MODELS/PointCircle.x");

	// 剛体
	auto rigid = obj->AddComponent<CRigidBodyComponent>();

	if (!rigid)
	{
		return nullptr;
	}

	rigid->SetCollisionFilter(
		COLLISION_GROUP_STAGE,
		COLLISION_GROUP_STAGE |
		COLLISION_GROUP_PLAYER |
		COLLISION_GROUP_GHOST |
		COLLISION_GROUP_ENEMY |
		COLLISION_GROUP_CAMERA
	);

	rigid->SetDynamic(false);

	// モデルサイズ取得
	D3DXVECTOR3 modelSize = model->GetModelSize();
	
	// シリンダーコライダーの生成
	rigid->CreateCylinder(modelSize, 1.0f);

	// 得点サークルを保存
	m_PointCircles.push_back(obj);

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

//=============================================================================
// コンストラクタ
//=============================================================================
CRiftBlock::CRiftBlock() :
	m_MoveDirection		(INIT_VEC3),	// 進行方向
	m_currentPoint		(0),			// 現在のポイント(エディタ用)
	m_StartMovePoint	(0),			// 移動開始ポイント
	m_AutoMoveSpeed		(50.0f)			// 移動スピード
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

	// ポイントが無い場合は飛ばす
	if (m_MovePoints.empty())
	{
		return;
	}

	// トランスフォームコンポーネントの取得
	auto transform = GetComponent<CTransformComponent>();

	if (!transform)
	{
		return;
	}

	D3DXVECTOR3 target = m_MovePoints[m_StartMovePoint];

	D3DXVECTOR3 pos = transform->Position;

	D3DXVECTOR3 dir = target - pos;

	// 方向ベクトルが有効な場合
	if (D3DXVec3LengthSq(&dir) > 0.0001f)
	{
		D3DXVec3Normalize(&dir, &dir);

		// 進行方向を保存
		m_MoveDirection = dir;
	}

	// 距離を求める
	float dist = D3DXVec3Length(&dir);

	// 到着
	if (dist < REACH_DIST)
	{
		m_StartMovePoint++;

		if (m_StartMovePoint >= (int)m_MovePoints.size())
		{
			m_StartMovePoint = 0;
		}
	}

	// 正規化
	D3DXVec3Normalize(&dir, &dir);

	// リジッドボディコンポーネントの取得
	auto rigid = GetComponent<CRigidBodyComponent>();

	if (!rigid)
	{
		return;
	}

	transform->Position = target;

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
