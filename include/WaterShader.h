//=============================================================================
//
// 水シェーダー処理 [WaterShader.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _WATER_SHADER_H_// このマクロ定義がされていなかったら
#define _WATER_SHADER_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "ShaderResource.h"


//*****************************************************************************
// 水シェーダークラス
//*****************************************************************************
class CWaterShader : public CShaderResource
{
public:

    virtual void Apply(CGameObject* owner) override;

private:

    // 波
    float m_WaveAmplitude1  = 1.0f;
    float m_WaveLength1     = 50.0f;
    float m_WaveSpeed1      = 30.0f;

    float m_WaveAmplitude2  = 0.8f;
    float m_WaveLength2     = 24.0f;
    float m_WaveSpeed2      = 40.0f;

    float m_WaveAmplitude3  = 0.6f;
    float m_WaveLength3     = 10.0f;
    float m_WaveSpeed3      = 60.0f;

    // 白波
    D3DXVECTOR3 m_FoamPosition  = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    float       m_FoamRadius    = 5.0f;
    float       m_FoamStrength  = 1.0f;

    // 法線
    float m_NormalStrength  = 0.35f;

    // フレネル
    float m_FresnelPower    = 5.0f;
    float m_FresnelStrength = 0.8f;
};

#endif