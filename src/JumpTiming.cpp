//=============================================================================
//
// ジャンプタイミングUI処理 [JumpTiming.cpp]
// Author : SOICHIRO SASAKI
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "JumpTiming.h"
#include "Manager.h"
#include "SceneManager.h"
#include "Scene.h"
#include "UIComponent.h"
#include "SpriteRenderComponent.h"



//=============================================================================
// コンストラクタ
//=============================================================================
CJumpTiming::CJumpTiming():
	m_frame			(nullptr),
	m_circleButton	(nullptr)
{

}

//=============================================================================
// デストラクタ
//=============================================================================
CJumpTiming::~CJumpTiming()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CJumpTiming::Init(void)
{
	////---------------------------------------------------
	// アニメーション付きUIの生成
	//---------------------------------------------------
	auto scene = CManager::Instance()->GetSceneManager()->GetScene();
	if (!scene)
	{
		return E_FAIL;
	}
	// タイトルロゴオブジェクトの生成
	m_circleButton = scene->CreateObject();

	m_circleButton->SetTag("TimingA");

	// UIコンポーネント
	auto uiA = m_circleButton->AddComponent<CUIComponent>();

	auto& uiATransform = uiA->GetTransform();
	uiATransform.Position = { 0.5f, 0.5f };
	uiATransform.Alpha = 1.0f;
	uiATransform.Scale = { 0.3f, 0.3f };

	// スプライトコンポーネントに追加
	auto spriteA = m_circleButton->AddComponent<CSpriteRenderComponent>();

	// 描画順の設定
	spriteA->SetLayer(RenderLayer::UI);

	// テクスチャの設定
	spriteA->SetTexture("data/TEXTURE/timingcircle_frame.png");

	spriteA->SetSize(1.0f, 1.0f);

	//// タイトルロゴオブジェクトの生成
	//m_frame = scene->CreateObject();

	//m_frame->SetTag("TimingB");

	//// UIコンポーネント
	//auto uiB = m_frame->AddComponent<CUIComponent>();

	//auto& uiBTransform = uiB->GetTransform();
	//uiBTransform.Position = { 0.5f, 0.5f };
	//uiBTransform.Alpha = 1.0f;
	//uiBTransform.Scale = { 0.1f, 0.1f };

	//// スプライトコンポーネントに追加
	//auto spriteB = m_frame->AddComponent<CSpriteRenderComponent>();

	//// 描画順の設定
	//spriteB->SetLayer(RenderLayer::UI);

	//// テクスチャの設定
	//spriteB->SetTexture("data/TEXTURE/timingcircle.png");


	return S_OK;
}

//=============================================================================
// 更新処理
//=============================================================================
void CJumpTiming::Update(void)
{

}

#ifdef _DEBUG
//=============================================================================
// Imguiのデバッグ描画処理
//=============================================================================
void CJumpTiming::DrawDebugGUI(void)
{
	//// トランスフォームコンポーネントの取得
	//auto transform = GetComponent<CTransformComponent>();

	//if (!transform)
	//{
	//	return;
	//}

	//// プレイヤー情報
	//if (ImGui::TreeNode("Player Info"))
	//{
	//	ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 空白を空ける

	//	// リジッドボディコンポーネントの取得
	//	auto rigid = GetComponent<CRigidBodyComponent>();

	//	// ラジアン→角度に一時変換（静的変数で保持し操作中のみ更新）
	//	D3DXVECTOR3 degRot = D3DXToDegree(transform->Rotation);

	//	// 回転角度を - 180 ～ 180 度に正規化
	//	degRot.x = Normalize(degRot.x);
	//	degRot.y = Normalize(degRot.y);
	//	degRot.z = Normalize(degRot.z);

	//	bool isDynamic = rigid->IsDynamic();

	//	if (ImGui::Checkbox("is Dynamic", &isDynamic))
	//	{
	//		// true = Dynamic
	//		// false = Kinematic
	//		rigid->SetDynamic(isDynamic);
	//	}

	//	ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 空白を空ける

	//	//*********************************************************************
	//	// POS の情報
	//	//*********************************************************************

	//	// 位置
	//	ImGui::Text("POS"); ImGui::SameLine(70); // ラベル位置微調整

	//	ImGui::Text("X:"); ImGui::SameLine();
	//	ImGui::SetNextItemWidth(80); // 幅を狭く
	//	ImGui::DragFloat("##Player_pos_x", (float*)&transform->Position.x, 1.0f, -7000.0f, 7000.0f, "%.1f");

	//	ImGui::SameLine();
	//	ImGui::Text("Y:"); ImGui::SameLine();
	//	ImGui::SetNextItemWidth(80);
	//	ImGui::DragFloat("##Player_pos_y", (float*)&transform->Position.y, 1.0f, -7000.0f, 7000.0f, "%.1f");

	//	ImGui::SameLine();
	//	ImGui::Text("Z:"); ImGui::SameLine();
	//	ImGui::SetNextItemWidth(80);
	//	ImGui::DragFloat("##Player_pos_z", (float*)&transform->Position.z, 1.0f, -7000.0f, 7000.0f, "%.1f");

	//	//*********************************************************************
	//	// ROT の情報
	//	//*********************************************************************

	//	ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 空白を空ける

	//	// 向き
	//	ImGui::Text("ROT"); ImGui::SameLine(70); // ラベル位置微調整

	//	ImGui::Text("X:"); ImGui::SameLine();
	//	ImGui::SetNextItemWidth(80);
	//	bool changedX = ImGui::DragFloat("##Player_rot_x", &degRot.x, 0.1f, -180.0f, 180.0f, "%.2f");

	//	ImGui::SameLine();
	//	ImGui::Text("Y:"); ImGui::SameLine();
	//	ImGui::SetNextItemWidth(80);
	//	bool changedY = ImGui::DragFloat("##Player_rot_y", &degRot.y, 0.1f, -180.0f, 180.0f, "%.2f");

	//	ImGui::SameLine();
	//	ImGui::Text("Z:"); ImGui::SameLine();
	//	ImGui::SetNextItemWidth(80);
	//	bool changedZ = ImGui::DragFloat("##Player_rot_z", &degRot.z, 0.1f, -180.0f, 180.0f, "%.2f");

	//	bool isRotChanged = changedX || changedY || changedZ;

	//	//*********************************************************************
	//	// SIZE の情報
	//	//*********************************************************************

	//	ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 空白を空ける

	//	// ラベル
	//	ImGui::Text("SIZE"); ImGui::SameLine(70); // ラベルの位置ちょっと調整

	//	// X
	//	ImGui::Text("X:"); ImGui::SameLine();
	//	ImGui::SetNextItemWidth(80);
	//	ImGui::DragFloat("##Player_size_x", (float*)&transform->Scale.x, 0.1f, -100.0f, 100.0f, "%.1f");

	//	// Y
	//	ImGui::SameLine();
	//	ImGui::Text("Y:"); ImGui::SameLine();
	//	ImGui::SetNextItemWidth(80);
	//	ImGui::DragFloat("##Player_size_y", (float*)&transform->Scale.y, 0.1f, -100.0f, 100.0f, "%.1f");

	//	// Z
	//	ImGui::SameLine();
	//	ImGui::Text("Z:"); ImGui::SameLine();
	//	ImGui::SetNextItemWidth(80);
	//	ImGui::DragFloat("##Player_size_z", (float*)&transform->Scale.z, 0.1f, -100.0f, 100.0f, "%.1f");

	//	ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 空白を空ける

	//	// 角度→ラジアンに戻す
	//	D3DXVECTOR3 rotRad = D3DXToRadian(degRot);

	//	if (isRotChanged)
	//	{
	//		// 静的ブロックなら即物理Transformも更新して同期
	//		if (!isDynamic)
	//		{
	//			// 回転が変わった時だけセット
	//			transform->Rotation = rotRad;

	//			D3DXMATRIX matRot;
	//			D3DXMatrixRotationYawPitchRoll(&matRot, rotRad.y, rotRad.x, rotRad.z);

	//			D3DXQUATERNION dq;
	//			D3DXQuaternionRotationMatrix(&dq, &matRot);

	//			transform->Quaternion = { dq.x, dq.y, dq.z, dq.w };
	//		}
	//	}
	//	else
	//	{
	//		degRot = D3DXToDegree(transform->Rotation);
	//	}

	//	ImGui::TreePop(); // 閉じる

	//	// 自動移動コンポーネントの取得
	//	auto autoMove = GetComponent<CAutoMoveComponent>();

	//	if (autoMove)
	//	{
	//		// 自動移動ポイントエディターの描画
	//		autoMove->DrawMovePointEditor();
	//	}
	//}
}
#endif

