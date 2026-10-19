//=============================================================================
//
// レンダリング処理 [Renderer.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Renderer.h"
#include "Manager.h"
#include "imguimaneger.h"
#include "Game.h"
#include "ParticleRenderer2D.h"
#include "ParticleRenderer3D.h"
#include "ShaderManager.h"
#include "StageManager.h"
#include "ShaderResource.h"
#include "ModelInstancingRenderer.h"
#include "ShadowMap.h"
#include "ColorConstants.h"
#include "DeviceResourceManager.h"


//*****************************************************************************
// 静的メンバ変数宣言
//*****************************************************************************
//int CRenderer::m_nFPS = 0;

namespace GuiParam
{
	constexpr float SPACING_Y	= 10.0f;	// Yの余白
	constexpr float POS_X		= 20.0f;	// Xの位置
	constexpr float POS_Y		= 20.0f;	// Yの位置
	constexpr float SIZE_X		= 420.0f;	// Xのサイズ
	constexpr float SIZE_Y		= 500.0f;	// Yのサイズ
}

// 名前空間 ColorConstの使用
using namespace ColorConst;

//=============================================================================
// コンストラクタ
//=============================================================================
CRenderer::CRenderer() :
	m_pD3D			(nullptr),				// DirectX3Dオブジェクトへのポインタ
	m_pD3DDevice	(nullptr),				// デバイスへのポインタ
	m_ResizeWidth	(0),					// 再設定時の画面幅
	m_ResizeHeight	(0),					// 再設定時の画面高さ
	m_d3dpp			({}),					// 再設定用のパラメーター
	m_bgCol			(DX::BLACK),			// 背景の色
	m_bloomPower	(0.0f)					// ブルーム強さ
{

}
//=============================================================================
// デストラクタ
//=============================================================================
CRenderer::~CRenderer()
{
	// なし
}
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CRenderer::Init(HWND hWnd, BOOL bWindow)
{
	D3DDISPLAYMODE d3ddm;			// ディスプレイモード

	// DirectX3Dオブジェクトの生成
	m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);

	if (m_pD3D == nullptr)
	{
		return E_FAIL;
	}

	// 現在のディスプレイモードを取得
	if (FAILED(m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
	{
		return E_FAIL;
	}

	// デバイスのプレゼンテーションパラメータの設定
	ZeroMemory(&m_d3dpp, sizeof(m_d3dpp));								// パラメータのゼロクリア

	m_d3dpp.BackBufferWidth				= SCREEN_WIDTH;					// ゲーム画面サイズ（幅）
	m_d3dpp.BackBufferHeight			= SCREEN_HEIGHT;				// ゲーム画面サイズ（高さ）
	m_d3dpp.BackBufferFormat			= d3ddm.Format;					// バックバッファの形式
	m_d3dpp.BackBufferCount				= 1;							// バックバッファの数
	m_d3dpp.SwapEffect					= D3DSWAPEFFECT_DISCARD;		// ダブルバッファの切り替え
	m_d3dpp.EnableAutoDepthStencil		= TRUE;							// デプスバッファとステンシルバッファを作成
	m_d3dpp.AutoDepthStencilFormat		= D3DFMT_D24S8;					// デプスバッファとして16bitを使う
	m_d3dpp.Windowed					= bWindow;						// ウインドウモード
	m_d3dpp.FullScreen_RefreshRateInHz	= D3DPRESENT_RATE_DEFAULT;		// リフレッシュレート
	m_d3dpp.PresentationInterval		= D3DPRESENT_INTERVAL_DEFAULT;	// インターバル

	// DirectX3Dデバイスの生成
	if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&m_d3dpp,
		&m_pD3DDevice)))
	{
		// DirectX3Dデバイスの生成
		if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			hWnd,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&m_d3dpp,
			&m_pD3DDevice)))
		{
			// DirectX3Dデバイスの生成
			if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
				D3DDEVTYPE_REF,
				hWnd,
				D3DCREATE_SOFTWARE_VERTEXPROCESSING,
				&m_d3dpp,
				&m_pD3DDevice)))
			{
				return E_FAIL;
			}
		}
	}

	// レンダーステートの設定
	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// サンプラーステートの設定
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	// テクスチャステージステートの設定
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

	//// デバッグフォントの初期化
	//m_pDebug->Init();

	//// 3Dデバッグ表示の初期化
	//m_pDebug3D->Init();

	auto shaderMgr = CManager::Instance()->GetShaderManager();

	m_pBrightShader = shaderMgr->Load("data/Shader/Bloom/BloomBrightPass.hlsl");

	m_pHorizontalShader = shaderMgr->Load("data/Shader/Bloom/BloomHorizontalBlur.hlsl");

	m_pVerticalShader = shaderMgr->Load("data/Shader/Bloom/BloomVerticalBlur.hlsl");

	m_pCombineShader = shaderMgr->Load("data/Shader/Bloom/BloomCombine.hlsl");

	// レンダーターゲット生成
	CreateRenderTargets();

	// シャドウマップ生成
	m_ShadowMap = std::make_unique<CShadowMap>();

	if (FAILED(m_ShadowMap->Init()))
	{
		return E_FAIL;
	}

	return S_OK;
}
//=============================================================================
// 終了処理
//=============================================================================
void CRenderer::Uninit(void)
{
	//// デバッグフォントの終了処理
	//m_pDebug->Uninit();

	//// 3Dデバッグ表示の終了処理
	//m_pDebug3D->Uninit();

	// Direct3Dデバイスの破棄
	if (m_pD3DDevice != nullptr)
	{
		m_pD3DDevice->Release();
		m_pD3DDevice = nullptr;
	}

	// Direct3Dオブジェクトの破棄
	if (m_pD3D != nullptr)
	{
		m_pD3D->Release();
		m_pD3D = nullptr;
	}

	m_SceneRT.Release();
	m_BrightRT.Release();
	m_BlurRT1.Release();
	m_BlurRT2.Release();

	if (m_pBackBuffer != nullptr)
	{
		m_pBackBuffer->Release();
		m_pBackBuffer = nullptr;
	}
}

//=============================================================================
// レンダーターゲットリリース
//=============================================================================
void CRenderer::ReleaseRenderTargets(void)
{
	m_SceneRT.Release();
	m_BrightRT.Release();
	m_BlurRT1.Release();
	m_BlurRT2.Release();

	if (m_pBackBuffer != nullptr)
	{
		m_pBackBuffer->Release();
		m_pBackBuffer = nullptr;
	}
}

//=============================================================================
// 開始
//=============================================================================
void CRenderer::Begin(void)
{
	// バックバッファを保存
	m_pD3DDevice->GetRenderTarget(0, &m_pBackBuffer);

	// 描画先をSceneRTへ切り替え
	m_SceneRT.Set();

	// クリア
	m_pD3DDevice->Clear(
		0,
		nullptr,
		D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		D3DCOLOR_XRGB(0, 0, 0),
		1.0f,
		0);

	m_pD3DDevice->BeginScene();
}

//=============================================================================
// 終了
//=============================================================================
void CRenderer::End(void)
{
	m_pD3DDevice->EndScene();

	// ブルーム描画
	RenderBloom();

	m_pD3DDevice->BeginScene();

#ifdef _DEBUG

	// ImGuiの描画終了
	CImGuiManager::Instance().EndImguiRender();

#endif

	m_pD3DDevice->EndScene();

	m_pD3DDevice->Present(nullptr, nullptr, nullptr, nullptr);

	// 毎フレーム取得したBackBufferは解放
	if (m_pBackBuffer != nullptr)
	{
		m_pBackBuffer->Release();
		m_pBackBuffer = nullptr;
	}
}

//=============================================================================
// デバイスのリセット
//=============================================================================
void CRenderer::ResetDevice(void)
{
	if (!m_pD3DDevice || m_ResizeWidth == 0 || m_ResizeHeight == 0)
	{
		return;
	}

	//// デバッグフォントの破棄
	//m_pDebug->Uninit();

	//// 3Dデバッグ表示の破棄
	//m_pDebug3D->Uninit();

	//// 全UIにデバイス再設定通知
	//CUIManager* pUIManager = CUIManager::GetInstance();

	//for (auto ui : pUIManager->GetAllUI())
	//{
	//	ui->Uninit(); // CUIText の場合は D3DXFONT を再生成
	//}

	m_d3dpp.BackBufferWidth = m_ResizeWidth;
	m_d3dpp.BackBufferHeight = m_ResizeHeight;
	m_ResizeWidth = m_ResizeHeight = 0;

	// パーティクルのリセット
	CManager::Instance()->GetParticleRenderer2D()->OnLostDevice();
	CManager::Instance()->GetParticleRenderer3D()->OnLostDevice();

	// モデルインスタンシングのリセット
	CManager::Instance()->GetModelInstancingRenderer()->OnLostDevice();

	// シェーダーのリセット
	CManager::Instance()->GetShaderManager()->OnLostDevice();

	// デバイスリソースマネージャーのデバイスロスト通知
	CDeviceResourceManager::GetInstance().OnLostDevice();

	ImGui_ImplDX9_InvalidateDeviceObjects();

	// レンダーターゲットのリリース
	ReleaseRenderTargets();

	// シャドウマップ
	m_ShadowMap->OnLostDevice();

	// ステージエディタのサムネイルのリリース通知
	CManager::Instance()->GetStageManager()->ReleaseThumbnail();

	HRESULT hr = m_pD3DDevice->Reset(&m_d3dpp);

	if (hr == D3DERR_INVALIDCALL)
	{
		IM_ASSERT(0);
	}

	ImGui_ImplDX9_CreateDeviceObjects();

	// デバイスリソースマネージャーのデバイスリセット通知
	CDeviceResourceManager::GetInstance().OnResetDevice();

	// パーティクルのリセット
	CManager::Instance()->GetParticleRenderer2D()->OnResetDevice();
	CManager::Instance()->GetParticleRenderer3D()->OnResetDevice();

	// モデルインスタンシングのリセット
	CManager::Instance()->GetModelInstancingRenderer()->OnResetDevice();

	// レンダーターゲット生成
	CreateRenderTargets();

	// シェーダーのリセット
	CManager::Instance()->GetShaderManager()->OnResetDevice();

	// シャドウマップ
	m_ShadowMap->OnResetDevice();

	// ステージエディタのサムネイルのリセット通知
	CManager::Instance()->GetStageManager()->ResetThumbnail();

	// レンダーステートの設定
	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// サンプラーステートの設定
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	// テクスチャステージステートの設定
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

	//// デバッグフォントの初期化
	//m_pDebug->Init();

	//// 3Dデバッグ表示の初期化
	//m_pDebug3D->Init();

	//for (auto ui : pUIManager->GetAllUI())
	//{
	//	ui->Init(); // CUIText の場合は D3DXFONT を再生成
	//}

	// デバイスリセット通知(シーンライト)
	CManager::Instance()->OnDeviceReset();
}

//=============================================================================
// レンダーターゲット生成処理
//=============================================================================
HRESULT CRenderer::CreateRenderTargets(void)
{
	UINT w = m_d3dpp.BackBufferWidth;
	UINT h = m_d3dpp.BackBufferHeight;

	// Scene
	if (FAILED(m_SceneRT.Create(w, h, D3DFMT_A8R8G8B8, D3DFMT_D24S8)))
	{
		return E_FAIL;
	}

	// Bright
	if (FAILED(m_BrightRT.Create(w / 2, h / 2, D3DFMT_A8R8G8B8, D3DFMT_UNKNOWN)))
	{
		return E_FAIL;
	}

	// Blur1
	if (FAILED(m_BlurRT1.Create(w / 2, h / 2, D3DFMT_A8R8G8B8, D3DFMT_UNKNOWN)))
	{
		return E_FAIL;
	}

	// Blur2
	if (FAILED(m_BlurRT2.Create(w / 2, h / 2, D3DFMT_A8R8G8B8, D3DFMT_UNKNOWN)))
	{
		return E_FAIL;
	}

	return S_OK;
}

//=============================================================================
// フルスクリーンクアッド描画処理
//=============================================================================
void CRenderer::DrawFullscreenQuad(ID3DXEffect* effect, float width, float height)
{
	POST_VERTEX v[] =
	{
		{ -0.5f,                 -0.5f,                  0,1, 0,0 },
		{ width - 0.5f,      -0.5f,                  0,1, 1,0 },
		{ -0.5f,                  height - 0.5f,     0,1, 0,1 },
		{ width - 0.5f,      height - 0.5f,     0,1, 1,1 },
	};

	UINT passCount;

	effect->Begin(&passCount, 0);

	for (UINT i = 0; i < passCount; i++)
	{
		effect->BeginPass(i);

		m_pD3DDevice->SetFVF(FVF_POST);

		m_pD3DDevice->DrawPrimitiveUP(
			D3DPT_TRIANGLESTRIP,
			2,
			v,
			sizeof(POST_VERTEX));

		effect->EndPass();
	}

	effect->End();
}

//=============================================================================
// ブルーム描画処理
//=============================================================================
void CRenderer::RenderBloom(void)
{
	m_pD3DDevice->BeginScene();

	//
	// Bright
	//

	m_BrightRT.Begin();

	ID3DXEffect* effect = m_pBrightShader->GetEffect();

	effect->SetTexture("SceneTexture", m_SceneRT.GetTexture());

	// ビューポート設定
	SetViewport(m_d3dpp.BackBufferWidth / 2, m_d3dpp.BackBufferHeight / 2);

	DrawFullscreenQuad(effect, static_cast<float>(m_d3dpp.BackBufferWidth / 2.0f), static_cast<float>(m_d3dpp.BackBufferHeight / 2.0f));

	m_BrightRT.End();

	//
	// Horizontal
	//

	m_BlurRT1.Begin();

	ID3DXEffect* effect2 = m_pHorizontalShader->GetEffect();

	effect2->SetTexture("SceneTexture", m_BrightRT.GetTexture());
	effect2->SetFloat("TexelWidth", 1.0f / (m_d3dpp.BackBufferWidth / 2.0f));

	// ビューポート設定
	SetViewport(m_d3dpp.BackBufferWidth / 2, m_d3dpp.BackBufferHeight / 2);

	DrawFullscreenQuad(effect2, static_cast<float>(m_d3dpp.BackBufferWidth / 2.0f), static_cast<float>(m_d3dpp.BackBufferHeight / 2.0f));
	
	m_BlurRT1.End();

	//
	// Vertical
	//

	m_BlurRT2.Begin();

	ID3DXEffect* effect3 = m_pVerticalShader->GetEffect();

	effect3->SetTexture("SceneTexture", m_BlurRT1.GetTexture());
	effect3->SetFloat("TexelHeight", 1.0f / (m_d3dpp.BackBufferHeight / 2.0f));

	// ビューポート設定
	SetViewport(m_d3dpp.BackBufferWidth / 2, m_d3dpp.BackBufferHeight / 2);

	DrawFullscreenQuad(effect3, static_cast<float>(m_d3dpp.BackBufferWidth / 2.0f), static_cast<float>(m_d3dpp.BackBufferHeight / 2.0f));

	m_BlurRT2.End();

	//
	// BackBuffer
	//

	m_pD3DDevice->SetRenderTarget(0, m_pBackBuffer);

	ID3DXEffect* effect4 = m_pCombineShader->GetEffect();

	effect4->SetTexture("SceneTexture", m_SceneRT.GetTexture());
	effect4->SetTexture("BloomTexture", m_BlurRT1.GetTexture());
	effect4->SetFloat("BloomPower", m_bloomPower);

	// ビューポート設定
	SetViewport(m_d3dpp.BackBufferWidth, m_d3dpp.BackBufferHeight);

	DrawFullscreenQuad(effect4, static_cast<float>(m_d3dpp.BackBufferWidth), static_cast<float>(m_d3dpp.BackBufferHeight));

	m_pD3DDevice->EndScene();
}
//=============================================================================
// ビューポート設定
//=============================================================================
void CRenderer::SetViewport(UINT width, UINT height)
{
	D3DVIEWPORT9 vp{};
	vp.X = 0;
	vp.Y = 0;
	vp.Width = width;
	vp.Height = height;
	vp.MinZ = 0.0f;
	vp.MaxZ = 1.0f;

	m_pD3DDevice->SetViewport(&vp);
}

//=============================================================================
// シャドウマップの取得
//=============================================================================
CShadowMap* CRenderer::GetShadowMap(void)
{ 
	return m_ShadowMap.get(); 
}

//=============================================================================
// サイズの再設定
//=============================================================================
void CRenderer::OnResize(UINT width, UINT height)
{
	m_ResizeWidth = width;
	m_ResizeHeight = height;
}
//=============================================================================
// デバイスのリセットが必要かどうか
//=============================================================================
bool CRenderer::NeedsReset(void) const
{
	return (m_ResizeWidth != 0 && m_ResizeHeight != 0);
}