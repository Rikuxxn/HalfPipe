//=============================================================================
//
// カメラ処理 [Camera.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _CAMERA_H_// このマクロ定義がされていなかったら
#define _CAMERA_H_// 2重インクルード防止のマクロ定義

// 前方宣言
class ICameraController;

//*****************************************************************************
// カメラ状態
//*****************************************************************************
struct CameraTransform
{
	D3DXVECTOR3 posV        = INIT_VEC3;
    D3DXVECTOR3 posR        = INIT_VEC3;
	D3DXVECTOR3 target      = INIT_VEC3;
	D3DXVECTOR3 rotation    = INIT_VEC3;

	float distance = 0.0f;
};

//*****************************************************************************
// カメラクラス
//*****************************************************************************
class CCamera
{
public:
	CCamera();
	~CCamera();

	// カメラの種類
	typedef enum
	{
		MODE_EDIT = 0,		// エディターカメラ
        MODE_FIXED,			// 固定カメラ
		MODE_FIRST_PERSON,	// 一人称
		MODE_THIRD_PERSON,	// 三人称
		MODE_DIRECTION,		// 演出カメラ
		MODE_MOVIE,			// ムービーカメラ
		MODE_MAX
	}MODE;

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void SetCamera(void);
    void AdjustCameraPosition(const D3DXVECTOR3& playerPos);

    //-------------------------------------------------------------------------
	// コントローラー
    //-------------------------------------------------------------------------

    ICameraController* GetController(void) const { return m_pController.get(); }

    //-------------------------------------------------------------------------
    // カメラ情報
    //-------------------------------------------------------------------------

    D3DXVECTOR3 GetPosV     (void) const { return m_FinalTransform.posV; }
    D3DXVECTOR3 GetPosR     (void) const { return m_FinalTransform.posR; }
    D3DXVECTOR3 GetRot      (void) const { return m_FinalTransform.rotation; }
    D3DXVECTOR3 GetTarget   (void) const { return m_FinalTransform.target; }
    D3DXVECTOR3 GetUp       (void) const { return D3DXVECTOR3(0.0f, 1.0f, 0.0f); }
    D3DXVECTOR3 GetForward  (void) const;

    //-------------------------------------------------------------------------
    // マトリックス
    //-------------------------------------------------------------------------

    D3DXMATRIX GetViewMatrix(void) const { return m_mtxView; }
    D3DXMATRIX GetProjMatrix(void) const { return m_mtxProjection; }

    //-------------------------------------------------------------------------
    // トランスフォーム
    //-------------------------------------------------------------------------

    void SetPosV    (const D3DXVECTOR3& position)   { m_BaseTransform.posV = position; }
    void SetPosR    (const D3DXVECTOR3& position)   { m_BaseTransform.posR = position; }
    void SetTarget  (const D3DXVECTOR3& target)     { m_BaseTransform.target = target; }
    void SetRot     (const D3DXVECTOR3& rotation)   { m_BaseTransform.rotation = rotation; }
    void SetDistance(float distance)                { m_BaseTransform.distance = distance; }

    //-------------------------------------------------------------------------
    // カメラモード
    //-------------------------------------------------------------------------

    void SetCamMode(MODE mode);
    MODE GetMode    (void) const    { return m_Mode; }

    //-------------------------------------------------------------------------
    // 振動
    //-------------------------------------------------------------------------

    void CameraShake        (void);
    void TriggerEventShake  (float power, float duration);
    void SetCameraShake     (bool flag) { m_isCameraShakeOn = flag;}

#ifdef _DEBUG
    // Imguiのデバッグ描画
    void DrawDebugGUI(void);
#endif

private:

    void SetController(std::unique_ptr<ICameraController> controller);

    void UpdateViewMatrix(void);
    void UpdateProjectionMatrix(void);

    void ApplyShake(void);

    void NormalizeRotX(void);
    void NormalizeRotY(void);

private:

    //-------------------------------------------------------------------------
    // コントローラー
    //-------------------------------------------------------------------------

    std::unique_ptr<ICameraController> m_pController;

    //-------------------------------------------------------------------------
    // カメラトランスフォーム
    //-------------------------------------------------------------------------

    // Controllerが作る基本状態
    CameraTransform m_BaseTransform;

    // Shakeなどを適用した最終状態
    CameraTransform m_FinalTransform;

    //-------------------------------------------------------------------------
    // 行列
    //-------------------------------------------------------------------------

    D3DXMATRIX m_mtxView;
    D3DXMATRIX m_mtxProjection;

    //-------------------------------------------------------------------------
    // カメラモード
    //-------------------------------------------------------------------------

    MODE m_Mode;

    //-------------------------------------------------------------------------
    // 振動
    //-------------------------------------------------------------------------

    D3DXVECTOR3 m_shakeOffset;
    float       m_eventShakeTime;
    float       m_eventShakePower;
    float       m_eventShakeDuration;
    bool        m_isCameraShakeOn;

private:

    //-------------------------------------------------------------------------
    // カメラパラメータ
    //-------------------------------------------------------------------------

    static constexpr float FOV              = 80.0f;    // 視野角
    static constexpr float RENDER_DISTANCE  = 9000;     // 描画距離
    static constexpr float GAMECAM_DISTANCE = 280.0f;   // ゲームカメラの距離
    static constexpr float CAM_MIN_DISTANCE = 100.0f;   // 最低距離
    static constexpr float CAM_MAX_DISTANCE = 800.0f;   // 最高距離
    static constexpr float SHAKE_DISTANCE   = 1300.0f;  // 振動距離
};

#endif