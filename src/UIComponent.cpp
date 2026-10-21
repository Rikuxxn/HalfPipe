//=============================================================================
//
// UIコンポーネント処理 [UIComponent.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "UIComponent.h"

//=============================================================================
// UVフレームの設定
//=============================================================================
void CUIComponent::SetUVFrame(int frameIndex, int frameCount)
{
    // フレーム数のチェック
    if (frameCount <= 0)
    {
        return;
    }

    // インデックスのチェック
    if (frameIndex < 0 || frameIndex >= frameCount)
    {
        return;
    }

    // 1フレーム分のUVサイズ
    float frameHeight = 1.0f / static_cast<float>(frameCount);

    // UVの開始位置
    m_Transform.UVPosition.x = 0.0f;
    m_Transform.UVPosition.y = frameHeight * frameIndex;

    // UVのサイズ
    m_Transform.UVSize.x = 1.0f;
    m_Transform.UVSize.y = frameHeight;
}