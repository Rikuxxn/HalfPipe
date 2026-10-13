//=============================================================================
//
// パーティクルレンダラー処理 [ParticleRenderer.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _PARTICLE_RENDERER_H_// このマクロ定義がされていなかったら
#define _PARTICLE_RENDERER_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "ParticleEmitterComponent.h"


//*****************************************************************************
// パーティクルレンダラー基底クラス
//*****************************************************************************
class CParticleRenderer
{
public:
    virtual ~CParticleRenderer() = default;

    virtual HRESULT Init(void) = 0;

    virtual void Draw(
        const std::vector<PARTICLE>& particles,
        LPDIRECT3DTEXTURE9 texture,
        ID3DXEffect* effect) = 0;

    virtual void OnLostDevice(void) = 0;
    virtual void OnResetDevice(void) = 0;
};

#endif