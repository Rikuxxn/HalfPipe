//=============================================================================
//
// パーティクルカラーモジュール [ParticleColorModule.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _PARTICLE_COLOR_MODULE_H_// このマクロ定義がされていなかったら
#define _PARTICLE_COLOR_MODULE_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "ParticleModule.h"


//*****************************************************************************
// パーティクルカラーモジュールクラス
//*****************************************************************************
class CParticleColorModule : public IParticleModule
{
public:
    CParticleColorModule() = default;
    ~CParticleColorModule() = default;

    D3DXCOLOR StartColor    = D3DXCOLOR(1, 1, 1, 1);
    D3DXCOLOR EndColor      = D3DXCOLOR(1, 1, 1, 0);

    void SetColor(const D3DXCOLOR& color)
    {
        StartColor = color;
        EndColor = color;
    }

    void Update(PARTICLE& p) override
    {
        float t = p.Age / p.LifeTime;

        p.Color.r = StartColor.r + (EndColor.r - StartColor.r) * t;
        p.Color.g = StartColor.g + (EndColor.g - StartColor.g) * t;
        p.Color.b = StartColor.b + (EndColor.b - StartColor.b) * t;
        p.Color.a = StartColor.a + (EndColor.a - StartColor.a) * t;
    }
};

#endif