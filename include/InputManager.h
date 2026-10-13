//=============================================================================
//
// 入力マネージャー処理 [InputManager.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _INPUT_MANAGER_H_// このマクロ定義がされていなかったら
#define _INPUT_MANAGER_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************

// 前方宣言
class CInputKeyboard;
class CInputMouse;
class CInputJoypad;

//*****************************************************************************
// 入力マネージャークラス
//*****************************************************************************
class CInputManager
{
private:

    std::unique_ptr<CInputKeyboard> m_Keyboard;
    std::unique_ptr<CInputMouse>    m_Mouse;
    std::unique_ptr<CInputJoypad>   m_Joypad;

public:
    CInputManager() {}
    ~CInputManager() {}

    HRESULT Init(HINSTANCE hInstance, HWND hWnd);
    void Update(void);

public:

    CInputKeyboard* GetKeyboard (void) { return m_Keyboard.get(); }
    CInputMouse*    GetMouse    (void) { return m_Mouse.get(); }
    CInputJoypad*   GetJoypad   (void) { return m_Joypad.get(); }
};

#endif