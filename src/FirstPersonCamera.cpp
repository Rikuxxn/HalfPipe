//=============================================================================
//
// 一人称カメラ処理 [FirstPersonCamera.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "FirstPersonCamera.h"
#include "MathConstants.h"
#include "MathUtility.h"
#include "Manager.h"
#include "Input.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Player.h"
#include "TransformComponent.h"


// 名前空間の使用
using namespace MathConst;
using namespace MathUtil;


//=============================================================================
// コンストラクタ
//=============================================================================
CFirstPersonCamera::CFirstPersonCamera() :
	m_fDistance(280.0f)
{

}

//=============================================================================
// 更新処理
//=============================================================================
void CFirstPersonCamera::Update(CameraTransform& transform)
{

}

//=============================================================================
// ゲームパッドのカメラ回転処理
//=============================================================================
void CFirstPersonCamera::CameraWithGamepad(CameraTransform& transform, float stickX, float stickY)
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
