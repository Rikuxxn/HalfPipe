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
#include "RigidBodyComponent.h"
#include "ColorConstants.h"
#include "MathUtility.h"
#include "MathConstants.h"
#include "RaycastComponent.h"
#include "MatrixUtility.h"
#include "NetWorkManager.h"
#include "AutoMoveComponent.h"
#include "SceneManager.h"
#include "Scene.h"
#include "ThrowComponent.h"
#include "AimComponent.h"
//#include "FBXManager.h"
//#include "FBXModelComponent.h"


// 名前空間の使用
using namespace ColorConst;
using namespace MathUtil;
using namespace MathConst;
using namespace MatrixUtil;


//=============================================================================
// コンストラクタ
//=============================================================================
CPlayer::CPlayer() :
	m_rotDest			(INIT_VEC3),			// 目標の向き
	m_bIsMoving			(false),				// 移動しているか
	m_bControlFlag		(true),					// 移動許可フラグ
	m_SurfaceNormal		({ 0, 1, 0 }),			// 面の法線
    m_SurfaceForward	({ 0, 0, 1 }),			// 面に沿う進行方向
    m_IsGrounded		(false),				// 接地中か
    m_WasGrounded		(false),				// 前フレームの接地状態
    m_AlignToSurface	(true)					// 面に姿勢を合わせるか
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
	// 自動移動コンポーネントの追加
	auto autoMove = AddComponent<CAutoMoveComponent>();

	// 投擲コンポーネントの追加
	AddComponent<CThrowComponent>();

	// 自動移動ポイントの読み込み
	autoMove->LoadMovePoint("data/MovePointInfo01.json");

	// 自動移動開始
	autoMove->StartAutoMove();

	// エイムコンポーネントの追加
	auto aim = AddComponent<CAimComponent>();

	if (!aim)
	{
		return E_FAIL;
	}

	// エイムは最初は非表示
	aim->InvisibleAim();

	// レイキャストコンポーネントの追加
	AddComponent<CRaycastComponent>();

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
	CInputJoypad* pJoypad = CManager::Instance()->GetInput()->GetJoypad();

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

	// デルタタイムの取得
	float dt = CManager::Instance()->GetTimeSystem()->GetDeltaTime();

	// エイムコンポーネント取得
	auto aim = GetComponent<CAimComponent>();

	// 自動移動コンポーネントの取得
	auto autoMove = GetComponent<CAutoMoveComponent>();

	if (aim && autoMove->IsJump())
	{
		aim->MoveAim(input.moveDir, dt);

		// エイムを表示
		aim->VisibleAim();
	}
	else
	{
		// エイムを非表示
		aim->InvisibleAim();
	}

	// 玉の投擲
	if (autoMove->IsJump() && 
		(pKeyboard->GetTrigger(DIK_F) || pJoypad->GetTriggerR2()))
	{
		// 投擲コンポーネントの取得
		auto ballThrow = GetComponent<CThrowComponent>();

		if (ballThrow)
		{
			ballThrow->Throw();
		}
	}

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

	// ハーフパイプの面に合わせた姿勢制御
	UpdateSurfaceAlignment(dt);
}

//=============================================================================
// ハーフパイプの面に姿勢を合わせる
//=============================================================================
void CPlayer::UpdateSurfaceAlignment(float deltaTime)
{
	// コンポーネント取得
	auto ray = GetComponent<CRaycastComponent>();
	auto transform = GetComponent<CTransformComponent>();

	if (!ray || !transform)
	{
		return;
	}

	// 前フレームの接地状態を保存
	m_WasGrounded = m_IsGrounded;

	// プレイヤーの位置
	D3DXVECTOR3 position = transform->Position;

	// プレイヤーの少し上から下向きにレイを飛ばす
	D3DXVECTOR3 origin = position;
	origin.y += 50.0f;

	D3DXVECTOR3 direction(0.0f, -1.0f, 0.0f);

	// レイキャスト
	bool hit = ray->Raycast(origin, direction, 100.0f);

	m_IsGrounded = false;

	if (hit)
	{
		CGameObject* hitObject = ray->GetHitObject();

		// 必要に応じて、ハーフパイプ以外のオブジェクトを除外する
		if (hitObject)
		{
			m_SurfaceNormal = ray->GetHitNormal();

			// 正規化
			D3DXVec3Normalize(&m_SurfaceNormal, &m_SurfaceNormal);

			m_IsGrounded = true;
		}
	}

	// 空中では面に合わせた姿勢制御をしない
	if (!m_AlignToSurface || !m_IsGrounded)
	{
		return;
	}

	//=============================================================================
	// 面の法線に沿う姿勢を計算する
	//=============================================================================

	// 面の法線をUp方向として使用する
	D3DXVECTOR3 up = m_SurfaceNormal;

	// 自動移動コンポーネントを取得
	auto autoMove = GetComponent<CAutoMoveComponent>();

	if (!autoMove)
	{
		return;
	}

	// 実際の進行方向を取得
	D3DXVECTOR3 forward = -autoMove->GetMoveDirection();

	// 方向ベクトルが有効か確認
	if (D3DXVec3LengthSq(&forward) < 0.0001f)
	{
		return;
	}

	// 進行方向から、面の法線方向の成分を取り除く
	// これによって進行方向を面に沿わせる
	float dot = D3DXVec3Dot(&forward, &up);

	forward -= up * dot;

	// 投影後のベクトルが十分な長さを持つか確認する
	if (D3DXVec3LengthSq(&forward) > 0.0001f)
	{
		// 正規化
		D3DXVec3Normalize(&forward, &forward);

		// 右方向を計算する
		D3DXVECTOR3 right;
		D3DXVec3Cross(&right, &up, &forward);

		if (D3DXVec3LengthSq(&right) > 0.0001f)
		{
			D3DXVec3Normalize(&right, &right);

			// 直交性を保つため、Forwardを再計算する
			D3DXVec3Cross(&forward, &right, &up);
			D3DXVec3Normalize(&forward, &forward);

			// 面に沿う方向を保存
			m_SurfaceForward = forward;

			//=============================================================
			// 面の法線と進行方向から回転行列を作成
			//=============================================================
			D3DXMATRIX rotationMatrix = Identity();

			// Right方向
			rotationMatrix._11 = right.x;
			rotationMatrix._12 = right.y;
			rotationMatrix._13 = right.z;

			// Up方向
			rotationMatrix._21 = up.x;
			rotationMatrix._22 = up.y;
			rotationMatrix._23 = up.z;

			// Forward方向
			rotationMatrix._31 = forward.x;
			rotationMatrix._32 = forward.y;
			rotationMatrix._33 = forward.z;

			//=============================================================
			// 回転行列をクォータニオンに変換
			//=============================================================
			D3DXQUATERNION surfaceRotation = MatrixToQuaternion(rotationMatrix);

			D3DXQUATERNION currentQuaternion = transform->Quaternion;
			D3DXQUATERNION targetQuaternion = surfaceRotation;

			// 補間率
			float t = 1.0f - expf(-5.0f * deltaTime);

			// クォータニオン補間
			D3DXQUATERNION resultQuaternion;

			D3DXQuaternionSlerp(
				&resultQuaternion,
				&currentQuaternion,
				&targetQuaternion,
				t
			);

			D3DXQuaternionNormalize(
				&resultQuaternion,
				&resultQuaternion
			);

			//=============================================================
			// トランスフォームに反映
			//=============================================================
			transform->Quaternion = resultQuaternion;
			transform->UseQuaternion = true;
		}
	}
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
		ImGui::DragFloat("##Player_pos_x", (float*)&transform->Position.x, 1.0f, -7000.0f, 7000.0f, "%.1f");

		ImGui::SameLine();
		ImGui::Text("Y:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##Player_pos_y", (float*)&transform->Position.y, 1.0f, -7000.0f, 7000.0f, "%.1f");

		ImGui::SameLine();
		ImGui::Text("Z:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##Player_pos_z", (float*)&transform->Position.z, 1.0f, -7000.0f, 7000.0f, "%.1f");

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

		// 自動移動コンポーネントの取得
		auto autoMove = GetComponent<CAutoMoveComponent>();

		if (autoMove)
		{
			// 自動移動ポイントエディターの描画
			autoMove->DrawMovePointEditor();
		}
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
	//if (!m_IsLocalPlayer)
	//{
	//	// サーバーから受信した入力を使用する
	//	return m_NetworkInput;
	//}

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