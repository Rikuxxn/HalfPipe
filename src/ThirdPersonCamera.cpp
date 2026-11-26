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
#include "TimeSystem.h"


// 名前空間の使用
using namespace MathConst;
using namespace MathUtil;



//=============================================================================
// コンストラクタ
//=============================================================================
CThirdPersonCamera::CThirdPersonCamera() :
	m_fDistance			(50.0f),
	m_LandingBlend		(0.0f),
	m_LandingStartPos	(INIT_VEC3),
	m_LandingStartTarget(INIT_VEC3),
	m_IsLandingBlend	(false)
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

		D3DXVECTOR3 dir = playerAutoMove->GetMoveDirection();

		float fCurrentTime = playerAutoMove->GetSplineDelta();
		static float nTimeCount = 0.0f;
		static bool bJumpUp = false;
		nTimeCount += fCurrentTime;

		float dirX = playerPos.x - transform.posV.x;
		float dirZ = playerPos.z - transform.posV.z;

		if (!playerAutoMove->IsJump())
		{
			nTimeCount = 0.0f;
			bJumpUp = false;
			m_LandingBlend = 0.0f;
			m_IsLandingBlend = false;

			transform.posV.x += dirX * 0.05f;
			transform.posV.y += ((playerPos.y + 60.0f) - transform.posV.y) * 0.05f;
			transform.posV.z += dirZ * 0.05f;

			transform.posR = playerPos;
			transform.posR.y += 60.0f;
		}
		else if (nTimeCount <= 1.5f)
		{
			// ジャンプ中の注視位置
			auto aim = pPlayer->GetComponent<CAimComponent>();

			D3DXVECTOR3 targetCenter = aim->GetTargetCenter();

			transform.posV.x = playerPos.x + (playerPos.x - targetCenter.x) * 0.1f;
			transform.posV.y = playerPos.y + (playerPos.y - targetCenter.y) * 0.08f;
			transform.posV.z += dirZ * 0.1f;

			transform.posR = targetCenter;
			transform.posR.y += 20.0f;
		}
		else
		{
			//=====================================================
			// 着地カメラへの切り替え開始
			//=====================================================
			if (!m_IsLandingBlend)
			{
				m_IsLandingBlend = true;
				m_LandingBlend = 0.0f;

				// 現在のカメラ位置と注視点を保存
				m_LandingStartPos = transform.posV;
				m_LandingStartTarget = transform.posR;
			}

			//=====================================================
			// 補間時間
			//=====================================================
			float deltaTime = CManager::Instance()->GetTimeSystem()->GetDeltaTime();

			m_LandingBlend += deltaTime / 0.8f;

			if (m_LandingBlend > 1.0f)
			{
				m_LandingBlend = 1.0f;
			}

			// イージング
			float t = m_LandingBlend * m_LandingBlend * (3.0f - 2.0f * m_LandingBlend);

			//=====================================================
			// 着地カメラの目標位置
			//=====================================================
			D3DXVECTOR3 targetPos = playerPos;

			// プレイヤーより後方・上方に配置
			targetPos.x -= dir.x * 150.0f;
			targetPos.y += 120.0f;
			targetPos.z -= dir.z * 150.0f;

			// 注視点は着地地点
			D3DXVECTOR3 targetLook = playerPos;
			targetLook.y += 10.0f;

			//=====================================================
			// カメラ位置と注視点を補間
			//=====================================================
			transform.posV = m_LandingStartPos +
				(targetPos - m_LandingStartPos) * t;

			transform.posR = m_LandingStartTarget +
				(targetLook - m_LandingStartTarget) * t;
		}

		// 向きの更新
		UpdateCameraRotation(transform);

		// カメラの位置補正処理
		//AdjustCameraPosition(transform, playerPos);
	}
}

//=============================================================================
// 向き更新
//=============================================================================
void CThirdPersonCamera::UpdateCameraRotation(CameraTransform& transform)
{
	D3DXVECTOR3 direction = transform.posR - transform.posV;

	// 水平方向の向きを計算
	direction.y = 0.0f;

	if (D3DXVec3LengthSq(&direction) < 0.0001f)
	{
		return;
	}

	// 正規化
	D3DXVec3Normalize(&direction, &direction);

	transform.rotation.y = atan2f(-direction.x, -direction.z);
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
