//=============================================================================
//
// レンダーターゲット処理 [RenderTarget.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "RenderTarget.h"
#include "Manager.h"
#include "Renderer.h"


//=============================================================================
// コンストラクタ
//=============================================================================
CRenderTarget::CRenderTarget() :
    m_Texture       (nullptr),          // テクスチャ
    m_ColorSurface  (nullptr),          // カラーサーフェス
    m_DepthSurface  (nullptr),          // 深度サーフェス
    m_BackupColor   (nullptr),          // バックアップカラー
    m_BackupDepth   (nullptr),          // バックアップ深度
    m_Width         (0),                // 幅
    m_Height        (0),                // 高さ
    m_ColorFormat   (D3DFMT_UNKNOWN),   // カラーフォーマット
    m_DepthFormat   (D3DFMT_UNKNOWN)    // 深度フォーマット
{

}

//=============================================================================
// デストラクタ
//=============================================================================
CRenderTarget::~CRenderTarget()
{

}

//=============================================================================
// 生成処理
//=============================================================================
HRESULT CRenderTarget::Create(UINT width, UINT height, D3DFORMAT colorFormat, D3DFORMAT depthFormat)
{
    Release();

    // 再生成用保存
    m_Width = width;
    m_Height = height;
    m_ColorFormat = colorFormat;
    m_DepthFormat = depthFormat;

    // デバイスの取得
    auto device = CManager::Instance()->GetRenderer()->GetDevice();

    // テクスチャの生成
    HRESULT hr = device->CreateTexture(
        width,
        height,
        1,
        D3DUSAGE_RENDERTARGET,
        colorFormat,
        D3DPOOL_DEFAULT,
        m_Texture.ReleaseAndGetAddressOf(),
        nullptr);

    if (FAILED(hr))
    {
        return hr;
    }

    m_Texture->GetSurfaceLevel(0, m_ColorSurface.ReleaseAndGetAddressOf());

    if (depthFormat != D3DFMT_UNKNOWN)
    {
        hr = device->CreateDepthStencilSurface(
            width,
            height,
            depthFormat,
            D3DMULTISAMPLE_NONE,
            0,
            TRUE,
            m_DepthSurface.ReleaseAndGetAddressOf(),
            nullptr);

        if (FAILED(hr))
        {
            return hr;
        }
    }

    return hr;
}

//=============================================================================
// 解放
//=============================================================================
void CRenderTarget::Release(void)
{
    m_Texture.Reset();
    m_ColorSurface.Reset();
    m_DepthSurface.Reset();

    m_BackupColor.Reset();
    m_BackupDepth.Reset();
}

//=============================================================================
// 描画開始
//=============================================================================
void CRenderTarget::Begin(void)
{
    // デバイスの取得
    auto device = CManager::Instance()->GetRenderer()->GetDevice();

    //-----------------------------
    // 現在のRT保存
    //-----------------------------

    device->GetRenderTarget(0, m_BackupColor.ReleaseAndGetAddressOf());

    device->GetDepthStencilSurface(m_BackupDepth.ReleaseAndGetAddressOf());

    //-----------------------------
    // RT切り替え
    //-----------------------------

    device->SetRenderTarget(0, m_ColorSurface.Get());

    if (m_DepthSurface)
    {
        device->SetDepthStencilSurface(m_DepthSurface.Get());
    }
    else
    {
        device->SetDepthStencilSurface(nullptr);
    }

    //-----------------------------
    // クリア
    //-----------------------------

    device->Clear(
        0,
        nullptr,
        D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
        0x00000000,
        1.0f,
        0);
}

//=============================================================================
// 描画終了
//=============================================================================
void CRenderTarget::End(void)
{
    // デバイスの取得
    auto device = CManager::Instance()->GetRenderer()->GetDevice();

    if (m_BackupColor)
    {
        device->SetRenderTarget(0, m_BackupColor.Get());
    }

    if (m_BackupDepth)
    {
        device->SetDepthStencilSurface(m_BackupDepth.Get());
    }

    m_BackupColor.Reset();
    m_BackupDepth.Reset();
}

//=============================================================================
// 設定
//=============================================================================
void CRenderTarget::Set(void)
{
    // デバイスの取得
    auto device = CManager::Instance()->GetRenderer()->GetDevice();

    device->SetRenderTarget(0, m_ColorSurface.Get());
    device->SetDepthStencilSurface(m_DepthSurface.Get());
}

//=============================================================================
// デバイスロスト時
//=============================================================================
void CRenderTarget::OnLostDevice(void)
{
    Release();
}

//=============================================================================
// デバイスリセット時
//=============================================================================
void CRenderTarget::OnResetDevice(void)
{
    HRESULT hr = Create(
        m_Width,
        m_Height,
        m_ColorFormat,
        m_DepthFormat);

    if (FAILED(hr))
    {
        return;
    }
}

//=============================================================================
// リソースの破棄
//=============================================================================
void CRenderTarget::ReleaseResource(void)
{
    Release();

    m_Width = 0;
    m_Height = 0;

    m_ColorFormat = D3DFMT_UNKNOWN;
    m_DepthFormat = D3DFMT_UNKNOWN;
}

//=============================================================================
// テクスチャ取得
//=============================================================================
IDirect3DTexture9* CRenderTarget::GetTexture(void) const
{
    return m_Texture.Get();
}

//=============================================================================
// カラーサーフェス取得
//=============================================================================
IDirect3DSurface9* CRenderTarget::GetColorSurface(void) const
{
    return m_ColorSurface.Get();
}