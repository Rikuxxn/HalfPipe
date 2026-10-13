//=============================================================================
//
// ステージオブジェクト処理 [StageObject.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "StageObject.h"
#include "TransformComponent.h"
#include "RigidBodyComponent.h"


//=============================================================================
// デストラクタ
//=============================================================================
CStageObject::~CStageObject()
{

}

//=============================================================================
// 更新処理
//=============================================================================
void CStageObject::Update(void)
{
	// ゲームオブジェクトの更新処理
	CGameObject::Update();
}

//=============================================================================
// オブジェクト保存
//=============================================================================
void CStageObject::SaveToJson(json& j)
{
    // トランスフォームコンポーネントの取得
    auto transform = GetComponent<CTransformComponent>();

	// リジッドボディコンポーネントの取得
	auto rigid = GetComponent<CRigidBodyComponent>();

    D3DXVECTOR3 degRot = D3DXToDegree(transform->Rotation);
    j["type"]   = static_cast<int>(GetType());
    j["pos"]    = { transform->Position.x, transform->Position.y, transform->Position.z };
    j["rot"]    = { degRot.x, degRot.y, degRot.z };
    j["scale"]  = { transform->Scale.x, transform->Scale.y, transform->Scale.z };
    //j["collider_size"] = { m_colliderSize.x, m_colliderSize.y, m_colliderSize.z };
    j["is_dynamic"] = rigid->IsDynamic();
}

//=============================================================================
// オブジェクト読み込み
//=============================================================================
void CStageObject::LoadFromJson(const json& j)
{
	// トランスフォームコンポーネントの取得
	auto transform = GetComponent<CTransformComponent>();

	// リジッドボディコンポーネントの取得
	auto rigid = GetComponent<CRigidBodyComponent>();

	transform->Position = { j["pos"][0], j["pos"][1], j["pos"][2] };
	D3DXVECTOR3 degRot(j["rot"][0], j["rot"][1], j["rot"][2]);
	transform->Rotation = D3DXToRadian(degRot);

	D3DXMATRIX matRot;

	D3DXMatrixRotationYawPitchRoll(&matRot, transform->Rotation.y, transform->Rotation.x, transform->Rotation.z);

	D3DXQUATERNION q;
	D3DXQuaternionRotationMatrix(&q, &matRot);

	transform->Quaternion = { q.x, q.y, q.z, q.w };

	transform->Scale = { j["scale"][0], j["scale"][1], j["scale"][2] };

	if (j.contains("is_dynamic"))
	{
		rigid->SetDynamic(j["is_dynamic"]);
	}

	if (rigid)
	{
		// 当たり判定更新
		rigid->UpdateFromScale(transform->Scale);

		// 位置更新
		rigid->Transform(); 
	}

	//if (b.contains("collider_size"))
	//{
	//	D3DXVECTOR3 collider(
	//		b["collider_size"][0],
	//		b["collider_size"][1],
	//		b["collider_size"][2]);
	//	SetColliderSize(collider);
	//	UpdateCollider();
	//}
}
