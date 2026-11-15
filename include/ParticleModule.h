//=============================================================================
//
// パーティクルモジュール [ParticleModule.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _PARTICLE_MODULE_H_// このマクロ定義がされていなかったら
#define _PARTICLE_MODULE_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
//#include <ParticleEmitterComponent.h>

// パーティクル1つの構造体
struct PARTICLE
{
    D3DXVECTOR3 Position;
    D3DXVECTOR3 Velocity;
    D3DXCOLOR   Color;
    float       Size = 32.0f;
    float       Rotation = 0.0f;
    float       LifeTime = 60.0f;
    float       Age = 0.0f;
    bool        Active = false;
};

//*****************************************************************************
// パーティクルモジュールインターフェースクラス
//*****************************************************************************
class IParticleModule
{
public:

    virtual ~IParticleModule() = default;

    // パーティクル生成時
    virtual void OnSpawn(PARTICLE& /*particle*/) {}

    // パーティクル更新時
    virtual void Update(PARTICLE& /*particle*/) {}
};

#endif