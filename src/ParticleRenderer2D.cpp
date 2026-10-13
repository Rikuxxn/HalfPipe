//=============================================================================
//
// パーティクルレンダー2D処理 [ParticleRender2D.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================
#include "ParticleRenderer2D.h"
#include "Manager.h"
#include "Renderer.h"


//=============================================================================
// コンストラクタ
//=============================================================================
CParticleRenderer2D::CParticleRenderer2D()
{
    m_pVtxBuff = nullptr;
    m_pIdxBuff = nullptr;
}

//=============================================================================
// デストラクタ
//=============================================================================
CParticleRenderer2D::~CParticleRenderer2D()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CParticleRenderer2D::Init(void)
{
    auto device = CManager::Instance()->GetRenderer()->GetDevice();

    //---------------------------------
    // 頂点バッファ
    //---------------------------------
    device->CreateVertexBuffer(
        sizeof(VERTEX_2D) * 4 * MAX_PARTICLES,
        D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
        FVF_VERTEX_2D,
        D3DPOOL_DEFAULT,
        m_pVtxBuff.GetAddressOf(),
        nullptr);

    //---------------------------------
    // インデックスバッファ
    //---------------------------------
    device->CreateIndexBuffer(
        sizeof(WORD) * 6 * MAX_PARTICLES,
        D3DUSAGE_WRITEONLY,
        D3DFMT_INDEX16,
        D3DPOOL_MANAGED,
        m_pIdxBuff.GetAddressOf(),
        nullptr);

    //---------------------------------
    // Index生成（固定）
    //---------------------------------
    WORD* idx = nullptr;
    m_pIdxBuff->Lock(0, 0, (void**)&idx, 0);

    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        WORD v = static_cast<WORD>(i * 4);
        int i6 = i * 6;

        // triangle 1
        idx[i6 + 0] = static_cast<WORD>(v + 0);
        idx[i6 + 1] = static_cast<WORD>(v + 1);
        idx[i6 + 2] = static_cast<WORD>(v + 2);

        // triangle 2
        idx[i6 + 3] = static_cast<WORD>(v + 2);
        idx[i6 + 4] = static_cast<WORD>(v + 1);
        idx[i6 + 5] = static_cast<WORD>(v + 3);
    }

    m_pIdxBuff->Unlock();
    return S_OK;
}

//=============================================================================
// 描画処理
//=============================================================================
void CParticleRenderer2D::Draw(
    const std::vector<PARTICLE>& particles,
    LPDIRECT3DTEXTURE9 texture,
    ID3DXEffect* /*effect*/)
{
    auto device = CManager::Instance()->GetRenderer()->GetDevice();

    //---------------------------------
    // ブレンド設定
    //---------------------------------

    device->SetRenderState(D3DRS_LIGHTING, FALSE);         // ライトを無効にする

    // Zテスト
    device->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);	// Zの比較方法
    device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);		// Zバッファに書き込まない

    // αブレンディングを加算合成に設定
    device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
    device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

    // αテストを有効
    device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);//デフォルトはfalse
    device->SetRenderState(D3DRS_ALPHAREF, 0);
    device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);//0より大きかったら描画

    //---------------------------------
    // 頂点
    //---------------------------------

    VERTEX_2D* vertex = nullptr;

    // 頂点バッファをロックし、頂点情報へのポインタを取得
    m_pVtxBuff->Lock(0, 0, (void**)&vertex, D3DLOCK_DISCARD);

    //---------------------------------
    // 頂点生成
    //---------------------------------

    int vtxCount = 0;

    for (const auto& p : particles)
    {
        if (!p.Active) continue;
        if (vtxCount >= MAX_PARTICLES * 4) break;

        float cx = p.Position.x;
        float cy = p.Position.y;

        float half = p.Size * 0.5f;

        float c = cosf(p.Rotation);
        float s = sinf(p.Rotation);

        D3DXVECTOR2 local[4] =
        {
            {-half,-half},
            { half,-half},
            {-half, half},
            { half, half}
        };

        for (int i = 0; i < 4; i++)
        {
            float rx = local[i].x * c - local[i].y * s;
            float ry = local[i].x * s + local[i].y * c;

            vertex[vtxCount + i].pos = { cx + rx, cy + ry, p.Position.z };
            vertex[vtxCount + i].rhw = 1.0f;
            vertex[vtxCount + i].col = p.Color;
        }

        vertex[vtxCount + 0].tex = { 0,0 };
        vertex[vtxCount + 1].tex = { 1,0 };
        vertex[vtxCount + 2].tex = { 0,1 };
        vertex[vtxCount + 3].tex = { 1,1 };

        vtxCount += 4;
    }

    //頂点バッファをアンロックする
    m_pVtxBuff->Unlock();

    // テクスチャの設定
    device->SetTexture(0, texture);

    // 頂点バッファをデータストリームに設定
    device->SetStreamSource(0, m_pVtxBuff.Get(), 0, sizeof(VERTEX_2D));

    // インデックスバッファの設定
    device->SetIndices(m_pIdxBuff.Get());

    // 頂点フォーマットの設定
    device->SetFVF(FVF_VERTEX_2D);

    // ポリゴンの描画
    int particleCount = vtxCount / 4;

    device->DrawIndexedPrimitive(
        D3DPT_TRIANGLELIST,
        0,
        0,
        vtxCount,
        0,
        particleCount * 2
    );

    // 元に戻す
    device->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);  // Zの比較方法
    device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);       // Zバッファに書き込む

    // αテストを無効に戻す
    device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);//デフォルトはfalse

    // αブレンディングを元に戻す
    device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
    device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    device->SetRenderState(D3DRS_LIGHTING, TRUE);          // ライトを有効にする
}

//=============================================================================
// デバイスロスト時
//=============================================================================
void CParticleRenderer2D::OnLostDevice(void)
{
    m_pVtxBuff.Reset();
}

//=============================================================================
// デバイスリセット時
//=============================================================================
void CParticleRenderer2D::OnResetDevice(void)
{
    auto device = CManager::Instance()->GetRenderer()->GetDevice();

    device->CreateVertexBuffer(
        sizeof(VERTEX_2D) * 4 * MAX_PARTICLES,
        D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
        FVF_VERTEX_2D,
        D3DPOOL_DEFAULT,
        m_pVtxBuff.GetAddressOf(),
        nullptr);
}