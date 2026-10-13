//=============================================================================
//
// ムービーカメラ処理 [MovieCamera.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _MOVIE_CAMERA_H_// このマクロ定義がされていなかったら
#define _MOVIE_CAMERA_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "CameraController.h"
#include "Camera.h"

//*****************************************************************************
// ムービーカメラクラス
//*****************************************************************************
class CMovieCamera : public ICameraController
{
public:
    CMovieCamera();
    ~CMovieCamera() = default;

    struct KeyFrame
    {
        D3DXVECTOR3 posV;       // 視点
        D3DXVECTOR3 posR;       // 注視点
        D3DXVECTOR3 rotation;   // 向き
        float       distance;   // 距離
        float       duration;   // 次のキーフレームまでの時間
        float       delay;      // このキーフレーム開始前の待ち時間
   
        // 次のキーフレームへ移動するときのイージング
        std::function<float(float)> easing;
    };

    void SetKeyFrames(const std::vector<KeyFrame>& keys);

    void Start(void);
    void Stop(void);

    void Update(CameraTransform& transform) override;

    // 保存・読み込み
    void Save(const char* filename);
    void Load(const char* filename);

private:

    std::string GetEasingName(const std::function<float(float)>& easing);

    std::function<float(float)> GetEasingFunction(const std::string& name);

private:
    std::vector<KeyFrame> m_Keys;

    int     m_CurrentKey;
    float   m_CurrentTime;
    bool    m_IsPlaying;
};

#endif