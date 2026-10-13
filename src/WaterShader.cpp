//=============================================================================
//
// 水シェーダー処理 [WaterShader.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "WaterShader.h"
#include "Manager.h"
#include "Renderer.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "Camera.h"
#include "TimeSystem.h"
#include "GameTime.h"
#include "SceneManager.h"
#include "Scene.h"
#include "EnvironmentUtility.h"

// 名前空間の使用
using namespace EnvironmentUtil;


//=============================================================================
// シェーダー適用
//=============================================================================
void CWaterShader::Apply(CGameObject* owner)
{
    auto effect = GetEffect();

    if (!effect)
    {
        return;
    }

    // デバイスの取得
    auto device = CManager::Instance()->GetRenderer()->GetDevice();

    if (!device)
    {
        return;
    }

    auto transform = owner->GetComponent<CTransformComponent>();

    D3DXMATRIX world;
    D3DXMATRIX view;
    D3DXMATRIX projection;

    // オブジェクトのWorld行列
    world = transform->GetWorldMatrix();

    // カメラ
    device->GetTransform(D3DTS_VIEW, &view);
    device->GetTransform(D3DTS_PROJECTION, &projection);

    //=========================================================================
    // 行列
    //=========================================================================
    effect->SetMatrix("World", &world);
    effect->SetMatrix("View", &view);
    effect->SetMatrix("Projection", &projection);

    //=========================================================================
    // カメラ
    //=========================================================================
    D3DXVECTOR3 cameraPos = CManager::Instance()->GetCamera()->GetPosV();

    effect->SetValue("CameraPos", &cameraPos, sizeof(D3DXVECTOR3));

    //=========================================================================
    // 時間(ゲーム時間)
    //=========================================================================
    float time = CManager::Instance()->GetGameTime()->GetTimeOfDay();

    effect->SetFloat("Time", time);

    //=========================================================================
    // 波 1
    //=========================================================================
    D3DXVECTOR2 waveDirection1(1.0f, 0.2f);

    effect->SetValue("WaveDirection1", &waveDirection1, sizeof(D3DXVECTOR2));
    effect->SetFloat("WaveAmplitude1", m_WaveAmplitude1);
    effect->SetFloat("WaveLength1", m_WaveLength1);
    effect->SetFloat("WaveSpeed1", m_WaveSpeed1);

    //=========================================================================
    // 波 2
    //=========================================================================
    D3DXVECTOR2 waveDirection2(-0.3f,1.0f);

    effect->SetValue("WaveDirection2", &waveDirection2, sizeof(D3DXVECTOR2));
    effect->SetFloat("WaveAmplitude2", m_WaveAmplitude2);
    effect->SetFloat("WaveLength2", m_WaveLength2);
    effect->SetFloat("WaveSpeed2", m_WaveSpeed2);

    //=========================================================================
    // 波 3
    //=========================================================================
    D3DXVECTOR2 waveDirection3(0.8f, -0.5f);

    effect->SetValue("WaveDirection3", &waveDirection3, sizeof(D3DXVECTOR2));
    effect->SetFloat("WaveAmplitude3", m_WaveAmplitude3);
    effect->SetFloat("WaveLength3", m_WaveLength3);
    effect->SetFloat("WaveSpeed3", m_WaveSpeed3);

    auto player =
        CManager::Instance()
        ->GetSceneManager()
        ->GetScene()
        ->FindGameObject("Player");

    if (player)
    {
        auto transformP =
            player->GetComponent<CTransformComponent>();

        if (transformP)
        {
            m_FoamPosition =
                transformP->Position;
        }
    }

    //=============================================================================
    // 白波
    //=============================================================================
    effect->SetValue("FoamPosition", &m_FoamPosition, sizeof(D3DXVECTOR3));
    effect->SetFloat("FoamRadius", m_FoamRadius);
    effect->SetFloat("FoamStrength", m_FoamStrength);

    //=========================================================================
    // 法線
    //=========================================================================
    effect->SetFloat("NormalStrength", m_NormalStrength);

    //=========================================================================
    // 水の色
    //=========================================================================
    D3DXVECTOR4 waterColor(0.03f, 0.30f, 0.38f, 1.0f);

    effect->SetValue("WaterColor", &waterColor, sizeof(D3DXVECTOR4));

    D3DXVECTOR4 deepWaterColor(0.01f, 0.08f, 0.12f, 1.0f);

    effect->SetValue("DeepWaterColor", &deepWaterColor, sizeof(D3DXVECTOR4));

    //=========================================================================
    // 太陽 ・ 月
    //=========================================================================
    D3DXVECTOR3 sunDir = GetSunDirection(time);
    D3DXVECTOR3 moonDir = GetMoonDirection(time);

    float sunHeight = sunDir.y;
    float moonHeight = moonDir.y;

    //=========================================================================
    // 現在の光源
    //=========================================================================
    D3DXVECTOR3 lightDir;
    D3DXVECTOR3 lightColor;
    float lightIntensity;

    //-------------------------------------------------------------------------
    // 太陽が地平線より上
    //-------------------------------------------------------------------------
    if (sunHeight > 0.05f)
    {
        // Surface → Sun
        lightDir = sunDir;

        lightColor = D3DXVECTOR3(1.0f, 0.95f, 0.85f);

        lightIntensity = 2.0f;
    }
    //-------------------------------------------------------------------------
    // 月が地平線より上
    //-------------------------------------------------------------------------
    else if (moonHeight > 0.05f)
    {
        // Surface → Moon
        lightDir = moonDir;

        lightColor = D3DXVECTOR3(0.35f, 0.45f, 0.70f);

        lightIntensity = 0.6f;
    }
    //-------------------------------------------------------------------------
    // 太陽も月も沈んでいる
    //-------------------------------------------------------------------------
    else
    {
        lightDir = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

        lightColor = D3DXVECTOR3(0.05f, 0.08f, 0.15f);

        lightIntensity = 0.1f;
    }

    effect->SetValue("LightDirection", &lightDir, sizeof(D3DXVECTOR3));
    effect->SetValue("LightColor", &lightColor, sizeof(D3DXVECTOR3));
    effect->SetFloat("LightIntensity", lightIntensity);

    //=========================================================================
    // フレネル設定
    //=========================================================================
    effect->SetFloat("FresnelPower", m_FresnelPower);
    effect->SetFloat("FresnelStrength", m_FresnelStrength);
}
