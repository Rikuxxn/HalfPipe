//=============================================================================
//
// パーティクルシステム処理 [ParticleSystem.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _PARTICLE_SYSTEM_H_// このマクロ定義がされていなかったら
#define _PARTICLE_SYSTEM_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include <ParticleEmitterComponent.h>

class CParticleEmitterComponent;

//*****************************************************************************
// パーティクルシステムクラス
//*****************************************************************************
class CParticleSystem
{
public:
    CParticleSystem();
    ~CParticleSystem();

    void AddEmitter(CParticleEmitterComponent* emitter);

    void RemoveEmitter(CParticleEmitterComponent* emitter);

    const std::vector<CParticleEmitterComponent*>& GetEmitters(void) const { return m_Emitters; }

private:
    std::vector<CParticleEmitterComponent*> m_Emitters;
};


//*****************************************************************************
// レンダラー共通インターフェースクラス
//*****************************************************************************
class IParticleRenderer
{
public:
    virtual void Draw(
        const std::vector<PARTICLE>& particles,
        LPDIRECT3DTEXTURE9 texture,
        ID3DXEffect* effect) = 0;
};

#endif