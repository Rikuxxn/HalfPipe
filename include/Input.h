//=============================================================================
//
// 入力処理 [Input.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _INPUT_H_// このマクロ定義がされていなかったら
#define _INPUT_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************


//*****************************************************************************
// 入力クラス
//*****************************************************************************
class CInput
{
public:
	CInput();
	virtual ~CInput();

	static constexpr int NUM_KEY_MAX = 256;	// キーの最大数

	virtual HRESULT Init(HINSTANCE hInstance);
	virtual void Uninit(void);
	virtual void Update(void) = 0;
	LPDIRECTINPUTDEVICE8 GetDevice(void) { return m_pDevice; }

protected:
	static LPDIRECTINPUT8	m_pInput;		// DirectInputオブジェクトへのポインタ
	LPDIRECTINPUTDEVICE8	m_pDevice;		// 入力デバイスへのポインタ

private:
};

//*****************************************************************************
// キーボードクラス
//*****************************************************************************
class CInputKeyboard : public CInput
{
public:
	CInputKeyboard();
	~CInputKeyboard();

	HRESULT Init(HINSTANCE hInstance, HWND hWnd);
	void Uninit(void);
	void Update(void);
	bool GetPress(int nKey);
	bool GetTrigger(int nKey);
	bool GetAnyKeyTrigger(void);
	bool GetRelease(int nKey);
	bool GetRepeat(int nKey);
private:
	BYTE m_aKeyState[NUM_KEY_MAX];				// キーボードのプレス情報
	BYTE m_aOldState[NUM_KEY_MAX];				// キーボードの前回のプレス情報
};

//*****************************************************************************
// ジョイパッドクラス
//*****************************************************************************
class CInputJoypad
{
public:
	CInputJoypad();
	~CInputJoypad();

	static constexpr float DEADZONE		= 14000.0f;	// デッドゾーン
	static constexpr float LSTICK_VALUE = 32767.0f;	// 左スティックの値

	// スティック入力方向の定義(コマンド入力用)
	enum class StickDir
	{
		RIGHT,	// 右
		UR,		// 右上
		UP,		// 上
		UL,		// 左上
		LEFT,	// 左
		DL,		// 左下
		DOWN,	// 下
		DR,		// 右下
		NONE	// 無し
	};

	//キーの種類
	typedef enum
	{
		JOYKEY_UP = 0,	// 十字キー上
		JOYKEY_DOWN,	// 十字キー下
		JOYKEY_LEFT,	// 十字キー左
		JOYKEY_RIGHT,	// 十字キー右
		JOYKEY_START,	// スタートボタン
		JOYKEY_BACK,	// バックボタン
		JOYKEY_LS,		// 左スティック押し込み
		JOYKEY_RS,		// 右スティック押し込み
		JOYKEY_LB,		// 左ボタン
		JOYKEY_RB,		// 右ボタン
		JOYKEY_LT,		// 左トリガー
		JOYKEY_RT,		// 右トリガー
		JOYKEY_A,		// Aボタン
		JOYKEY_B,		// Bボタン
		JOYKEY_X,		// Xボタン
		JOYKEY_Y,		// Yボタン
		JOYKEY_MAX
	}JOYKEY;

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	bool GetTrigger(JOYKEY Key);
	bool GetRelease(JOYKEY Key);
	bool GetPress(JOYKEY Key);
	bool GetTriggerL2(void);
	bool GetTriggerR2(void);
	bool GetPressL2(void);
	bool GetPressR2(void);
	bool GetAnyPress(void);
	bool GetAnyTrigger(void);
	bool GetStick(void);
	bool CheckCommand(const std::vector<StickDir>& command, int maxFrame = DEFAULT_FRAME);
	StickDir GetDirection(float x, float y);
	XINPUT_STATE* GetStickAngle(void);
	XINPUT_STATE GetState(void) { return m_joyKeyState; }
	void SetVibration(WORD left, WORD right);
	void SetVibration(WORD left, WORD right, int nTimer);
	void StopVibration(void);

private:
	static constexpr int	DIR_COUNT		= 8;			// 入力方向数
	static constexpr int	HISTORY_LIMIT	= 30;			// 古い履歴削除フレーム数
	static constexpr float	DEFAULT_FRAME	= 90;			// デフォルトの入力判定フレーム(このフレーム以内にコマンド入力)

	std::vector<std::pair<StickDir, int>> m_inputHistory;	// pair<方向, 入力フレーム番号>
	StickDir	 m_prevDir;									// 直前の入力方向
	XINPUT_STATE m_joyKeyState;								// ジョイパッドのプレス情報
	XINPUT_STATE m_joyKeyStateTrigger;						// ジョイパッドのトリガー情報
	XINPUT_STATE m_joyKeyStateRelease;						// ジョイパッドのリリース情報
	XINPUT_STATE m_aOldJoyKeyState;							// ジョイパッドの前回の情報
	int			 m_VibrationTimer;							// 振動時間
	int			 m_frameCounter;							// コマンド入力のフレーム
	bool		 m_joyKeyFlag[JOYKEY_MAX];					// ジョイパッドのフラグ
	bool		 m_bVibration;								// 振動フラグ
};

//*****************************************************************************
// マウスクラス
//*****************************************************************************
class CInputMouse : public CInput
{
public:
	CInputMouse();
	~CInputMouse();

	HRESULT Init(HINSTANCE hInstance, HWND hWnd);
	void Uninit(void);
	void Update(void);
	bool GetPress(int button);
	bool GetTrigger(int button);
	bool GetRelease(int button);
	bool GetMouseState(DIMOUSESTATE* mouseState);
	static int GetWheel(void);
	static void SetCursorVisibility(bool visible);
	D3DXVECTOR3 GetGroundHitPosition(void); // 地面Y=0との交差点を取得

private:
	static DIMOUSESTATE m_mouseState;			// マウスの状態
	DIMOUSESTATE m_prevMouseState{};
};

#endif