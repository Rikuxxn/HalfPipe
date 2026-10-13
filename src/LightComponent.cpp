//=============================================================================
//
// ライトコンポーネント処理 [LightComponent.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "LightComponent.h"
#include "GameObject.h"
#include "LightManager.h"
#include "TransformComponent.h"
#include "Manager.h"
#include "Renderer.h"

//=============================================================================
// コンストラクタ
//=============================================================================
CLightComponent::CLightComponent()
{
    ZeroMemory(&m_Light, sizeof(D3DLIGHT9));
    m_Enable = true;
    m_LightIndex = -1;
    m_Light.Type = D3DLIGHT_DIRECTIONAL;
    m_Light.Diffuse = { 1.0f,1.0f,1.0f,1.0f };
    m_Light.Ambient = { 0.3f,0.3f,0.3f,1.0f };
    m_Light.Specular = { 1.0f,1.0f,1.0f,1.0f };
    m_Light.Direction = { 0.0f, -1.0f,0.0f };
    m_Light.Range = 1000.0f;
}

//=============================================================================
// デストラクタ
//=============================================================================
CLightComponent::~CLightComponent()
{
    OnDestroy();
}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CLightComponent::Init(void)
{
    auto device = CManager::Instance()->GetRenderer()->GetDevice();

    m_LightIndex = CLightManager::Instance()->RegisterLight(m_Light);

    device->SetLight(m_LightIndex, &m_Light);
    device->LightEnable(m_LightIndex, TRUE);

    m_Enable = true;

    return S_OK;
}

//=============================================================================
// ライトの破棄
//=============================================================================
void CLightComponent::OnDestroy(void)
{
    if (m_LightIndex >= 0)
    {
        auto device = CManager::Instance()->GetRenderer()->GetDevice();

        device->LightEnable(m_LightIndex, FALSE);

        CLightManager::Instance()->FreeLight(m_LightIndex);

        m_LightIndex = -1;
    }
}

//=============================================================================
// 更新処理
//=============================================================================
void CLightComponent::Update(void)
{
    auto transform = m_pOwner->GetComponent<CTransformComponent>();

    if (!transform)
    {
        return;
    }

    //---------------------------------
    // 座標同期
    //---------------------------------

    if (m_Light.Type != D3DLIGHT_DIRECTIONAL)
    {
        m_Light.Position = transform->Position;
    }

    CLightManager::Instance()->UpdateLight(m_LightIndex, m_Light);
}

//=============================================================================
// ディフューズの設定
//=============================================================================
void CLightComponent::SetDiffuse(const D3DXCOLOR& color)
{
    m_Light.Diffuse = color;
}

//=============================================================================
// アンビエントの設定
//=============================================================================
void CLightComponent::SetAmbient(const D3DXCOLOR& color)
{
    m_Light.Ambient = color;
}

//=============================================================================
// スペキュラーの設定
//=============================================================================
void CLightComponent::SetSpecular(const D3DXCOLOR& color)
{
    m_Light.Specular = color;
}

//=============================================================================
// 方向の設定
//=============================================================================
void CLightComponent::SetDirection(const D3DXVECTOR3& dir)
{
    D3DXVECTOR3 n = dir;

    // 正規化
    D3DXVec3Normalize(&n, &n);

    m_Light.Direction = n;
}

//=============================================================================
// 範囲の設定
//=============================================================================
void CLightComponent::SetRange(float range)
{
    m_Light.Range = range;
}

//=============================================================================
// の設定
//=============================================================================
void CLightComponent::SetAttenuation(float a0, float a1, float a2)
{
    m_Light.Attenuation0 = a0;
    m_Light.Attenuation1 = a1;
    m_Light.Attenuation2 = a2;
}

//=============================================================================
// の設定
//=============================================================================
void CLightComponent::SetTheta(float theta)
{
    m_Light.Theta = D3DXToRadian(theta);
}

//=============================================================================
// の設定
//=============================================================================
void CLightComponent::SetPhi(float phi)
{
    m_Light.Phi = D3DXToRadian(phi);
}

//=============================================================================
// の設定
//=============================================================================
void CLightComponent::SetFalloff(float falloff)
{
    m_Light.Falloff = falloff;
}