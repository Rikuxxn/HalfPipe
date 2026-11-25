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
#include "RenderTarget.h"
#include "RenderTargetMulti.h"



//=============================================================================
// シェーダー適用
//=============================================================================
void CDoFShader::Apply(CGameObject* owner)
{
    auto effect = GetEffect();

    m_mainTexture=CManager::Instance()->GetRenderer()->GetSceneRT().GetTexture();
    m_gaussianTexture = CManager::Instance()->GetRenderer()->GetZScene().GetTexture();
    m_depthTexture = CManager::Instance()->GetRenderer()->GetZScene().GetTexture();

    effect->SetFloat("g_fGaussStart", 700.0f);
    effect->SetFloat("g_fGaussEnd", 1000.0f);
    effect->SetTexture("g_mainTexture", m_mainTexture);
    effect->SetTexture("g_gaussianTexture", m_gaussianTexture);
    effect->SetTexture("g_depthTexture", m_depthTexture);
}