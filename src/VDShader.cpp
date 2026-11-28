//=============================================================================
//
// VDブラーシェーダー処理 [VDShader.cpp]
// Author : SOICHIRO SASAKI
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "VDShader.h"
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
void CVDShader::Apply(CGameObject* owner)
{
    auto effect = GetEffect();

    m_MainTexture = CManager::Instance()->GetRenderer()->GetSceneRT().GetTexture();

    effect->SetTexture("g_mainTexture", m_MainTexture);
}