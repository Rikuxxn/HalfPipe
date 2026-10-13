//=============================================================================
//
// レンダーコンポーネント処理 [RenderComponent.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _RENDER_COMPONENT_H_// このマクロ定義がされていなかったら
#define _RENDER_COMPONENT_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Component.h"

class CTextureResource;

// 描画順
enum class RenderLayer
{
    Background = 0,
    Shadow,
    World,
    Player,
    Model,
    Gizmo,
    Mesh,
    Effect,
    Particle3D,
    Particle2D,
    UI,
    Pause,
    Fade,
};

//*****************************************************************************
// レンダーコンポーネントクラス
//*****************************************************************************
class CRenderComponent : public CComponent
{
protected:

    std::shared_ptr<CTextureResource> m_TextureResource;

    RenderLayer m_Layer;

public:

    CRenderComponent()
    {
        m_Layer = RenderLayer::World;
    }

    virtual void Draw(void) override = 0;

    virtual bool IsShadowCaster(void)
    {
        return false;
    }

    virtual void DrawShadow(void)
    {
    }

    // リソース直接指定
    void SetTexture(std::shared_ptr<CTextureResource> texture);

    // パス指定
    void SetTexture(const std::string& path);

    // テクスチャ取得
    std::shared_ptr<CTextureResource>GetTexture(void) const { return m_TextureResource; }

    // 描画順の設定と取得
    void        SetLayer(RenderLayer layer) { m_Layer = layer; }
    RenderLayer GetLayer(void) const        { return m_Layer; }
};

#endif