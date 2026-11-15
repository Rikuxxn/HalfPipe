//=============================================================================
//
// パーティクル速度モジュール [ParticleVelocityModule.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _PARTICLE_VELOCITY_MODULE_H_// このマクロ定義がされていなかったら
#define _PARTICLE_VELOCITY_MODULE_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "ParticleModule.h"
#include <Random.h>


//*****************************************************************************
// パーティクル速度モジュールクラス
//*****************************************************************************
class CParticleVelocityModule : public IParticleModule
{
public:

    // 速度の最小値
    D3DXVECTOR3 VelocityMin = { -1, -1, -1 };

    // 速度の最大値
    D3DXVECTOR3 VelocityMax = { 1, 1, 1 };

    void OnSpawn(PARTICLE& p) override
    {
        p.Velocity =
        {
            CRandom::RandF(VelocityMin.x, VelocityMax.x),
            CRandom::RandF(VelocityMin.y, VelocityMax.y),
            CRandom::RandF(VelocityMin.z, VelocityMax.z)
        };
    }
};

#endif