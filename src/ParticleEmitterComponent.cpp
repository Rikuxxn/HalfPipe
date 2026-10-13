//=============================================================================
//
// パーティクルエミッターコンポーネント処理 [ParticleEmitterComponent.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================
#include "ParticleEmitterComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "ParticleRenderer2D.h"
#include "Manager.h"
#include "TextureManager.h"
#include "TextureResource.h"
#include "Random.h"
#include "MathConstants.h"
//#include "ParticleSystem.h"
#include "ShaderResource.h"
#include "ShaderManager.h"
#include "ParticleRenderer2D.h"
#include "ParticleRenderer3D.h"


//=============================================================================
// コンストラクタ
//=============================================================================
CParticleEmitterComponent::CParticleEmitterComponent()
{

}

//=============================================================================
// デストラクタ
//=============================================================================
CParticleEmitterComponent::~CParticleEmitterComponent()
{
    //if (m_pParticleSystem)
    //{
    //    m_pParticleSystem->RemoveEmitter(this);
    //}
}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CParticleEmitterComponent::Init(void)
{
    //m_pParticleSystem = CManager::Instance()->GetParticleSystem();

    //if (m_pParticleSystem)
    //{
    //    m_pParticleSystem->AddEmitter(this);
    //}

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

    if (EmitMode == ParticleEmitMode::Loop)
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
        // 初速度
        //---------------------------------

        switch (Type)
        {
        case ParticleType::Fire:
        {
            p.Velocity =
            {
                CRandom::RandF(-0.3f, 0.3f),
                CRandom::RandF(1.0f, 3.0f),
                CRandom::RandF(-0.3f, 0.3f)
            };
        }
        break;

        case ParticleType::Snow:
        {
            p.Velocity =
            {
                CRandom::RandF(-0.2f, 0.2f),
                CRandom::RandF(-1.0f, -0.3f),
                CRandom::RandF(-0.2f, 0.2f)
            };
        }
        break;

        default:
        {
            float speed = CRandom::RandF(SpeedMin, SpeedMax);

            if (Space == ParticleSpace::Space2D)
            {
                float angle = CRandom::RandF(0.0f, D3DX_PI * MathConst::F_DOUBLE);

                p.Velocity = { cosf(angle) * speed, sinf(angle) * speed, 0.0f };
            }
            else
            {
                float theta = CRandom::RandF(0.0f, D3DX_PI * MathConst::F_DOUBLE);
                float phi = CRandom::RandF(0.0f, D3DX_PI);

                p.Velocity =
                {
                    sinf(phi) *
                    cosf(theta) *
                    speed,

                    cosf(phi) *
                    speed,

                    sinf(phi) *
                    sinf(theta) *
                    speed,
                };
            }
        }
        break;
        }

        //---------------------------------
        // その他
        //---------------------------------

        p.Size = CRandom::RandF(SizeMin, SizeMax);
        p.LifeTime = CRandom::RandF(LifeTimeMin, LifeTimeMax);
        p.Color = col;
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

        //---------------------------------
        // タイプ別更新
        //---------------------------------

        switch (Type)
        {
        case ParticleType::Fire:
            UpdateFire(p);
            break;

        case ParticleType::Smoke:
            UpdateSmoke(p);
            break;

        case ParticleType::Spark:
            UpdateSpark(p);
            break;

        case ParticleType::Snow:
            UpdateSnow(p);
            break;

        default:
            UpdateDefault(p);
            break;
        }
    }
}

//=============================================================================
// デフォルト更新
//=============================================================================
void CParticleEmitterComponent::UpdateDefault(PARTICLE& p)
{
    p.Position += p.Velocity;

    float t = (float)p.Age / p.LifeTime;

    p.Color.a = 1.0f - t;
}

//=============================================================================
// 炎パーティクル
//=============================================================================
void CParticleEmitterComponent::UpdateFire(PARTICLE& p)
{
    //---------------------------------
     // 移動
     //---------------------------------

    p.Position += p.Velocity;

    //---------------------------------
    // 揺らぎ
    //---------------------------------

    p.Velocity.x += CRandom::RandF(-0.02f, 0.02f);

    p.Velocity.z += CRandom::RandF(-0.02f, 0.02f);

    //---------------------------------
    // 徐々に減速
    //---------------------------------

    p.Velocity *= 0.98f;

    //---------------------------------
    // サイズ縮小
    //---------------------------------

    p.Size *= 0.96f;

    //---------------------------------
    // 色変化
    //---------------------------------

    float t = (float)p.Age / p.LifeTime;

    p.Color.r = 1.0f;
    p.Color.g = 1.0f - t;
    p.Color.b = 0.1f;
    p.Color.a = 1.0f - t;
}

//=============================================================================
// 煙パーティクル
//=============================================================================
void CParticleEmitterComponent::UpdateSmoke(PARTICLE& p)
{
    p.Position += p.Velocity;

    //---------------------------------
    // ゆっくり上昇
    //---------------------------------

    p.Velocity.y += 0.003f;

    //---------------------------------
    // 空気抵抗
    //---------------------------------

    p.Velocity *= 0.98f;

    //---------------------------------
    // 広がる
    //---------------------------------

    p.Size *= 1.01f;

    //---------------------------------
    // フェード
    //---------------------------------

    float t = (float)p.Age / p.LifeTime;

    p.Color.a = 0.5f * (1.0f - t);
}

//=============================================================================
// スパークエフェクト
//=============================================================================
void CParticleEmitterComponent::UpdateSpark(PARTICLE& p)
{
    p.Position += p.Velocity;

    //---------------------------------
    // 重力
    //---------------------------------

    p.Velocity.y -= 0.05f;

    //---------------------------------
    // フェード
    //---------------------------------

    float t = (float)p.Age / p.LifeTime;

    p.Color.a = 1.0f - t;
}

//=============================================================================
// 雪エフェクト
//=============================================================================
void CParticleEmitterComponent::UpdateSnow(PARTICLE& p)
{
    //---------------------------------
    // 移動
    //---------------------------------

    p.Position += p.Velocity;

    //---------------------------------
    // ふわふわ横揺れ
    //---------------------------------

    p.Velocity.x += CRandom::RandF(-0.01f, 0.01f);

    p.Velocity.z += CRandom::RandF(-0.01f, 0.01f);

    //---------------------------------
    // 落下速度制限
    //---------------------------------

    if (p.Velocity.y < -1.5f)
    {
        p.Velocity.y = -1.5f;
    }

    //---------------------------------
    // フェード
    //---------------------------------

    float t = (float)p.Age / p.LifeTime;

    p.Color.a = 1.0f - t;
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
