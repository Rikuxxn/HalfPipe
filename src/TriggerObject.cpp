//=============================================================================
//
// トリガーオブジェクト処理 [TriggerObject.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "TriggerObject.h"
#include "TransformComponent.h"
#include "GhostObjectComponent.h"


//=============================================================================
// デストラクタ
//=============================================================================
CTriggerObject::~CTriggerObject()
{

}

//=============================================================================
// 更新処理
//=============================================================================
void CTriggerObject::Update(void)
{
	// ゲームオブジェクトの更新処理
	CGameObject::Update();
}

//=============================================================================
// オブジェクト保存
//=============================================================================
void CTriggerObject::SaveToJson(json& j)
{
	// トランスフォームコンポーネントの取得
	auto transform = GetComponent<CTransformComponent>();

	D3DXVECTOR3 degRot = D3DXToDegree(transform->Rotation);
	j["type"] = static_cast<int>(GetType());
	j["pos"] = { transform->Position.x, transform->Position.y, transform->Position.z };
	j["rot"] = { degRot.x, degRot.y, degRot.z };
	j["scale"] = { transform->Scale.x, transform->Scale.y, transform->Scale.z };
}

//=============================================================================
// オブジェクト読み込み
//=============================================================================
void CTriggerObject::LoadFromJson(const json& j)
{
	// トランスフォームコンポーネントの取得
	auto transform = GetComponent<CTransformComponent>();

	// ゴーストオブジェクトコンポーネントの取得
	auto ghost = GetComponent<CGhostObjectComponent>();

	transform->Position = { j["pos"][0], j["pos"][1], j["pos"][2] };
	D3DXVECTOR3 degRot(j["rot"][0], j["rot"][1], j["rot"][2]);
	transform->Rotation = D3DXToRadian(degRot);

	D3DXMATRIX matRot;

	D3DXMatrixRotationYawPitchRoll(&matRot, transform->Rotation.y, transform->Rotation.x, transform->Rotation.z);

	D3DXQUATERNION q;
	D3DXQuaternionRotationMatrix(&q, &matRot);

	transform->Quaternion = { q.x, q.y, q.z, q.w };

	transform->Scale = { j["scale"][0], j["scale"][1], j["scale"][2] };

	if (ghost)
	{
		// 当たり判定更新
		ghost->UpdateFromScale(transform->Scale);

		// 位置更新
		ghost->Transform();
	}
}
