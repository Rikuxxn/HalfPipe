//=============================================================================
//
// エディターカメラ処理 [EditCamera.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _EDIT_CAMERA_H_// このマクロ定義がされていなかったら
#define _EDIT_CAMERA_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "CameraController.h"
#include "Camera.h"

//*****************************************************************************
// エディターカメラクラス
//*****************************************************************************
class CEditCamera : public ICameraController
{
public:
    CEditCamera();
    ~CEditCamera() override = default;

    void Update(CameraTransform& transform) override;

private:

    void NormalizeRotation(CameraTransform& transform);

private:

    static constexpr float MOUSE_SENSITIVITY    = 0.004f;       // マウス感度
    static constexpr float ZOOM_SPEED           = 15.0f;        // ズームスピード
    static constexpr float MIN_DISTANCE         = 100.0f;       // 最低距離
    static constexpr float MAX_DISTANCE         = 800.0f;       // 最高距離

private:
    float       m_fDistance;
};

#endif