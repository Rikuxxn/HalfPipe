//=============================================================================
//
// レンダーターゲット処理 [RenderTarget.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _RENDER_TARGET_H_// このマクロ定義がされていなかったら
#define _RENDER_TARGET_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "DeviceResource.h"


//*****************************************************************************
// レンダーターゲットクラス
//*****************************************************************************
class CRenderTarget : public IDeviceResource
{
public:
    CRenderTarget();
    ~CRenderTarget();

    HRESULT Create(UINT width, UINT height, D3DFORMAT colorFormat, D3DFORMAT depthFormat);

    void Release(void);

    void Begin(void);
    void End(void);
    void Set(void);

    // デバイスロスト・リセット時
    void OnLostDevice(void) override;
    void OnResetDevice(void) override;
    void ReleaseResource(void) override;

    IDirect3DTexture9* GetTexture(void) const;
    IDirect3DSurface9* GetColorSurface(void) const;

    UINT        GetWidth        (void) { return m_Width; }
    UINT        GetHeight       (void) { return m_Height; }
    D3DFORMAT   GetColorFormat  (void) { return m_ColorFormat; }
    D3DFORMAT   GetDepthFormat  (void) { return m_DepthFormat; }

private:

    ComPtr<IDirect3DTexture9> m_Texture;        // テクスチャ
    ComPtr<IDirect3DSurface9> m_ColorSurface;   // カラーサーフェス
    ComPtr<IDirect3DSurface9> m_DepthSurface;   // 深度サーフェス
    ComPtr<IDirect3DSurface9> m_BackupColor;    // バックアップカラー
    ComPtr<IDirect3DSurface9> m_BackupDepth;    // バックアップ深度

    UINT m_Width;
    UINT m_Height;

    D3DFORMAT m_ColorFormat;
    D3DFORMAT m_DepthFormat;
};

#endif