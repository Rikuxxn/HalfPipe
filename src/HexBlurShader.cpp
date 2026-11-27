//=============================================================================
//
// 六角形ブラーシェーダー処理 [HexBlurShader.cpp]
// Author : SOICHIRO SASAKI
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "HexBlurShader.h"
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
void CHexShader::Apply(CGameObject* owner)
{
    auto effect = GetEffect();

    m_VerticalTexture = CManager::Instance()->GetRenderer()->GetSceneRT().GetTexture();
    m_DiagonalTexture = CManager::Instance()->GetRenderer()->GetHexBlur().GetTexture();

    effect->SetTexture("g_verticalTexture", m_VerticalTexture);
    effect->SetTexture("g_diagonalTexture", m_DiagonalTexture);
}