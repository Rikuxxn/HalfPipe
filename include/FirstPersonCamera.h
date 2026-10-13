//=============================================================================
//
// 一人称カメラ処理 [FirstPersonCamera.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _FIRST_PERSON_CAMERA_H_// このマクロ定義がされていなかったら
#define _FIRST_PERSON_CAMERA_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "CameraController.h"
#include "Camera.h"

//*****************************************************************************
// 一人称カメラクラス
//*****************************************************************************
class CFirstPersonCamera : public ICameraController
{
public:
    CFirstPersonCamera();
    ~CFirstPersonCamera() override = default;

    void Update(CameraTransform& transform) override;

private:

    void CameraWithGamepad(CameraTransform& transform, float stickX, float stickY);

private:
    float       m_fDistance;

};

#endif