//=============================================================================
//
// パーティクルレンダラー2D処理 [ParticleRenderer2D.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _PARTICLE_RENDERER_2D_H_// このマクロ定義がされていなかったら
#define _PARTICLE_RENDERER_2D_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "ParticleEmitterComponent.h"
#include "ParticleRenderer.h"


//*****************************************************************************
// パーティクルレンダラー2Dクラス
//*****************************************************************************
class CParticleRenderer2D : public CParticleRenderer
{
public:
    CParticleRenderer2D();
    ~CParticleRenderer2D() override;

    HRESULT Init(void) override;

    void Draw(const std::vector<PARTICLE>& particles,
        LPDIRECT3DTEXTURE9 texture,
        ID3DXEffect* effect) override;

    // デバイスリセット時
    void OnLostDevice(void) override;
    void OnResetDevice(void) override;

private:
    static constexpr int MAX_PARTICLES = 1000;

    ComPtr<IDirect3DVertexBuffer9> m_pVtxBuff;
    ComPtr<IDirect3DIndexBuffer9>  m_pIdxBuff;
};

#endif