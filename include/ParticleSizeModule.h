//=============================================================================
//
// パーティクルサイズモジュール [ParticleSizeModule.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _PARTICLE_SIZE_MODULE_H_// このマクロ定義がされていなかったら
#define _PARTICLE_SIZE_MODULE_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "ParticleModule.h"


//*****************************************************************************
// パーティクルサイズモジュールクラス
//*****************************************************************************
class CParticleSizeModule : public IParticleModule
{
public:
    CParticleSizeModule() = default;
    ~CParticleSizeModule() = default;

    float StartSize = 32.0f;
    float EndSize   = 0.0f;

    void OnSpawn(PARTICLE& p) override
    {
        p.Size = StartSize;
    }

    void Update(PARTICLE& p) override
    {
        float t = p.Age / p.LifeTime;

        p.Size = StartSize + (EndSize - StartSize) * t;
    }
};

#endif