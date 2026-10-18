//=============================================================================
//
// モデルインスタンシングレンダラー処理 [ModelInstancingRenderer.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _MODEL_INSTANCING_RENDER_H_// このマクロ定義がされていなかったら
#define _MODEL_INSTANCING_RENDER_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "RenderComponent.h"


// 前方宣言
class CXModelResource;
class CShaderResource;


//*****************************************************************************
// モデルインスタンシングレンダラークラス
//*****************************************************************************
class CModelInstancingRenderer
{
public:

    HRESULT Init(void);

    void Begin(void);

    void AddInstance(
        CXModelResource* model, 
        const D3DXMATRIX& world, 
        RenderLayer layer,
        const std::unordered_map<int, std::shared_ptr<CTextureResource>>& faceTextures = {});

    void EndMain    (void);
    void FlushMain  (void);
    void EndShadow  (void);

    // ビュー・プロジェクションの設定(シャドウでライトのこれが必要なため)
    void SetViewProjection(const D3DXMATRIX& view, const D3DXMATRIX& projection);

    // デバイスリセット時
    void OnLostDevice(void);
    void OnResetDevice(void);

private:

    void Render         (ID3DXEffect* effect, bool useMaterial);
    void RenderShadow   (ID3DXEffect* effect);
    void RenderGizmo    (ID3DXEffect* effect);

    struct INSTANCE_DATA
    {
        D3DXVECTOR4 World0;
        D3DXVECTOR4 World1;
        D3DXVECTOR4 World2;
        D3DXVECTOR4 World3;
    };

    struct INSTANCE_GROUP
    {
        CXModelResource* Model;
        std::vector<INSTANCE_DATA> Instances;

        // 面番号と上書きテクスチャ
        std::unordered_map<int, std::shared_ptr<CTextureResource>> FaceTextures;
    };

private:

    static constexpr int MAX_DATA = 10000;

    ComPtr<IDirect3DVertexBuffer9> m_InstanceVB;
    ComPtr<IDirect3DVertexDeclaration9> m_VertexDecl;

    // 通常描画用のインスタンスグループ
    std::vector<INSTANCE_GROUP> m_Groups;

    // ギズモ描画用
    std::unordered_map<CXModelResource*, std::vector<INSTANCE_DATA>> m_GizmoGroups;
    std::shared_ptr<CShaderResource> m_pShader;
    std::shared_ptr<CShaderResource> m_pShadowShader;

    D3DXMATRIX m_View;
    D3DXMATRIX m_Projection;
};

#endif