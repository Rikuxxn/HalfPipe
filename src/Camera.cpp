//=============================================================================
//
// カメラ処理 [Camera.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Camera.h"
#include "Renderer.h"
#include "Manager.h"
#include "Game.h"
#include "MathConstants.h"
#include "Input.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "CameraController.h"
#include "TimeSystem.h"
#include "imguimaneger.h"
#include "EditCamera.h"
#include "FirstPersonCamera.h"
#include "ThirdPersonCamera.h"
#include "DirectionCamera.h"
#include "MovieCamera.h"
#include "FixedCamera.h"


namespace CameraParam
{
	const D3DXVECTOR3 DEFAULT_POS_V{ 0.0f, 80.0f, -540.0f };
	const D3DXVECTOR3 DEFAULT_POS_R{ 0.0f, 80.0f, 0.0f };
	const D3DXVECTOR3 DEFAULT_VEC_U{ 0.0f, 1.0f, 0.0f };
	const D3DXVECTOR3 DEFAULT_ROT{ 0.0f, D3DX_PI, 0.0f };
	const D3DXVECTOR3 GAME_POS_R{ 0.0f, -55.8f, -30.0f };

	constexpr float GAME_ROT_X				= 0.89f;
	constexpr float GAME_ROT_Y				= D3DX_PI;
	constexpr float DIRECTION_DISTANCE		= 520.0f;
	constexpr float PLAYER_EYE				= 60.0f;
	constexpr float HIT_POINT				= 10.0f;
	constexpr float OFFSET_PLAYER_DISTANCE	= 20.0f;
}


//=============================================================================
// コンストラクタ
//=============================================================================
CCamera::CCamera()
{
	// 値のクリア
	m_mtxProjection			= {};		// プロジェクションマトリックス
	m_mtxView				= {};		// ビューマトリックス
}
//=============================================================================
// デストラクタ
//=============================================================================
CCamera::~CCamera()
{
	// なし
}
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CCamera::Init(void)
{	
	// 名前空間CameraParamの使用
	using namespace CameraParam;

	D3DXMatrixIdentity(&m_mtxView);
	D3DXMatrixIdentity(&m_mtxProjection);

	m_BaseTransform.posV		= DEFAULT_POS_V;
	m_BaseTransform.posR		= DEFAULT_POS_R;
	m_BaseTransform.target		= D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	m_BaseTransform.rotation	= DEFAULT_ROT;
	m_BaseTransform.distance	= GAMECAM_DISTANCE;
	m_FinalTransform			= m_BaseTransform;

	m_Mode = MODE_EDIT;

	m_shakeOffset = INIT_VEC3;

	m_eventShakeTime		= 0.0f;
	m_eventShakePower		= 0.0f;
	m_eventShakeDuration	= 0.0f;

	m_isCameraShakeOn = false;

	UpdateProjectionMatrix();

	// Controllerで基本カメラを初期化
	if (m_pController)
	{
		m_pController->Init();
	}

	return S_OK;
}
//=============================================================================
// 終了処理
//=============================================================================
void CCamera::Uninit(void)
{

}
//=============================================================================
// 更新処理
//=============================================================================
void CCamera::Update(void)
{
	// Controllerで基本カメラを更新
	if (m_pController)
	{
		m_pController->Update(m_BaseTransform);
	}

	// 基本状態をコピー
	m_FinalTransform = m_BaseTransform;

	// カメラシェイク
	ApplyShake();

	// ビューマトリックス更新
	UpdateViewMatrix();

	// プロジェクションマトリックス更新
	UpdateProjectionMatrix();
}
//=============================================================================
// カメラの設定処理
//=============================================================================
void CCamera::SetCamera(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::Instance()->GetRenderer()->GetDevice();

	if (!pDevice)
	{
		return;
	}

	// ビューマトリックスの設定
	pDevice->SetTransform(D3DTS_VIEW, &m_mtxView);

	// プロジェクションマトリックスの設定
	pDevice->SetTransform(D3DTS_PROJECTION, &m_mtxProjection);
}

//=============================================================================
// ビューマトリックス更新
//=============================================================================
void CCamera::UpdateViewMatrix(void)
{
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);

	D3DXMatrixLookAtLH(
		&m_mtxView,
		&m_FinalTransform.posV,
		&m_FinalTransform.posR,
		&up);
}

//=============================================================================
// プロジェクションマトリックス更新
//=============================================================================
void CCamera::UpdateProjectionMatrix(void)
{
	// レンダラーの取得
	auto device = CManager::Instance()->GetRenderer()->GetDevice();

	if (!device)
	{
		return;
	}

	D3DVIEWPORT9 vp;
	device->GetViewport(&vp);

	float aspect = (float)vp.Width / (float)vp.Height;

	D3DXMatrixPerspectiveFovLH(
		&m_mtxProjection,
		D3DXToRadian(FOV),
		aspect,
		1.0f,
		RENDER_DISTANCE);
}

//=============================================================================
// コントローラーの設定処理(各種カメラへの設定)
//=============================================================================
void CCamera::SetController(std::unique_ptr<ICameraController> controller)
{
	if (m_pController)
	{
		m_pController->Uninit();
	}

	m_pController = std::move(controller);

	if (m_pController)
	{
		m_pController->Init();
	}
}

//=============================================================================
// カメラモードの設定
//=============================================================================
void CCamera::SetCamMode(MODE mode)
{
	switch (mode)
	{
	case MODE_EDIT:
		SetController(std::make_unique<CEditCamera>());
		break;

	case MODE_FIXED:
		SetController(std::make_unique<CFixedCamera>());
		break;

	case MODE_FIRST_PERSON:
		SetController(std::make_unique<CFirstPersonCamera>());
		break;

	case MODE_THIRD_PERSON:
		SetController(std::make_unique<CThirdPersonCamera>());
		break;

	case MODE_DIRECTION:
		SetController(std::make_unique<CDirectionCamera>());
		break;

	case MODE_MOVIE:
		SetController(std::make_unique<CMovieCamera>());
		break;

	default:
		return;
	}

	m_Mode = mode;
}

//=============================================================================
// カメラの前方ベクトル取得
//=============================================================================
D3DXVECTOR3 CCamera::GetForward(void) const
{
	// カメラの回転角度（Y軸）から前方ベクトルを計算
	float yaw = m_BaseTransform.rotation.y;

	D3DXVECTOR3 forward(-sinf(yaw), 0.0f, -cosf(yaw));

	// 正規化する
	D3DXVec3Normalize(&forward, &forward);

	return forward;
}

//=============================================================================
// 角度の正規化 X軸
//=============================================================================
void CCamera::NormalizeRotX(void)
{
	if (m_BaseTransform.rotation.x > D3DX_PI * MathConst::HALF)
	{
		m_BaseTransform.rotation.x = D3DX_PI * MathConst::HALF;
	}
	if (m_BaseTransform.rotation.x < -D3DX_PI * MathConst::HALF)
	{
		m_BaseTransform.rotation.x = -D3DX_PI * MathConst::HALF;
	}
}
//=============================================================================
// 角度の正規化 Y軸
//=============================================================================
void CCamera::NormalizeRotY(void)
{
	if (m_BaseTransform.rotation.y > D3DX_PI)
	{
		m_BaseTransform.rotation.y -= D3DX_PI * MathConst::F_DOUBLE;
	}
	else if (m_BaseTransform.rotation.y < -D3DX_PI)
	{
		m_BaseTransform.rotation.y += D3DX_PI * MathConst::F_DOUBLE;
	}
}

//=============================================================================
// カメラシェイクの適用処理
//=============================================================================
void CCamera::ApplyShake(void)
{
	if (!m_isCameraShakeOn)
	{
		return;
	}

	// カメラシェイク
	CameraShake();

	m_FinalTransform.posV	+= m_shakeOffset;
	m_FinalTransform.posR	+= m_shakeOffset;
	m_FinalTransform.target	+= m_shakeOffset;
}

//=============================================================================
// カメラシェイク処理
//=============================================================================
void CCamera::CameraShake(void)
{
	// 毎フレームリセット
	m_shakeOffset = INIT_VEC3;

	//=========================================================
	// イベントシェイク
	//=========================================================
	if (m_eventShakeTime > 0.0f)
	{
		// 残り時間による減衰
		float ratio = m_eventShakeTime / m_eventShakeDuration;

		float power = m_eventShakePower * ratio;

		// ランダムな揺れ
		m_shakeOffset.x = ((rand() % 2000 - 1000) / 1000.0f) * power;
		m_shakeOffset.y = ((rand() % 2000 - 1000) / 1000.0f) * power;
		m_shakeOffset.z = ((rand() % 2000 - 1000) / 1000.0f) * power;

		// 時間を進める
		float deltaTime = CManager::Instance()->GetTimeSystem()->GetDeltaTime();

		m_eventShakeTime -= deltaTime;

		if (m_eventShakeTime < 0.0f)
		{
			m_eventShakeTime = 0.0f;
		}
	}
}

//=============================================================================
// イベント時のカメラシェイクの設定
//=============================================================================
void CCamera::TriggerEventShake(float power, float duration)
{
	m_eventShakePower = power;
	m_eventShakeDuration = duration;
	m_eventShakeTime = duration;
}

#ifdef _DEBUG
//=============================================================================
// Imguiのデバッグ描画処理
//=============================================================================
void CCamera::DrawDebugGUI(void)
{
	// カメラ情報
	if (ImGui::TreeNode("Camera Info"))
	{
		ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 空白を空ける

		//*********************************************************************
		// POS の情報
		//*********************************************************************

		// 位置
		ImGui::Text("POS V"); ImGui::SameLine(80); // ラベル位置微調整

		ImGui::Text("X:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80); // 幅を狭く
		ImGui::DragFloat("##camera_posV_x", (float*)&m_BaseTransform.posV.x, 1.0f, -1000.0f, 1000.0f, "%.1f");

		ImGui::SameLine();
		ImGui::Text("Y:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##camera_posV_y", (float*)&m_BaseTransform.posV.y, 1.0f, -1000.0f, 1000.0f, "%.1f");

		ImGui::SameLine();
		ImGui::Text("Z:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##camera_posV_z", (float*)&m_BaseTransform.posV.z, 1.0f, -1000.0f, 1000.0f, "%.1f");

		ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 空白を空ける

		// 位置
		ImGui::Text("POS R"); ImGui::SameLine(80); // ラベル位置微調整

		ImGui::Text("X:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80); // 幅を狭く
		ImGui::DragFloat("##camera_posR_x", (float*)&m_BaseTransform.posR.x, 1.0f, -1000.0f, 1000.0f, "%.1f");

		ImGui::SameLine();
		ImGui::Text("Y:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##camera_posR_y", (float*)&m_BaseTransform.posR.y, 1.0f, -1000.0f, 1000.0f, "%.1f");

		ImGui::SameLine();
		ImGui::Text("Z:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##camera_posR_z", (float*)&m_BaseTransform.posR.z, 1.0f, -1000.0f, 1000.0f, "%.1f");

		//*********************************************************************
		// ROT の情報
		//*********************************************************************

		ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 空白を空ける

		// 向き
		ImGui::Text("ROT"); ImGui::SameLine(80); // ラベル位置微調整

		ImGui::Text("X:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		bool changedX = ImGui::DragFloat("##camera_rot_x", &m_BaseTransform.rotation.x, 0.1f, -D3DX_PI, D3DX_PI, "%.2f");

		ImGui::SameLine();
		ImGui::Text("Y:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		bool changedY = ImGui::DragFloat("##camera_rot_y", &m_BaseTransform.rotation.y, 0.1f, -D3DX_PI, D3DX_PI, "%.2f");

		ImGui::SameLine();
		ImGui::Text("Z:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		bool changedZ = ImGui::DragFloat("##camera_rot_z", &m_BaseTransform.rotation.z, 0.1f, -D3DX_PI, D3DX_PI, "%.2f");

		ImGui::TreePop(); // 閉じる
	}
}
#endif