//=============================================================================
//
// パーティクルレンダラー3D処理 [ParticleRenderer3D.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _PARTICLE_RENDERER_3D_H_// このマクロ定義がされていなかったら
#define _PARTICLE_RENDERER_3D_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "ParticleEmitterComponent.h"
#include "ParticleRenderer.h"


//*****************************************************************************
// パーティクルレンダー3Dクラス
//*****************************************************************************
class CParticleRenderer3D : public CParticleRenderer
{
public:
    CParticleRenderer3D();
    ~CParticleRenderer3D() override;

    HRESULT Init(void) override;

    void Draw(const std::vector<PARTICLE>& particles,
        LPDIRECT3DTEXTURE9 texture,
        ID3DXEffect* effect) override;

    // デバイスリセット時
    void OnLostDevice(void) override;
    void OnResetDevice(void) override;

private:

    struct INSTANCE_DATA
    {
        D3DXVECTOR3 Position;
        float       Size;
        D3DCOLOR    Color;
    };

    struct QUAD_VERTEX
    {
        D3DXVECTOR3 Pos;
        D3DXVECTOR2 Tex;
    };

private:
    ComPtr<IDirect3DVertexBuffer9> m_pQuadVB;
    ComPtr<IDirect3DVertexBuffer9> m_pInstanceVB;
    ComPtr<IDirect3DIndexBuffer9>  m_pIdxBuff;

    LPDIRECT3DVERTEXDECLARATION9   m_pVertexDecl;
};

#endif