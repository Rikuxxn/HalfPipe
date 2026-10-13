//=============================================================================
//
// フェード処理 [Fade.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _FADE_H_// このマクロ定義がされていなかったら
#define _FADE_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Scene.h"

//*****************************************************************************
// フェードクラス
//*****************************************************************************
class CFade
{
public:
	// 状態
	typedef enum
	{
		FADE_NONE = 0,    // 何もしていない状態
		FADE_IN,          // フェードイン状態
		FADE_OUT,         // フェードアウト状態
		FADE_MAX
	}FadeState;

	CFade();
	~CFade();

	HRESULT Init(void);
	void	Update(void);
	void	Draw(void);

	void StartFadeOut(CScene::Mode next);

	FadeState GetFade(void) { return m_State; }

private:
	static constexpr float	FHD_WIDTH	= 1920.0f;	// 画面の幅
	static constexpr float	FHD_HEIGHT	= 1080.0f;	// 画面の高さ
	static constexpr float	FADE_SPEED	= 0.03f;	// フェードスピード

	ComPtr<IDirect3DVertexBuffer9>	m_pVtxBuff;				// 頂点バッファ
	FadeState						m_State;				// 状態
	CScene::Mode					m_NextMode;				// 次のモード
	float							m_Alpha;
};

#endif