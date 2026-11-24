//=============================================================================
//
// レンダリング処理 [Renderer.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _RENDERER_H_// このマクロ定義がされていなかったら
#define _RENDERER_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
//#include "imguimaneger.h"
#include "RenderTarget.h"

//*****************************************************************************
// 前方宣言
//*****************************************************************************
class CDebugProc;
class CDebugProc3D;
class CLightComponent;
class CShaderResource;
class CShadowMap;


//*****************************************************************************
// レンダラークラス
//*****************************************************************************
class CRenderer
{
public:
	CRenderer();
	~CRenderer();

	HRESULT Init(HWND hWnd, BOOL bWindow);
	void Uninit(void);

	void Begin(void);
	void End(void);
	void ResetDevice(void);
	void OnResize(UINT width, UINT height);
	void SetViewport(UINT width, UINT height);

	// レンダーターゲット生成処理(ブルーム)
	HRESULT CreateRenderTargets(void);

	void DrawFullscreenQuad(ID3DXEffect* effect, float width, float height);

	void RenderBloom(void);

	void ReleaseRenderTargets(void);

	//*****************************************************************************
	// flagment関数
	//*****************************************************************************
	bool NeedsReset(void) const;

	//*****************************************************************************
	// setter関数
	//*****************************************************************************
	//void SetBgCol(D3DXCOLOR col) { m_bgCol = col; }
	void SetBloomPower(float power) { m_bloomPower = power; }

	//*****************************************************************************
	// getter関数
	//*****************************************************************************
	//static CDebugProc* GetDebug(void) { return m_pDebug; }
	//static CDebugProc3D* GetDebug3D(void) { return m_pDebug3D; }
	LPDIRECT3DDEVICE9 GetDevice(void) { return m_pD3DDevice; };
	D3DXCOLOR GetBgCol(void) { return m_bgCol; }
	UINT GetBackBufferWidth(void) { return m_d3dpp.BackBufferWidth; }
	UINT GetBackBufferHeight(void) { return m_d3dpp.BackBufferHeight; }
	float GetBloomPower(void) { return m_bloomPower; }

	CShadowMap* GetShadowMap(void);

private:
	LPDIRECT3D9						m_pD3D;				// DirectX3Dオブジェクトへのポインタ
	LPDIRECT3DDEVICE9				m_pD3DDevice;		// デバイスへのポインタ
	D3DXCOLOR						m_bgCol;			// 画面背景の色
	UINT							m_ResizeWidth;		// 再設定用の画面の幅
	UINT							m_ResizeHeight;		// 再設定用の画面の高さ
	D3DPRESENT_PARAMETERS			m_d3dpp;			// 再設定用のパラメーター


	//*****************************************************************************
	// ブルーム用変数
	//*****************************************************************************

	CRenderTarget m_SceneRT;
	CRenderTarget m_BrightRT;
	CRenderTarget m_BlurRT1;
	CRenderTarget m_BlurRT2;

	// BackBuffer
	LPDIRECT3DSURFACE9 m_pBackBuffer = nullptr;

	// ブルームシェーダー変数
	std::shared_ptr<CShaderResource> m_pBrightShader;
	std::shared_ptr<CShaderResource> m_pHorizontalShader;
	std::shared_ptr<CShaderResource> m_pVerticalShader;
	std::shared_ptr<CShaderResource> m_pCombineShader;

	// ブルーム強さ(輝度)
	float m_bloomPower;

	// シャドウマップ
	std::unique_ptr<CShadowMap> m_ShadowMap;
};

#endif