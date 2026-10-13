//=============================================================================
//
// 空シェーダー処理 [SkyShader.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "SkyShader.h"
#include "Manager.h"
#include "Renderer.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "Camera.h"
#include "TimeSystem.h"
#include "GameTime.h"


//=============================================================================
// シェーダー適用
//=============================================================================
void CSkyShader::Apply(CGameObject* owner)
{
    auto effect = GetEffect();

    // デバイスの取得
    auto device = CManager::Instance()->GetRenderer()->GetDevice();

    // カメラの視点位置取得
    D3DXVECTOR3 camPos = CManager::Instance()->GetCamera()->GetPosV();

    D3DXMATRIX world, view, proj;

    auto transform = owner->GetComponent<CTransformComponent>();

    transform->Position = camPos;

    world = transform->GetWorldMatrix();

    device->GetTransform(D3DTS_VIEW, &view);
    device->GetTransform(D3DTS_PROJECTION, &proj);

    effect->SetMatrix("World", &world);
    effect->SetMatrix("View", &view);
    effect->SetMatrix("Projection", &proj);

    effect->SetValue("CameraPos", &camPos, sizeof(camPos));

    // ゲーム時間
    float time = CManager::Instance()->GetGameTime()->GetTimeOfDay();

    effect->SetFloat("TimeOfDay", time);

    // 太陽の設定
    effect->SetFloat("SunRadius", 0.0008f);
    effect->SetFloat("SunIntensity", 15.0f);

    // 月の設定
    effect->SetFloat("MoonRadius", 0.0015f);
    effect->SetFloat("MoonIntensity", 0.6f);

    // デルタタイムの取得
    float dt = CManager::Instance()->GetTimeSystem()->GetDeltaTime();

    // 雲の移動速度
    m_CloudTime += dt;

    // 雲の設定
    effect->SetFloat("CloudTime", m_CloudTime);
}
