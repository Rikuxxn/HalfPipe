//=============================================================================
//
// パーティクル重力モジュール [ParticleGravityModule.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _PARTICLE_GRAVITY_MODULE_H_// このマクロ定義がされていなかったら
#define _PARTICLE_GRAVITY_MODULE_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "ParticleModule.h"


//*****************************************************************************
// パーティクルグラビティモジュールクラス
//*****************************************************************************
class CParticleGravityModule : public IParticleModule
{
public:
    CParticleGravityModule() = default;
    ~CParticleGravityModule() = default;

    D3DXVECTOR3 Gravity = { 0.0f, -0.1f, 0.0f };

    void Update(PARTICLE& p) override
    {
        p.Velocity += Gravity;
        p.Position += p.Velocity;
    }
};

#endif