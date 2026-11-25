//=============================================================================
//
// 被写界深度シェーダー処理 [DepthOfFieldShader.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "DepthOfFieldShader.h"
#include "Manager.h"
#include "Renderer.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "Camera.h"
#include "TimeSystem.h"
#include "GameTime.h"
#include "RenderTargetMulti.h"



//=============================================================================
// シェーダー適用
//=============================================================================
void CDoFShader::Apply(CGameObject* owner)
{
    auto effect = GetEffect();

    m_mainTexture;
    m_gaussianTexture;
    m_depthTexture;

    effect->SetFloat("g_fGaussStart", m_fGaussStart);
    effect->SetFloat("g_fGaussEnd", m_fGaussEnd);
    effect->SetTexture("g_mainTexture", m_mainTexture);
    effect->SetTexture("g_gaussianTexture", m_gaussianTexture);
    effect->SetTexture("g_depthTexture", m_depthTexture);
}