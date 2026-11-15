//=============================================================================
//
// パーティクルエミッターコンポーネント処理 [ParticleEmitterComponent.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================
#include "ParticleEmitterComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "Manager.h"
#include "TextureManager.h"
#include "TextureResource.h"
#include "Random.h"
#include "MathConstants.h"
#include "ShaderResource.h"
#include "ShaderManager.h"
#include "ParticleRenderer2D.h"
#include "ParticleRenderer3D.h"


//=============================================================================
// コンストラクタ
//=============================================================================
CParticleEmitterComponent::CParticleEmitterComponent() :
    m_IsPlay (false)
{

}

//=============================================================================
// デストラクタ
//=============================================================================
CParticleEmitterComponent::~CParticleEmitterComponent()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CParticleEmitterComponent::Init(void)
{
    m_Particles.resize(MAX_PARTICLES);

    for (auto& p : m_Particles)
    {
        p.Active = false;
    }

	return S_OK;
}

//=============================================================================
// 更新処理
//=============================================================================
void CParticleEmitterComponent::Update(void)
{
    //---------------------------------
    // 発生
    //---------------------------------

    if (EmitMode == ParticleEmitMode::Loop && m_IsPlay)
    {
        m_EmitTimer += 1.0f;

        if (m_EmitTimer >= EmitRate)
        {
            Emit(1);

            m_EmitTimer = 0;
        }
    }

    //---------------------------------
    // 更新
    //---------------------------------

    UpdateParticles();
}

//=============================================================================
// 描画処理
//=============================================================================
void CParticleEmitterComponent::Draw(void)
{
    if (Space == ParticleSpace::Space2D)
    {
        // 2Dパーティクルの描画
        auto particleRenderer2D = CManager::Instance()->GetParticleRenderer2D();

        if (particleRenderer2D == nullptr)
        {
            return;
        }

        particleRenderer2D->Draw(
            m_Particles,
            GetTexture(),
            GetShader());
    }
    else
    {
        // 3Dパーティクルの描画
        auto particleRenderer3D = CManager::Instance()->GetParticleRenderer3D();

        if (particleRenderer3D == nullptr)
        {
            return;
        }

        particleRenderer3D->Draw(
            m_Particles,
            GetTexture(),
            GetShader());
    }
}

//=============================================================================
// エミッター
//=============================================================================
void CParticleEmitterComponent::Emit(int count)
{
    auto transform = m_pOwner->GetComponent<CTransformComponent>();

    if (transform == nullptr)
    {
        return;
    }

    for (int i = 0; i < count; i++)
    {
        PARTICLE& p = m_Particles[m_NextParticle];

        m_NextParticle++;

        if (m_NextParticle >= MAX_PARTICLES)
        {
            m_NextParticle = 0;
        }

        p = PARTICLE();
        p.Active = true;

        //---------------------------------
        // 生成位置
        //---------------------------------

        p.Position = transform->Position + SpawnPosition();

        //---------------------------------
        // その他
        //---------------------------------

        p.Size = CRandom::RandF(SizeMin, SizeMax);
        p.LifeTime = CRandom::RandF(LifeTimeMin, LifeTimeMax);
        p.Color = col;

        // モジュール初期化
        for (auto& module : m_Modules)
        {
            module->OnSpawn(p);
        }
    }
}

//=============================================================================
// パーティクル更新
//=============================================================================
void CParticleEmitterComponent::UpdateParticles(void)
{
    for (auto& p : m_Particles)
    {
        if (!p.Active)
        {
            continue;
        }

        p.Age++;

        if (p.Age >= p.LifeTime)
        {
            p.Active = false;
            continue;
        }

        // 停止の場合はスキップ
        if (!m_IsPlay)
        {
            continue;
        }

        // 各モジュールで更新
        for (auto& module : m_Modules)
        {
            module->Update(p);
        }
    }
}

//=============================================================================
// 生成位置
//=============================================================================
D3DXVECTOR3 CParticleEmitterComponent::SpawnPosition(void)
{
    switch (SpawnShape)
    {
    case ParticleSpawnShape::Point:
        return { 0,0,0 };

    case ParticleSpawnShape::Box:
    {
        if (Space == ParticleSpace::Space2D)
        {
            return
            {
                CRandom::RandF(-BoxSize.x * MathConst::HALF, BoxSize.x * MathConst::HALF),
                CRandom::RandF(-BoxSize.y * MathConst::HALF, BoxSize.y * MathConst::HALF),
                0
            };
        }
        else if (Space == ParticleSpace::Space3D)
        {
            return
            {
                CRandom::RandF(-BoxSize.x * MathConst::HALF, BoxSize.x * MathConst::HALF),
                CRandom::RandF(-BoxSize.y * MathConst::HALF, BoxSize.y * MathConst::HALF),
                CRandom::RandF(-BoxSize.z * MathConst::HALF, BoxSize.z * MathConst::HALF),
            };
        }
    }

    case ParticleSpawnShape::Circle:
    {
        float angle = CRandom::RandF(0, D3DX_PI * MathConst::F_DOUBLE);

        float r = CRandom::RandF(0, Radius);

        return
        {
            cosf(angle) * r,
            sinf(angle) * r,
            0
        };
    }
    case ParticleSpawnShape::Sphere:
    {
        float theta = CRandom::RandF(0.0f, D3DX_PI * MathConst::F_DOUBLE);
        float phi = CRandom::RandF(0.0f, D3DX_PI);

        float r = CRandom::RandF(0.0f, Radius);

        return
        {
            r * sinf(phi) * cosf(theta),
            r * cosf(phi),
            r * sinf(phi) * sinf(theta)
        };
    }
    }

    return { 0,0,0 };
}

//=============================================================================
// シェーダーの設定
//=============================================================================
void CParticleEmitterComponent::SetShader(const std::string& path)
{
    m_Shader = CManager::Instance()->GetShaderManager()->Load(path);
}

//=============================================================================
// シェーダーの取得
//=============================================================================
ID3DXEffect* CParticleEmitterComponent::GetShader(void) const
{
    if (!m_Shader)
    {
        return nullptr;
    }

    return m_Shader->GetEffect();
}

//=============================================================================
// テクスチャの設定
//=============================================================================
void CParticleEmitterComponent::SetTexture(const std::string& path)
{
    m_TextureResource = CManager::Instance()->GetTextureManager()->Load(path);
}

//=============================================================================
// テクスチャの取得
//=============================================================================
LPDIRECT3DTEXTURE9 CParticleEmitterComponent::GetTexture(void) const
{
    if (!m_TextureResource)
    {
        return nullptr;
    }

    return m_TextureResource->Texture.Get();
}
