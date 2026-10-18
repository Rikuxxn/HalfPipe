//=============================================================================
//
// モデルインスタンシングレンダラー処理 [ModelInstancingRenderer.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "ModelInstancingRenderer.h"
#include "XModelResource.h"
#include "Manager.h"
#include "Renderer.h"
#include "ShaderManager.h"
#include "ShadowMap.h"
#include "TextureResource.h"


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CModelInstancingRenderer::Init(void)
{
    // デバイスの取得
    auto device = CManager::Instance()->GetRenderer()->GetDevice();

    HRESULT hr = device->CreateVertexBuffer(
        sizeof(INSTANCE_DATA) * MAX_DATA,
        D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
        0,
        D3DPOOL_DEFAULT,
        &m_InstanceVB,
        nullptr);

    if (FAILED(hr))
    {
        return E_FAIL;
    }

    //----------------------------------
    // VertexDeclaration
    //----------------------------------

    D3DVERTEXELEMENT9 decl[] =
    {
        //-------------------------
        // Stream0 (メッシュ)
        //-------------------------

        { 0,  0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION,0 },
        { 0, 12,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,0 },
        { 0, 24,  D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,0 },

        //-------------------------
        // Stream1 インスタンス
        //-------------------------

        { 1, 0,  D3DDECLTYPE_FLOAT4,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_TEXCOORD,1 },
        { 1, 16, D3DDECLTYPE_FLOAT4,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_TEXCOORD,2 },
        { 1, 32, D3DDECLTYPE_FLOAT4,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_TEXCOORD,3 },
        { 1, 48, D3DDECLTYPE_FLOAT4,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_TEXCOORD,4 },

        D3DDECL_END()
    };

    hr = device->CreateVertexDeclaration(decl, &m_VertexDecl);

    if (FAILED(hr))
    {
        return E_FAIL;
    }

    // シェーダーマネージャーの取得
    auto shaderMgr = CManager::Instance()->GetShaderManager();

    // シェーダーの読み込み : インスタンシング
    m_pShader = shaderMgr->Load("data/Shader/Instancing/ModelInstancing.fx");

    // シェーダーの読み込み : シャドウマップ
    m_pShadowShader = shaderMgr->Load("data/Shader/Shadow/ShadowMap.hlsl");

    return S_OK;
}

//=============================================================================
// 開始
//=============================================================================
void CModelInstancingRenderer::Begin(void)
{
    m_Groups.clear();
    m_GizmoGroups.clear();
}

//=============================================================================
// インスタンス追加
//=============================================================================
void CModelInstancingRenderer::AddInstance(
    CXModelResource* model, 
    const D3DXMATRIX& world, 
    RenderLayer layer,
    const std::unordered_map<int, std::shared_ptr<CTextureResource>>& faceTextures)
{
    if (!model)
    {
        return;
    }

    INSTANCE_DATA data;
    memcpy(&data, &world, sizeof(D3DXMATRIX));

    // ギズモ
    if (layer == RenderLayer::Gizmo)
    {
        m_GizmoGroups[model].push_back(data);
        return;
    }

    // 同じモデルかつ同じテクスチャ設定のグループを探す
    for (auto& group : m_Groups)
    {
        if (group.Model != model)
        {
            continue;
        }

        if (group.FaceTextures.size() != faceTextures.size())
        {
            continue;
        }

        bool sameTextures = true;

        for (const auto& [faceIndex, texture] : faceTextures)
        {
            auto it = group.FaceTextures.find(faceIndex);

            if (it == group.FaceTextures.end() ||
                it->second.get() != texture.get())
            {
                sameTextures = false;
                break;
            }
        }

        if (sameTextures)
        {
            group.Instances.push_back(data);
            return;
        }
    }

    // 一致するグループがなければ新しく作る
    INSTANCE_GROUP group;
    group.Model = model;
    group.Instances.push_back(data);
    group.FaceTextures = faceTextures;

    m_Groups.push_back(std::move(group));
}

//=============================================================================
// 描画(通常)
//=============================================================================
void CModelInstancingRenderer::Render(ID3DXEffect* effect, bool useMaterial)
{
    // デバイス取得
    auto device = CManager::Instance()->GetRenderer()->GetDevice();

    //------------------------------------------
    // View Projection
    //------------------------------------------

    // 常に通常ViewProjectionを設定
    effect->SetMatrix("View", &m_View);
    effect->SetMatrix("Projection", &m_Projection);

    // シャドウマップの取得
    auto shadow = CManager::Instance()->GetRenderer()->GetShadowMap();

    // シャドウテクスチャ設定
    effect->SetTexture("ShadowTexture", shadow->GetShadowTexture());

    // ライトビュー設定
    effect->SetMatrix("LightView", &shadow->GetViewMatrix());
    
    // ライトプロジェクション設定
    effect->SetMatrix("LightProjection", &shadow->GetProjectionMatrix());

    // ライト方向設定
    effect->SetValue("LightDir", &shadow->GetLightDirection(), sizeof(D3DXVECTOR3));

    //-----------------------------------------
    // VertexDeclaration
    //-----------------------------------------

    device->SetVertexDeclaration(m_VertexDecl.Get());

    //-----------------------------------------
    // モデル毎
    //-----------------------------------------

    for (auto& group : m_Groups)
    {
        CXModelResource* model = group.Model;
        auto& instances = group.Instances;

        // 空の場合
        if (instances.empty())
        {
            continue;
        }

        //-----------------------------------------
        // InstanceVB更新
        //-----------------------------------------

        INSTANCE_DATA* pData = nullptr;

        const UINT dataSize = static_cast<UINT>(sizeof(INSTANCE_DATA) * instances.size());

        m_InstanceVB->Lock(
            0,
            dataSize,
            (void**)&pData,
            D3DLOCK_DISCARD);

        memcpy(pData, instances.data(), sizeof(INSTANCE_DATA) * instances.size());

        m_InstanceVB->Unlock();

        //-----------------------------------------
        // 頂点バッファをデータストリームに設定
        //-----------------------------------------

        device->SetStreamSource(0, model->VertexBuffer.Get(), 0, model->VertexStride);
        device->SetStreamSource(1, m_InstanceVB.Get(), 0, sizeof(INSTANCE_DATA));

        //-----------------------------------------
        // Frequency
        //-----------------------------------------

        device->SetStreamSourceFreq(0, static_cast<UINT>(D3DSTREAMSOURCE_INDEXEDDATA | instances.size()));
        device->SetStreamSourceFreq(1, static_cast<UINT>(D3DSTREAMSOURCE_INSTANCEDATA | 1));

        // インデックスバッファの設定
        device->SetIndices(model->IndexBuffer.Get());

        //------------------------------------------
        // Draw
        //------------------------------------------

        UINT passCount;

        // シェーダーの適用に失敗した場合
        if (FAILED(effect->Begin(&passCount, 0)))
        {
            continue;
        }

        for (UINT pass = 0; pass < passCount; pass++)
        {
            effect->BeginPass(pass);

            for (UINT i = 0; i < model->NumSubsets; i++)
            {
                auto& subset = model->Subsets[i];

                if (useMaterial)
                {
                    UINT mat = subset.MaterialIndex;

                    // デフォルトではモデル本来のテクスチャを使用
                    IDirect3DBaseTexture9* texture = nullptr;

                    if (mat < model->Textures.size())
                    {
                        texture = model->Textures[mat].Get();
                    }

                    // 面ごとの上書きテクスチャを検索
                    auto it = group.FaceTextures.find(i);

                    if (it != group.FaceTextures.end() && it->second)
                    {
                        // 上書きテクスチャを設定
                        texture = it->second->Texture.Get();
                    }

                    // テクスチャを設定
                    effect->SetTexture("Texture0", texture);

                    effect->SetBool("HasTexture", texture != nullptr);

                    if (mat < model->Materials.size())
                    {
                        auto& material = model->Materials[mat];

                        D3DXVECTOR4 color(
                            material.Diffuse.r,
                            material.Diffuse.g,
                            material.Diffuse.b,
                            material.Diffuse.a);

                        effect->SetVector("MatColor", &color);
                    }
                }

                effect->CommitChanges();

                device->DrawIndexedPrimitive(
                    D3DPT_TRIANGLELIST,
                    0,
                    subset.VertexStart,
                    subset.VertexCount,
                    subset.FaceStart * 3,
                    subset.FaceCount);
            }

            effect->EndPass();
        }

        effect->End();
    }

    // Effect解除
    device->SetVertexShader(nullptr);
    device->SetPixelShader(nullptr);
    device->SetVertexDeclaration(nullptr);

    // ストリーム解除
    device->SetStreamSource(0, nullptr, 0, 0);
    device->SetStreamSource(1, nullptr, 0, 0);
    device->SetIndices(nullptr);

    // テクスチャ解除
    device->SetTexture(0, nullptr);

    // Frequencyを元に戻す
    device->SetStreamSourceFreq(0, 1);
    device->SetStreamSourceFreq(1, 1);
}

//=============================================================================
// 描画(シャドウ)
//=============================================================================
void CModelInstancingRenderer::RenderShadow(ID3DXEffect* effect)
{
    // デバイスの取得
    auto device = CManager::Instance()->GetRenderer()->GetDevice();

    //------------------------------------------
    // Shadow用行列
    //------------------------------------------

    // シャドウ取得
    auto shadow = CManager::Instance()->GetRenderer()->GetShadowMap();

    D3DXMATRIX view = shadow->GetViewMatrix();
    D3DXMATRIX proj = shadow->GetProjectionMatrix();

    effect->SetMatrix("View", &view);
    effect->SetMatrix("Projection", &proj);

    //-----------------------------------------
    // VertexDeclaration
    //-----------------------------------------

    device->SetVertexDeclaration(m_VertexDecl.Get());

    //-----------------------------------------
    // モデル毎
    //-----------------------------------------

    for (auto& group : m_Groups)
    {
        CXModelResource* model = group.Model;
        auto& instances = group.Instances;

        // 空の場合
        if (instances.empty())
        {
            continue;
        }

        //-----------------------------------------
        // InstanceVB更新
        //-----------------------------------------

        INSTANCE_DATA* pData = nullptr;

        const UINT dataSize = static_cast<UINT>(sizeof(INSTANCE_DATA) * instances.size());

        m_InstanceVB->Lock(
            0,
            dataSize,
            (void**)&pData,
            D3DLOCK_DISCARD);

        memcpy(pData, instances.data(), sizeof(INSTANCE_DATA) * instances.size());

        m_InstanceVB->Unlock();

        //-----------------------------------------
        // 頂点バッファをデータストリームに設定
        //-----------------------------------------

        device->SetStreamSource(0, model->VertexBuffer.Get(), 0, model->VertexStride);
        device->SetStreamSource(1, m_InstanceVB.Get(), 0, sizeof(INSTANCE_DATA));

        //-----------------------------------------
        // Frequency
        //-----------------------------------------

        device->SetStreamSourceFreq(0, static_cast<UINT>(D3DSTREAMSOURCE_INDEXEDDATA | instances.size()));
        device->SetStreamSourceFreq(1, static_cast<UINT>(D3DSTREAMSOURCE_INSTANCEDATA | 1));

        // インデックスバッファの設定
        device->SetIndices(model->IndexBuffer.Get());

        //------------------------------------------
        // Draw
        //------------------------------------------

        UINT passCount;

        // シェーダーの適用に失敗した場合
        if (FAILED(effect->Begin(&passCount, 0)))
        {
            continue;
        }

        for (UINT pass = 0; pass < passCount; pass++)
        {
            effect->BeginPass(pass);

            for (UINT i = 0; i < model->NumSubsets; i++)
            {
                auto& subset = model->Subsets[i];

                effect->CommitChanges();

                device->DrawIndexedPrimitive(
                    D3DPT_TRIANGLELIST,
                    0,
                    subset.VertexStart,
                    subset.VertexCount,
                    subset.FaceStart * 3,
                    subset.FaceCount);
            }

            effect->EndPass();
        }

        effect->End();
    }

    // Effect解除
    device->SetVertexShader(nullptr);
    device->SetPixelShader(nullptr);
    device->SetVertexDeclaration(nullptr);

    // ストリーム解除
    device->SetStreamSource(0, nullptr, 0, 0);
    device->SetStreamSource(1, nullptr, 0, 0);
    device->SetIndices(nullptr);

    device->SetTexture(0, nullptr);

    // Frequencyを元に戻す
    device->SetStreamSourceFreq(0, 1);
    device->SetStreamSourceFreq(1, 1);
}

//=============================================================================
// ギズモ用描画
//=============================================================================
void CModelInstancingRenderer::RenderGizmo(ID3DXEffect* effect)
{
    // デバイス取得
    auto device = CManager::Instance()->GetRenderer()->GetDevice();

    // ギズモは常に手前に表示
    device->SetRenderState(D3DRS_ZENABLE, FALSE);
    device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

    //------------------------------------------
    // View Projection
    //------------------------------------------

    // 常に通常ViewProjectionを設定
    effect->SetMatrix("View", &m_View);
    effect->SetMatrix("Projection", &m_Projection);

    //// シャドウマップの取得
    //auto shadow = CManager::Instance()->GetRenderer()->GetShadowMap();

    //// シャドウテクスチャ設定
    //effect->SetTexture("ShadowTexture", shadow->GetShadowTexture());

    //// ライトビュー設定
    //effect->SetMatrix("LightView", &shadow->GetViewMatrix());

    //// ライトプロジェクション設定
    //effect->SetMatrix("LightProjection", &shadow->GetProjectionMatrix());

    //// ライト方向設定
    //effect->SetValue("LightDir", &shadow->GetLightDirection(), sizeof(D3DXVECTOR3));

    //-----------------------------------------
    // VertexDeclaration
    //-----------------------------------------

    device->SetVertexDeclaration(m_VertexDecl.Get());

    //-----------------------------------------
    // モデル毎
    //-----------------------------------------

    for (auto& group : m_GizmoGroups)
    {
        CXModelResource* model = group.first;
        auto& instances = group.second;

        // 空の場合
        if (instances.empty())
        {
            continue;
        }

        //-----------------------------------------
        // InstanceVB更新
        //-----------------------------------------

        INSTANCE_DATA* pData = nullptr;

        const UINT dataSize = static_cast<UINT>(sizeof(INSTANCE_DATA) * instances.size());

        m_InstanceVB->Lock(
            0,
            dataSize,
            (void**)&pData,
            D3DLOCK_DISCARD);

        memcpy(pData, instances.data(), sizeof(INSTANCE_DATA) * instances.size());

        m_InstanceVB->Unlock();

        //-----------------------------------------
        // 頂点バッファをデータストリームに設定
        //-----------------------------------------

        device->SetStreamSource(0, model->VertexBuffer.Get(), 0, model->VertexStride);
        device->SetStreamSource(1, m_InstanceVB.Get(), 0, sizeof(INSTANCE_DATA));

        //-----------------------------------------
        // Frequency
        //-----------------------------------------

        device->SetStreamSourceFreq(0, static_cast<UINT>(D3DSTREAMSOURCE_INDEXEDDATA | instances.size()));
        device->SetStreamSourceFreq(1, static_cast<UINT>(D3DSTREAMSOURCE_INSTANCEDATA | 1));

        // インデックスバッファの設定
        device->SetIndices(model->IndexBuffer.Get());

        //------------------------------------------
        // Draw
        //------------------------------------------

        UINT passCount;

        // シェーダーの適用に失敗した場合
        if (FAILED(effect->Begin(&passCount, 0)))
        {
            continue;
        }

        for (UINT pass = 0; pass < passCount; pass++)
        {
            effect->BeginPass(pass);

            for (UINT i = 0; i < model->NumSubsets; i++)
            {
                auto& subset = model->Subsets[i];

                //if (useMaterial)
                {
                    UINT mat = subset.MaterialIndex;

                    if (mat < model->Textures.size())
                    {
                        effect->SetTexture("Texture0", model->Textures[mat].Get());
                    }
                    else
                    {
                        effect->SetTexture("Texture0", nullptr);
                    }

                    bool hasTexture =
                        mat < model->Textures.size() &&
                        model->Textures[mat] != nullptr;

                    effect->SetBool("HasTexture", hasTexture);

                    if (mat < model->Materials.size())
                    {
                        auto& material = model->Materials[mat];

                        D3DXVECTOR4 color(
                            material.Diffuse.r,
                            material.Diffuse.g,
                            material.Diffuse.b,
                            material.Diffuse.a);

                        effect->SetVector("MatColor", &color);
                    }
                }

                effect->CommitChanges();

                device->DrawIndexedPrimitive(
                    D3DPT_TRIANGLELIST,
                    0,
                    subset.VertexStart,
                    subset.VertexCount,
                    subset.FaceStart * 3,
                    subset.FaceCount);
            }

            effect->EndPass();
        }

        effect->End();
    }

    // Effect解除
    device->SetVertexShader(nullptr);
    device->SetPixelShader(nullptr);
    device->SetVertexDeclaration(nullptr);

    // ストリーム解除
    device->SetStreamSource(0, nullptr, 0, 0);
    device->SetStreamSource(1, nullptr, 0, 0);
    device->SetIndices(nullptr);

    // テクスチャ解除
    device->SetTexture(0, nullptr);

    // Frequencyを元に戻す
    device->SetStreamSourceFreq(0, 1);
    device->SetStreamSourceFreq(1, 1);

    device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    device->SetRenderState(D3DRS_ZENABLE, TRUE);
}

//=============================================================================
// メイン(通常)描画終了
//=============================================================================
void CModelInstancingRenderer::EndMain(void)
{
    Render(m_pShader->GetEffect(), true);

    RenderGizmo(m_pShader->GetEffect()/*, true*/);
}

//=============================================================================
// メイン(通常)描画終了
//=============================================================================
void CModelInstancingRenderer::FlushMain(void)
{
    if (m_Groups.empty())
    {
        return;
    }

    Render(m_pShader->GetEffect(), true);

    m_Groups.clear();
}

//=============================================================================
// シャドウ描画終了
//=============================================================================
void CModelInstancingRenderer::EndShadow(void)
{
    RenderShadow(m_pShadowShader->GetEffect());
}

//=============================================================================
// ビュー・プロジェクションの設定(シャドウでライトのこれが必要なため)
//=============================================================================
void CModelInstancingRenderer::SetViewProjection(
    const D3DXMATRIX& view,
    const D3DXMATRIX& projection)
{
    m_View = view;
    m_Projection = projection;
}

//=============================================================================
// デバイスロスト
//=============================================================================
void CModelInstancingRenderer::OnLostDevice()
{
    auto device = CManager::Instance()->GetRenderer()->GetDevice();

    device->SetStreamSourceFreq(0, 1);
    device->SetStreamSourceFreq(1, 1);

    device->SetVertexDeclaration(nullptr);

    device->SetStreamSource(0, nullptr, 0, 0);
    device->SetStreamSource(1, nullptr, 0, 0);

    device->SetIndices(nullptr);

    m_InstanceVB.Reset();

    m_VertexDecl.Reset();

    m_Groups.clear();
}

//=============================================================================
// デバイスリセット
//=============================================================================
void CModelInstancingRenderer::OnResetDevice()
{
    Init();
}