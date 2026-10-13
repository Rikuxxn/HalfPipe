//=============================================================================
//
// プレイヤー処理 [Player.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Player.h"
#include "Manager.h"
#include "Input.h"
#include "InputManager.h"
#include "XModelRenderComponent.h"
#include "CharacterComponent.h"
#include "PlayerState.h"
#include "imguimaneger.h"
#include "TransformComponent.h"
#include "Camera.h"
#include "TimeSystem.h"
#include "HpGaugeComponent.h"
#include "RigidBodyComponent.h"
#include "ColorConstants.h"
#include "MathUtility.h"
#include "RaycastComponent.h"
#include "MatrixUtility.h"
#include "NetWorkManager.h"
//#include "FBXManager.h"
//#include "FBXModelComponent.h"


// 名前空間の使用
using namespace ColorConst;
using namespace MathUtil;


//=============================================================================
// コンストラクタ
//=============================================================================
CPlayer::CPlayer() :
	m_rotDest		(INIT_VEC3),	// 目標の向き
	m_bIsMoving		(false),		// 移動しているか
	m_bControlFlag	(true)			// 移動許可フラグ
{

}

//=============================================================================
// デストラクタ
//=============================================================================
CPlayer::~CPlayer()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CPlayer::Init(void)
{
	//CFBXManager::ArgumentLoad arg;
	//arg.FBXFilePath = "data/MODELS/Fbx/test.fbx";
	//arg.FBXMotionFilePath.push_back({ "data/MODELS/Fbx/test_anim1.fbx", true });

	//// FBXモデルコンポーネントの追加
	//auto c = AddComponent<CFBXModelComponent>(arg);

	//// 初期モーションを設定しておく
	//c->SetMotion(CFBXModelComponent::MOTION_TYPE_00, 30);

	//=========================================================================
	// HPゲージ
	//=========================================================================

	auto hpGauge = AddComponent<CHpGaugeComponent>();

	if (!hpGauge)
	{
		return E_FAIL;
	}

	// 位置(オフセット)
	hpGauge->SetOffset({ 0.0f, 100.0f, 0.0f });

	// 背景
	hpGauge->SetBackgroundSize(103.0f, 13.0f);
	hpGauge->SetBackgroundColor(ColorConst::DX::LIGHT_GRAY);

	// ゲージのサイズ
	hpGauge->SetGaugeSize(100.0f, 10.0f);

	// ゲージの色
	hpGauge->SetGaugeColor(ColorConst::DX::RED);

	// 初期HP
	hpGauge->SetRate(1.0f);

	//// テクスチャの設定
	//hpGauge->SetTexture("data/TEXTURE/ball.png");

    // インスタンスのポインタを渡す
    m_stateMachine.Start(this);

    // 初期状態のステートをセット
    m_stateMachine.ChangeState<CPlayerStandState>();

    return S_OK;
}

//=============================================================================
// 更新処理
//=============================================================================
void CPlayer::Update(void)
{
	CInputKeyboard* pKeyboard = CManager::Instance()->GetInput()->GetKeyboard();

	// カメラの取得
	CCamera* pCamera = CManager::Instance()->GetCamera();

    // ゲームオブジェクトの更新
    CGameObject::Update();

#ifdef _DEBUG
	if (pCamera->GetMode() == CCamera::MODE_EDIT &&
		pKeyboard->GetTrigger(DIK_C) == true)
	{
		// 三人称カメラ
		pCamera->SetCamMode(CCamera::MODE_THIRD_PERSON);
	}
	else if (pCamera->GetMode() == CCamera::MODE_THIRD_PERSON &&
		pKeyboard->GetTrigger(DIK_C) == true)
	{
		// エディターカメラ
		pCamera->SetCamMode(CCamera::MODE_EDIT);
	}
#endif

    // ステートマシン更新
    m_stateMachine.Update();

	// 入力判定の取得
	InputData input = GatherInput();

	//============================================================
	// LocalPlayerなら入力をサーバーへ送信
	//============================================================
	if (m_IsLocalPlayer)
	{
		auto network = CManager::Instance()->GetNetworkManager();

		if (network && network->IsConnected())
		{
			network->SendPlayerInput(
				input.moveDir.x,
				input.moveDir.y,
				input.moveDir.z);
		}
	}

	// 移動入力があればプレイヤー向きを入力方向に
	if (input.moveDir.x != 0.0f || input.moveDir.z != 0.0f)
	{
		//// Y成分だけを使いたいので目標の向きを取得
		//D3DXVECTOR3 rotDest = m_rotDest;

		// Yを入力方向に向ける
		m_rotDest.y = atan2f(-input.moveDir.x, -input.moveDir.z);

		//// 目標の向きに設定する
		//SetRotDest(rotDest);
	}

	// トランスフォームコンポーネントの取得
	auto transform = GetComponent<CTransformComponent>();

	// 角度差
	float diff = m_rotDest.y - transform->Rotation.y;

	// -π～π に正規化
	while (diff > D3DX_PI)
	{
		diff -= D3DX_PI * 2.0f;
	}

	while (diff < -D3DX_PI)
	{
		diff += D3DX_PI * 2.0f;
	}

	// Time
	float dt = CManager::Instance()->GetTimeSystem()->GetDeltaTime();

	// フレームレート非依存補間係数
	float t = 1.0f - expf(-3.5f * dt);

	// 補間
	transform->Rotation.y += diff * t;


	//// レイキャストコンポーネントの取得
	//auto raycast = GetComponent<CRaycastComponent>();

	//D3DXVECTOR3 origin(transform->Position.x, transform->Position.y + 50.0f, transform->Position.z);

	//// 方向
	//D3DXVECTOR3 direction = MatrixUtil::GetForward(transform->GetWorldMatrix());

	//if (raycast->Raycast(origin, direction, 100.0f))
	//{
	//	CGameObject* hitObject = raycast->GetHitObject();

	//	if (hitObject)
	//	{
	//		// 当たったオブジェクト
	//		int n = 0;
	//	}
	//}
}

//=============================================================================
// リモート用モーション更新
//=============================================================================
void CPlayer::UpdateNetworkMotion(bool isMoving)
{
	auto character = GetComponent<CCharacterComponent>();

	if (!character)
	{
		return;
	}

	// 現在のモーションと同じなら何もしない
	if (m_NetworkMoving == isMoving)
	{
		return;
	}

	m_NetworkMoving = isMoving;

	if (m_NetworkMoving)
	{
		// 移動モーション
		character->PlayMotion(1, 10);
	}
	else
	{
		// 待機モーション
		character->PlayMotion(0, 10);
	}
}

#ifdef _DEBUG
//=============================================================================
// Imguiのデバッグ描画処理
//=============================================================================
void CPlayer::DrawDebugGUI(void)
{
    // トランスフォームコンポーネントの取得
    auto transform = GetComponent<CTransformComponent>();

    if (!transform)
    {
        return;
    }

    // プレイヤー情報
    if (ImGui::TreeNode("Player Info"))
    {
		ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 空白を空ける

		// リジッドボディコンポーネントの取得
		auto rigid = GetComponent<CRigidBodyComponent>();

		// ラジアン→角度に一時変換（静的変数で保持し操作中のみ更新）
		D3DXVECTOR3 degRot = D3DXToDegree(transform->Rotation);

		// 回転角度を - 180 ～ 180 度に正規化
		degRot.x = Normalize(degRot.x);
		degRot.y = Normalize(degRot.y);
		degRot.z = Normalize(degRot.z);

		bool isDynamic = rigid->IsDynamic();

		if (ImGui::Checkbox("is Dynamic", &isDynamic))
		{
			// true = Dynamic
			// false = Kinematic
			rigid->SetDynamic(isDynamic);
		}

		ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 空白を空ける

		//*********************************************************************
		// POS の情報
		//*********************************************************************

		// 位置
		ImGui::Text("POS"); ImGui::SameLine(70); // ラベル位置微調整

		ImGui::Text("X:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80); // 幅を狭く
		ImGui::DragFloat("##Player_pos_x", (float*)&transform->Position.x, 1.0f, -1000.0f, 1000.0f, "%.1f");

		ImGui::SameLine();
		ImGui::Text("Y:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##Player_pos_y", (float*)&transform->Position.y, 1.0f, -1000.0f, 1000.0f, "%.1f");

		ImGui::SameLine();
		ImGui::Text("Z:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##Player_pos_z", (float*)&transform->Position.z, 1.0f, -1000.0f, 1000.0f, "%.1f");

		//*********************************************************************
		// ROT の情報
		//*********************************************************************

		ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 空白を空ける

		// 向き
		ImGui::Text("ROT"); ImGui::SameLine(70); // ラベル位置微調整

		ImGui::Text("X:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		bool changedX = ImGui::DragFloat("##Player_rot_x", &degRot.x, 0.1f, -180.0f, 180.0f, "%.2f");

		ImGui::SameLine();
		ImGui::Text("Y:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		bool changedY = ImGui::DragFloat("##Player_rot_y", &degRot.y, 0.1f, -180.0f, 180.0f, "%.2f");

		ImGui::SameLine();
		ImGui::Text("Z:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		bool changedZ = ImGui::DragFloat("##Player_rot_z", &degRot.z, 0.1f, -180.0f, 180.0f, "%.2f");

		bool isRotChanged = changedX || changedY || changedZ;

		//*********************************************************************
		// SIZE の情報
		//*********************************************************************

		ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 空白を空ける

		// ラベル
		ImGui::Text("SIZE"); ImGui::SameLine(70); // ラベルの位置ちょっと調整

		// X
		ImGui::Text("X:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##Player_size_x", (float*)&transform->Scale.x, 0.1f, -100.0f, 100.0f, "%.1f");

		// Y
		ImGui::SameLine();
		ImGui::Text("Y:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##Player_size_y", (float*)&transform->Scale.y, 0.1f, -100.0f, 100.0f, "%.1f");

		// Z
		ImGui::SameLine();
		ImGui::Text("Z:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##Player_size_z", (float*)&transform->Scale.z, 0.1f, -100.0f, 100.0f, "%.1f");

		ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 空白を空ける

		// 角度→ラジアンに戻す
		D3DXVECTOR3 rotRad = D3DXToRadian(degRot);

		if (isRotChanged)
		{
			// 静的ブロックなら即物理Transformも更新して同期
			if (!isDynamic)
			{
				// 回転が変わった時だけセット
				transform->Rotation = rotRad;

				D3DXMATRIX matRot;
				D3DXMatrixRotationYawPitchRoll(&matRot, rotRad.y, rotRad.x, rotRad.z);

				D3DXQUATERNION dq;
				D3DXQuaternionRotationMatrix(&dq, &matRot);

				transform->Quaternion = { dq.x, dq.y, dq.z, dq.w };
			}
		}
		else
		{
			degRot = D3DXToDegree(transform->Rotation);
		}

		ImGui::TreePop(); // 閉じる
    }
}
#endif

//=============================================================================
// 入力判定取得関数
//=============================================================================
CPlayer::InputData CPlayer::GatherInput(void)
{
	InputData input{};
	input.moveDir = INIT_VEC3;

	// ローカルプレイヤーではない場合
	// ネットワークから受信した入力を使用
	if (!m_IsLocalPlayer)
	{
		return m_NetworkInput;
	}

	CInputKeyboard* pKeyboard = CManager::Instance()->GetInput()->GetKeyboard();// キーボードの取得
	CInputJoypad* pJoypad = CManager::Instance()->GetInput()->GetJoypad();		// ジョイパッドの取得
	XINPUT_STATE* pStick = pJoypad->GetStickAngle();							// スティックの取得
	CCamera* pCamera = CManager::Instance()->GetCamera();						// カメラの取得
	D3DXVECTOR3 CamRot = pCamera->GetRot();										// カメラ角度の取得

	if (this == nullptr || !m_bControlFlag)
	{
		return input;
	}

	//---------------------------
	// ゲームパッド入力(左スティック)
	//---------------------------
	if (pJoypad->GetStick() && pStick)
	{
		// 左スティックの取得
		float stickX = pStick->Gamepad.sThumbLX;
		float stickY = pStick->Gamepad.sThumbLY;
		float magnitude = sqrtf(stickX * stickX + stickY * stickY);

		if (magnitude >= CInputJoypad::DEADZONE)
		{
			stickX /= magnitude;
			stickY /= magnitude;

			float normMag =
				std::min((magnitude - CInputJoypad::DEADZONE) / (CInputJoypad::LSTICK_VALUE - CInputJoypad::DEADZONE), 1.0f);

			stickX *= normMag;
			stickY *= normMag;

			D3DXVECTOR3 dir;
			float yaw = CamRot.y;

			dir.x = -(stickX * cosf(yaw) + stickY * sinf(yaw));
			dir.z = stickX * sinf(-yaw) + stickY * cosf(yaw);
			dir.z = -dir.z;

			input.moveDir += D3DXVECTOR3(dir.x, 0, dir.z);
		}
	}

	//---------------------------
	// キーボード入力
	//---------------------------
	if (pKeyboard->GetPress(DIK_W))
	{
		input.moveDir += D3DXVECTOR3(-sinf(CamRot.y), 0, -cosf(CamRot.y));
	}
	if (pKeyboard->GetPress(DIK_S))
	{
		input.moveDir += D3DXVECTOR3(sinf(CamRot.y), 0, cosf(CamRot.y));
	}
	if (pKeyboard->GetPress(DIK_A))
	{
		input.moveDir += D3DXVECTOR3(cosf(CamRot.y), 0, -sinf(CamRot.y));
	}
	if (pKeyboard->GetPress(DIK_D))
	{
		input.moveDir += D3DXVECTOR3(-cosf(CamRot.y), 0, sinf(CamRot.y));
	}

	// 正規化
	if (input.moveDir.x != 0.0f || input.moveDir.z != 0.0f)
	{
		D3DXVec3Normalize(&input.moveDir, &input.moveDir);
	}

	return input;
}

//=============================================================================
// ステート用のフラグ更新処理
//=============================================================================
void CPlayer::UpdateMovementFlags(const D3DXVECTOR3& moveDir)
{
	m_bIsMoving = (moveDir.x != 0.0f || moveDir.z != 0.0f);
}

//=============================================================================
// 衝突判定
//=============================================================================
void CPlayer::OnCollisionEnter(CGameObject* other)
{
	if (other->CompareTag("Wall"))
	{
		//int n = 0;
	}
}

void CPlayer::OnCollisionStay(CGameObject* other)
{
	if (other->CompareTag("Floor"))
	{

	}
}

void CPlayer::OnCollisionExit(CGameObject* other)
{
	if (other->CompareTag("Florr"))
	{

	}
}
