//=============================================================================
//
// パーティクルエミッターコンポーネント処理 [ParticleEmitterComponent.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _PARTICLE_EMITTER_COMPONENT_H_// このマクロ定義がされていなかったら
#define _PARTICLE_EMITTER_COMPONENT_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "RenderComponent.h"
#include "ParticleModule.h"


// 前方宣言
class CTextureResource;
class CParticleSystem;
class CShaderResource;


// 発生モード
enum class ParticleEmitMode
{
    Burst,     // 一回だけ
    Loop,      // 常時
    OneShot    // Emit後自動停止
};

// 生成範囲
enum class ParticleSpawnShape
{
    Point,
    Box,
    Circle,
    Sphere
};

// 放出方向
enum class ParticleSpace
{
    Space2D,
    Space3D,
};

//*****************************************************************************
// パーティクルエミッターコンポーネントクラス
//*****************************************************************************
class CParticleEmitterComponent : public CRenderComponent
{
public:
    CParticleEmitterComponent();
    ~CParticleEmitterComponent();

    // パーティクル最大数
    static constexpr int MAX_PARTICLES = 10000;

    // モジュール追加テンプレート関数
    template<class T>
    T* AddModule(void)
    {
        auto module = std::make_unique<T>();

        T* ptr = module.get();

        m_Modules.push_back(std::move(module));

        return ptr;
    }

    // モジュール取得テンプレート関数
    template<class T>
    T* GetModule(void)
    {
        for (auto& module : m_Modules)
        {
            if (auto ptr = dynamic_cast<T*>(module.get()))
            {
                return ptr;
            }
        }

        return nullptr;
    }

    HRESULT Init(void) override;
    void Update(void) override;
    void Draw(void) override;

    void SetTexture(const std::string& path);

    void SetPlay(bool flag) { m_IsPlay = flag; }

    // テクスチャ取得
    LPDIRECT3DTEXTURE9 GetTexture(void) const;

    // パーティクル取得
    const std::vector<PARTICLE>& GetParticles(void) const { return m_Particles; }

    // シェーダーの設定
    void SetShader(const std::string& path);

    // シェーダーの取得
    ID3DXEffect* GetShader(void) const;

private:

    void UpdateParticles(void);

    void Emit(int count);

    // ランダム位置生成
    D3DXVECTOR3 SpawnPosition(void);

public:

    //---------------------------------
    // 設定
    //---------------------------------

    ParticleEmitMode EmitMode = ParticleEmitMode::Loop;
    ParticleSpawnShape SpawnShape = ParticleSpawnShape::Point;
    ParticleSpace Space = ParticleSpace::Space2D;

    float EmitRate = 5.0f;

    float LifeTimeMin = 30;
    float LifeTimeMax = 60;

    float SpeedMin = 1;
    float SpeedMax = 5;

    float SizeMin = 16;
    float SizeMax = 64;

    //---------------------------------
    // 範囲
    //---------------------------------

    D3DXVECTOR3 BoxSize = { 100,100,100 };

    float Radius = 100.0f;

    D3DXCOLOR col = { 1.0f, 1.0f, 1.0f, 1.0f };

private:

    float m_EmitTimer = 0;
    int m_NextParticle = 0;

    CParticleSystem* m_pParticleSystem = nullptr;

    std::vector<PARTICLE> m_Particles;

    std::shared_ptr<CTextureResource> m_TextureResource;

    // シェーダーリソース
    std::shared_ptr<CShaderResource> m_Shader;

    // パーティクルを再生するかどうか
    bool m_IsPlay;

    // モジュール一覧
    std::vector<std::unique_ptr<IParticleModule>> m_Modules;
};

#endif