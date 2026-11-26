//=============================================================================
//
// 三人称カメラ処理 [ThirdPersonCamera.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _THIRD_PERSON_CAMERA_H_// このマクロ定義がされていなかったら
#define _THIRD_PERSON_CAMERA_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "CameraController.h"
#include "Camera.h"

//*****************************************************************************
// 三人称カメラクラス
//*****************************************************************************
class CThirdPersonCamera : public ICameraController
{
public:
    CThirdPersonCamera();
    ~CThirdPersonCamera() override = default;

    void Update(CameraTransform& transform) override;

private:

    // 向き更新
    void UpdateCameraRotation(CameraTransform& transform);

    void CameraWithGamepad(CameraTransform& transform, float stickX, float stickY);

    // カメラの壁貫通防止
    void AdjustCameraPosition(CameraTransform& transform, const D3DXVECTOR3& playerPos);

private:
    static constexpr float OFFSET_PLAYER_DISTANCE   = 20.0f;
    static constexpr float PLAYER_EYE               = 60.0f;
    static constexpr float HIT_POINT                = 10.0f;

    float       m_fDistance;

    // 着地カメラへの移行時間
    float m_LandingBlend;

    // 着地カメラへの移行開始位置
    D3DXVECTOR3 m_LandingStartPos;

    // 着地カメラへの移行開始注視点
    D3DXVECTOR3 m_LandingStartTarget;

    // 移行開始フラグ
    bool m_IsLandingBlend;
};

#endif