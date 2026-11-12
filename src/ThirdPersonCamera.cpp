//=============================================================================
//
// 三人称カメラ処理 [ThirdPersonCamera.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "ThirdPersonCamera.h"
#include "MathConstants.h"
#include "MathUtility.h"
#include "Manager.h"
#include "Input.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Player.h"
#include "TransformComponent.h"
#include "CollisionGroup.h"
#include "AutoMoveComponent.h"
#include "AimComponent.h"


// 名前空間の使用
using namespace MathConst;
using namespace MathUtil;



//=============================================================================
// コンストラクタ
//=============================================================================
CThirdPersonCamera::CThirdPersonCamera() :
	m_fDistance(50.0f)
{

}

//=============================================================================
// 更新処理
//=============================================================================
void CThirdPersonCamera::Update(CameraTransform& transform)
{
	// マウスの取得
	CInputMouse* pInputMouse = CManager::Instance()->GetInput()->GetMouse();

	// スティックの取得
	XINPUT_STATE* pStick = CManager::Instance()->GetInput()->GetJoypad()->GetStickAngle();

	auto scene = CManager::Instance()->GetSceneManager()->GetScene();

	// プレイヤーの取得
	CPlayer* pPlayer = nullptr;

	// ローカルプレイヤーを検索
	for (auto& object : scene->GetObjects())
	{
		auto player = dynamic_cast<CPlayer*>(object.get());

		if (player && player->IsLocalPlayer())
		{
			pPlayer = player;
			break;
		}
	}

	if (!pPlayer)
	{
		return;
	}

	if (pPlayer != nullptr && pPlayer->IsLocalPlayer())
	{
		// プレイヤーのトランスフォームを取得
		auto playerTransform = pPlayer->GetComponent<CTransformComponent>();
		// プレイヤーの自動移動を取得
		auto playerAutoMove = pPlayer->GetComponent<CAutoMoveComponent>();

		// プレイヤーの位置の取得
		D3DXVECTOR3 playerPos = playerTransform->Position;
		D3DXVECTOR3 playerRot = playerTransform->Rotation;

		//// マウスの状態を取得
		//DIMOUSESTATE mouseState;

		//// マウスカーソルを非表示にする
		//pInputMouse->SetCursorVisibility(false);

		//// ゲームパッド右スティックカメラ操作
		//if (pStick != nullptr)
		//{
		//	// 右スティックの値を取得
		//	float stickX = pStick->Gamepad.sThumbRX;
		//	float stickY = pStick->Gamepad.sThumbRY;

		//	// デッドゾーン処理
		//	const float DEADZONE = 10922.0f;
		//	if (fabsf(stickX) < DEADZONE)
		//	{
		//		stickX = 0.0f;
		//	}
		//	if (fabsf(stickY) < DEADZONE)
		//	{
		//		stickY = 0.0f;
		//	}

		//	// 正規化
		//	stickX /= 32768.0f;
		//	stickY /= 32768.0f;

		//	// カメラ回転の更新
		//	CameraWithGamepad(transform, stickX, stickY);
		//}
		//if (pInputMouse->GetMouseState(&mouseState))
		//{
		//	// 前フレームのカーソル位置を記録する静的変数
		//	static POINT prevCursorPos = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };

		//	// 現在のマウスの位置の取得
		//	POINT cursorPos;
		//	GetCursorPos(&cursorPos);

		//	// マウス感度
		//	const float mouseSensitivity = 0.003f;

		//	float deltaX = (float)(cursorPos.x - prevCursorPos.x) * mouseSensitivity;
		//	float deltaY = (float)(cursorPos.y - prevCursorPos.y) * mouseSensitivity;

		//	prevCursorPos = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
		//	SetCursorPos(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

		//	transform.rotation.y += deltaX;
		//	transform.rotation.x += deltaY;

		//	// 上下回転制限
		//	if (transform.rotation.x < -1.3f)
		//	{
		//		transform.rotation.x = -1.3f;
		//	}
		//	if (transform.rotation.x > 1.3f)
		//	{
		//		transform.rotation.x = 1.3f;
		//	}
		//}
		D3DXVECTOR3 dir = playerAutoMove->GetMoveDirection();
		// カメラ位置計算
		//transform.posV.x = playerPos.x + m_fDistance * -dir.x;
		//transform.posV.y = playerPos.y + m_fDistance + 80.0f* -dir.y;
		//transform.posV.z = playerPos.z + m_fDistance * -dir.z;

		float fCurrentTime = playerAutoMove->GetSplineDelta();
		static float nTimeCount = 0.0f;
		static bool bJumpUp = false;
		nTimeCount += fCurrentTime;

		float dirX = playerPos.x - transform.posV.x;
		float dirZ = playerPos.z - transform.posV.z;
		float rotY = atan2f(dirX,dirZ);

		transform.rotation.y = rotY > 0.0f ? -D3DX_PI * 0.5f : D3DX_PI * 0.5f;

		if (!playerAutoMove->IsJump())
		{
			nTimeCount = 0.0f;
			bJumpUp=false;
		}

		if (!playerAutoMove->IsJump())
		{
			transform.posV.x += (dirX) * 0.05f;
			transform.posV.y += ((playerPos.y + 60.0f) - transform.posV.y) * 0.05f;
			transform.posV.z += (playerPos.z - transform.posV.z) * 0.05f;
			// 注視点
			transform.posR = playerPos;
			transform.posR.y += 60.0f;


		}
		else if (nTimeCount >1.5f)
		{
			transform.posV.x += (dirX) * 0.05f;
			transform.posV.y += ((playerPos.y + 60.0f) - transform.posV.y) * 0.05f;
			transform.posV.z += (playerPos.z - transform.posV.z) * 0.05f;
			// 注視点
			transform.posR = playerPos;
			transform.posR.y += 60.0f;

		}
		else
		{
			// プレイヤーの自動移動を取得
			auto Aim = pPlayer->GetComponent<CAimComponent>();
			D3DXVECTOR3 targetCenter = Aim->GetTargetCenter();

			transform.posV.x += (dirX) * 0.001f;
			transform.posV.y += ((playerPos.y + 100.0f) - transform.posV.y) * 0.1f;
			transform.posV.z += (dirZ) * 0.1f;
			// 注視点

			transform.posR = targetCenter;
			transform.posR.y += 20.0f;
		}



		// カメラの位置補正処理
		//AdjustCameraPosition(transform, playerPos);
	}
}

//=============================================================================
// ゲームパッドのカメラ回転処理
//=============================================================================
void CThirdPersonCamera::CameraWithGamepad(CameraTransform& transform, float stickX, float stickY)
{
	const float rotationSpeed = 0.07f; // 回転速度

	 // 水平方向の回転（Y軸）
	transform.rotation.y += stickX * rotationSpeed;

	// 垂直方向の回転（X軸）
	transform.rotation.x -= stickY * rotationSpeed;

	// 垂直方向の回転を制限 (-90度 ～ 90度)
	if (transform.rotation.x > PI_HALF)
	{
		transform.rotation.x = PI_HALF;
	}
	if (transform.rotation.x < -PI_HALF)
	{
		transform.rotation.x = -PI_HALF;
	}

	// 水平方向の回転を正規化 (-π ～ π)
	if (transform.rotation.y > D3DX_PI)
	{
		transform.rotation.y -= D3DX_PI * 2.0f;
	}
	else if (transform.rotation.y < -D3DX_PI)
	{
		transform.rotation.y += D3DX_PI * 2.0f;
	}
}

//=============================================================================
// カメラの位置補正(壁貫通をなくす)処理
//=============================================================================
void CThirdPersonCamera::AdjustCameraPosition(CameraTransform& transform, const D3DXVECTOR3& playerPos)
{
	// プレイヤーの頭位置を注視点とする
	D3DXVECTOR3 playerEye = playerPos;
	playerEye.y += PLAYER_EYE; // プレイヤーの頭の高さ
	transform.posR = playerEye;

	// カメラの理想位置
	D3DXVECTOR3 offsetFromPlayer;
	offsetFromPlayer.x = sinf(transform.rotation.y) * cosf(transform.rotation.x) * m_fDistance;
	offsetFromPlayer.y = sinf(transform.rotation.x) * m_fDistance + OFFSET_PLAYER_DISTANCE;
	offsetFromPlayer.z = cosf(transform.rotation.y) * cosf(transform.rotation.x) * m_fDistance;

	D3DXVECTOR3 idealCamPos = playerEye + offsetFromPlayer;

	// Bullet Physics でレイキャスト
	btVector3 from(playerEye.x, playerEye.y, playerEye.z);
	btVector3 to(idealCamPos.x, idealCamPos.y, idealCamPos.z);

	btCollisionWorld::ClosestRayResultCallback rayCallback(from, to);

	// カメラRay自身のCollisionGroup
	rayCallback.m_collisionFilterGroup = COLLISION_GROUP_CAMERA;

	// カメラRayはStageだけを見る
	rayCallback.m_collisionFilterMask = COLLISION_GROUP_STAGE;

	// 物理世界の取得
	CManager::Instance()->GetPhysicsWorld()->rayTest(from, to, rayCallback);

	// 衝突判定
	if (rayCallback.hasHit())
	{
		// 衝突点
		btVector3 hitPoint =
			rayCallback.m_hitPointWorld;

		// プレイヤーからカメラ方向
		btVector3 rayDir =
			(to - from).normalized();

		// 壁にめり込まないよう少し手前に配置
		hitPoint -= rayDir * HIT_POINT;

		transform.posV =
			D3DXVECTOR3(
				hitPoint.x(),
				hitPoint.y(),
				hitPoint.z());
	}
	else
	{// 衝突なし
		// 理想位置そのまま
		transform.posV = idealCamPos;
	}

	// 注視点はプレイヤーの頭
	transform.posR = playerEye;
}
