//=============================================================================
//
// シェーダーリソース処理 [ShaderResource.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "ShaderResource.h"
#include "Manager.h"
#include "Renderer.h"


//=============================================================================
// コンストラクタ
//=============================================================================
CShaderResource::CShaderResource()
{

}

//=============================================================================
// デストラクタ
//=============================================================================
CShaderResource::~CShaderResource()
{

}

//=============================================================================
// 読み込み処理
//=============================================================================
HRESULT CShaderResource::Load(const std::string& path)
{
    auto device = CManager::Instance()->GetRenderer()->GetDevice();

    LPD3DXBUFFER error = nullptr;

    HRESULT hr =
        D3DXCreateEffectFromFileA(
            device,
            path.c_str(),
            nullptr,
            nullptr,
            D3DXSHADER_DEBUG,
            nullptr,
            m_pEffect.GetAddressOf(),
            &error);

    if (FAILED(hr))
    {
        if (error)
        {
            OutputDebugStringA((char*)error->GetBufferPointer());

            error->Release();
        }

        return hr;
    }

    return S_OK;
}

//=============================================================================
// デバイスロスト時
//=============================================================================
void CShaderResource::OnLostDevice(void)
{
    if (m_pEffect)
    {
        m_pEffect->OnLostDevice();
    }
}

//=============================================================================
// リセットデバイス時
//=============================================================================
void CShaderResource::OnResetDevice(void)
{
    if (m_pEffect)
    {
        m_pEffect->OnResetDevice();
    }
}