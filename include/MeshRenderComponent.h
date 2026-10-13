//=============================================================================
//
// メッシュレンダーコンポーネント処理 [MeshRenderComponent.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _MESH_RENDER_COMPONENT_H_// このマクロ定義がされていなかったら
#define _MESH_RENDER_COMPONENT_H_// 2重インクルード防止のマクロ定義


//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "RenderComponent.h"


// 前方宣言
class CMeshBase;
class CShaderResource;


//*****************************************************************************
// メッシュコンポーネントクラス
//*****************************************************************************
class CMeshRenderComponent : public CRenderComponent
{
public:

    std::shared_ptr<CMeshBase> m_Mesh;

    void SetMesh(std::shared_ptr<CMeshBase> mesh)
    {
        m_Mesh = std::move(mesh);
    }

    void SetShader(std::shared_ptr<CShaderResource> shader)
    {
        m_Shader = shader;
    }

    void SetTexture(const std::string& path);

public:

    virtual void Draw(void) override;

    // 影描画用
    void DrawShadow(void) override
    {
        Draw();
    }

    // 影を描画するか
    bool IsShadowCaster(void) override
    {
        return true;
    }

private:
    std::shared_ptr<CShaderResource> m_Shader;
};

#endif
