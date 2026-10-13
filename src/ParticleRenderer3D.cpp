//=============================================================================
//
// パーティクルレンダラー3D処理 [ParticleRendeerr3D.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "ParticleRenderer3D.h"
#include "Manager.h"
#include "Renderer.h"
#include <cassert>


//=============================================================================
// コンストラクタ
//=============================================================================
CParticleRenderer3D::CParticleRenderer3D()
{

}

//=============================================================================
// デストラクタ
//=============================================================================
CParticleRenderer3D::~CParticleRenderer3D()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CParticleRenderer3D::Init(void)
{
    auto device = CManager::Instance()->GetRenderer()->GetDevice();

    //---------------------------------
    // 頂点バッファ
    //---------------------------------
    device->CreateVertexBuffer(
        sizeof(QUAD_VERTEX) * 4,
        0,
        0,
        D3DPOOL_MANAGED,
        &m_pQuadVB,
        nullptr);

    QUAD_VERTEX* vtx;

    // 頂点バッファをロックし、頂点情報へのポインタを取得
    m_pQuadVB->Lock(0, 0, (void**)&vtx, 0);

    vtx[0].Pos = D3DXVECTOR3(-0.5f, 0.5f, 0);
    vtx[1].Pos = D3DXVECTOR3(0.5f, 0.5f, 0);
    vtx[2].Pos = D3DXVECTOR3(-0.5f, -0.5f, 0);
    vtx[3].Pos = D3DXVECTOR3(0.5f, -0.5f, 0);

    vtx[0].Tex = D3DXVECTOR2(0, 0);
    vtx[1].Tex = D3DXVECTOR2(1, 0);
    vtx[2].Tex = D3DXVECTOR2(0, 1);
    vtx[3].Tex = D3DXVECTOR2(1, 1);

    // 頂点バッファをアンロックする
    m_pQuadVB->Unlock();

    //---------------------------------
    // インデックスバッファ
    //---------------------------------
    device->CreateIndexBuffer(
        sizeof(WORD) * 6,
        0,
        D3DFMT_INDEX16,
        D3DPOOL_MANAGED,
        &m_pIdxBuff,
        nullptr);

    WORD* pIdx;

    // インデックスバッファをロックし、インデックス情報へのポインタを取得
    m_pIdxBuff->Lock(0, 0, (void**)&pIdx, 0);

    pIdx[0] = 0;
    pIdx[1] = 2;
    pIdx[2] = 1;

    pIdx[3] = 2;
    pIdx[4] = 3;
    pIdx[5] = 1;

    // インデックスバッファをアンロックする
    m_pIdxBuff->Unlock();

    // InstanceVB
    device->CreateVertexBuffer(
        sizeof(INSTANCE_DATA) * CParticleEmitterComponent::MAX_PARTICLES,
        D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
        0,
        D3DPOOL_DEFAULT,
        &m_pInstanceVB,
        nullptr);

    D3DVERTEXELEMENT9 decl[] =
    {
        //----------------------------------
        // Stream0 Quad
        //----------------------------------

        { 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0,12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },

        //----------------------------------
        // Stream1 Instance
        //----------------------------------

        { 1, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
        { 1, 12, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2 },
        { 1, 16, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },

        D3DDECL_END()
    };

    device->CreateVertexDeclaration(decl, &m_pVertexDecl);

	return S_OK;
}

//=============================================================================
// 描画処理
//=============================================================================
void CParticleRenderer3D::Draw(const std::vector<PARTICLE>& particles,  LPDIRECT3DTEXTURE9 texture, ID3DXEffect* effect)
{
    auto device = CManager::Instance()->GetRenderer()->GetDevice();

    //---------------------------------
    // ビューマトリックス取得
    //---------------------------------

    D3DXMATRIX view, proj;

    device->GetTransform(D3DTS_VIEW, &view);
    device->GetTransform(D3DTS_PROJECTION, &proj);

    D3DXMATRIX world;
    D3DXMatrixIdentity(&world);

    device->SetTransform(D3DTS_WORLD, &world);

    //---------------------------------
    // 描画設定
    //---------------------------------

    device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    // ライトを無効にする
    device->SetRenderState(D3DRS_LIGHTING, FALSE);

    INSTANCE_DATA* inst = nullptr;

    // 頂点バッファをロックし、頂点情報へのポインタを取得
    m_pInstanceVB->Lock(0, 0, (void**)&inst, D3DLOCK_DISCARD);

    int particleCount = 0;

    //---------------------------------
    // パーティクル生成
    //---------------------------------

    for (const auto& p : particles)
    {
        if (!p.Active) continue;

        if (particleCount >= CParticleEmitterComponent::MAX_PARTICLES)
            break;

        inst[particleCount].Position = p.Position;
        inst[particleCount].Size = p.Size;
        inst[particleCount].Color = p.Color;

        particleCount++;
    }

    //頂点バッファをアンロックする
    m_pInstanceVB->Unlock();

    //---------------------------------
    // 描画設定
    //---------------------------------

    // テクスチャの設定
    device->SetTexture(0, texture);

    device->SetVertexDeclaration(m_pVertexDecl);
    
    // 頂点バッファをデータストリームに設定
    device->SetStreamSource(0, m_pQuadVB.Get(), 0, sizeof(QUAD_VERTEX));

    // 頂点バッファをデータストリームに設定
    device->SetStreamSource(1, m_pInstanceVB.Get(), 0, sizeof(INSTANCE_DATA));

    device->SetStreamSourceFreq(0, D3DSTREAMSOURCE_INDEXEDDATA | particleCount);

    device->SetStreamSourceFreq(1, static_cast<UINT>(D3DSTREAMSOURCE_INSTANCEDATA | 1));

    // インデックスバッファの設定
    device->SetIndices(m_pIdxBuff.Get());

    //---------------------------------
    // 描画
    //---------------------------------

    if (effect)
    {
        effect->SetMatrix("View", &view);
        effect->SetMatrix("Projection", &proj);
        effect->SetTexture("Texture0", texture);

        UINT passCount = 0;

        effect->Begin(&passCount, 0);

        for (UINT i = 0; i < passCount; i++)
        {
            effect->BeginPass(i);

            // ポリゴンの描画
            device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

            effect->EndPass();
        }

        effect->End();
    }
    else
    {
        // ポリゴンの描画
        device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);
    }

    device->SetRenderState(D3DRS_LIGHTING, TRUE);          // ライトを有効にする

    device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    device->SetStreamSourceFreq(0, 1);
    device->SetStreamSourceFreq(1, 1);
    device->SetVertexDeclaration(nullptr);
}

//=============================================================================
// デバイスロスト時
//=============================================================================
void CParticleRenderer3D::OnLostDevice(void)
{
    auto device = CManager::Instance()->GetRenderer()->GetDevice();

    device->SetStreamSourceFreq(0, 1);
    device->SetStreamSourceFreq(1, 1);

    device->SetVertexDeclaration(nullptr);

    device->SetStreamSource(0, nullptr, 0, 0);
    device->SetStreamSource(1, nullptr, 0, 0);

    device->SetIndices(nullptr);

    m_pInstanceVB.Reset();

    if (m_pVertexDecl)
    {
        m_pVertexDecl->Release();
        m_pVertexDecl = nullptr;
    }
}

//=============================================================================
// デバイスリセット時
//=============================================================================
void CParticleRenderer3D::OnResetDevice(void)
{
    // 初期化処理
    Init();
}