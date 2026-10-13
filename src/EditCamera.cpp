//=============================================================================
//
// エディターカメラ処理 [EditCamera.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "EditCamera.h"
#include "MathConstants.h"
#include "MathUtility.h"
#include "Manager.h"
#include "Input.h"
#include "InputManager.h"


// 名前空間の使用
using namespace MathConst;
using namespace MathUtil;


//=============================================================================
// コンストラクタ
//=============================================================================
CEditCamera::CEditCamera() :
	m_fDistance (280.0f)
{

}

//=============================================================================
// エディターカメラ更新
//=============================================================================
void CEditCamera::Update(CameraTransform& transform)
{
    // 入力の取得
    CInputKeyboard* pInputKeyboard = CManager::Instance()->GetInput()->GetKeyboard();
    CInputMouse* pInputMouse = CManager::Instance()->GetInput()->GetMouse();

    if (!pInputKeyboard || !pInputMouse)
    {
        return;
    }

    // マウスカーソルを表示
    pInputMouse->SetCursorVisibility(true);

    //--------------------------------------------------------------------------
    // マウス移動量
    //--------------------------------------------------------------------------
    POINT cursorPos;
    GetCursorPos(&cursorPos);

	// 前フレームからのマウス移動量を取得
	static POINT prevCursorPos = { cursorPos.x, cursorPos.y };
	float deltaX = (float)(cursorPos.x - prevCursorPos.x);
	float deltaY = (float)(cursorPos.y - prevCursorPos.y);

	// 現在のカーソル位置を保存（次のフレームでの比較用）
	prevCursorPos = cursorPos;

	deltaX *= MOUSE_SENSITIVITY;
	deltaY *= MOUSE_SENSITIVITY;

	//====================================
	// マウスホイールでズームイン・アウト
	//====================================
	int wheel = pInputMouse->GetWheel();

	if (wheel != 0)
	{
		m_fDistance -= wheel * ZOOM_SPEED;

		// カメラ距離制限
		if (m_fDistance < MIN_DISTANCE)
		{
			m_fDistance = MIN_DISTANCE;
		}
		if (m_fDistance > MAX_DISTANCE)
		{
			m_fDistance = MAX_DISTANCE;
		}
	}

	if (pInputKeyboard->GetPress(DIK_LALT) && pInputMouse->GetPress(0)) // 左クリック押しながらマウス移動 → 視点回転
	{
		transform.rotation.y += deltaX; // 水平回転
		transform.rotation.x += deltaY; // 垂直回転

		//角度の正規化
		NormalizeRotation(transform);

		// 垂直回転の制限
		if (transform.rotation.x > PI_HALF)
		{
			transform.rotation.x = PI_HALF;
		}

		if (transform.rotation.x < -PI_HALF)
		{
			transform.rotation.x = -PI_HALF;
		}

		// 視点の更新（カメラの方向を適用）
		transform.posV.x = transform.posR.x + sinf(transform.rotation.y) * cosf(transform.rotation.x) * m_fDistance;
		transform.posV.y = transform.posR.y + sinf(transform.rotation.x) * m_fDistance;
		transform.posV.z = transform.posR.z + cosf(transform.rotation.y) * cosf(transform.rotation.x) * m_fDistance;

	}
	else if (pInputMouse->GetPress(1)) // 右クリック押しながらマウス移動 → 注視点回転
	{
		transform.rotation.y += deltaX; // 水平回転
		transform.rotation.x += deltaY; // 垂直回転

		//角度の正規化
		NormalizeRotation(transform);

		// 垂直回転の制限
		if (transform.rotation.x > PI_HALF)
		{
			transform.rotation.x = PI_HALF;
		}
		if (transform.rotation.x < -PI_HALF)
		{
			transform.rotation.x = -PI_HALF;
		}

		// 注視点の更新
		transform.posR.x = transform.posV.x - sinf(transform.rotation.y) * cosf(transform.rotation.x) * m_fDistance;
		transform.posR.y = transform.posV.y - sinf(transform.rotation.x) * m_fDistance;
		transform.posR.z = transform.posV.z - cosf(transform.rotation.y) * cosf(transform.rotation.x) * m_fDistance;
	}
	else
	{
		// 入力がない場合でもズーム反映のために視点を更新
		transform.posV.x = transform.posR.x + sinf(transform.rotation.y) * cosf(transform.rotation.x) * m_fDistance;
		transform.posV.y = transform.posR.y + sinf(transform.rotation.x) * m_fDistance;
		transform.posV.z = transform.posR.z + cosf(transform.rotation.y) * cosf(transform.rotation.x) * m_fDistance;
	}

	// 注視点の更新
	transform.posR.x = transform.posV.x - sinf(transform.rotation.y) * cosf(transform.rotation.x) * m_fDistance;
	transform.posR.y = transform.posV.y - sinf(transform.rotation.x) * m_fDistance;
	transform.posR.z = transform.posV.z - cosf(transform.rotation.y) * cosf(transform.rotation.x) * m_fDistance;
}

//=============================================================================
// 回転値の正規化
//=============================================================================
void CEditCamera::NormalizeRotation(CameraTransform& transform)
{
    // Y軸を -PI ～ PI に収める
    while (transform.rotation.y > D3DX_PI)
    {
		transform.rotation.y -= D3DX_PI * F_DOUBLE;
    }

    while (transform.rotation.y < -D3DX_PI)
    {
		transform.rotation.y += D3DX_PI * F_DOUBLE;
    }

    // X軸を制限
    if (transform.rotation.x > D3DX_PI * HALF)
    {
		transform.rotation.x = D3DX_PI * HALF;
    }

    if (transform.rotation.x < -D3DX_PI * HALF)
    {
		transform.rotation.x = -D3DX_PI * HALF;
    }
}