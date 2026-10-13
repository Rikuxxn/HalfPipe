//=============================================================================
//
// メッシュベース処理 [MeshBase.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "MeshBase.h"
#include "Manager.h"
#include "Renderer.h"


//=============================================================================
// コンストラクタ
//=============================================================================
CMeshBase::CMeshBase() :
	m_VertexCount		(0),
	m_IndexCount		(0),
	m_PrimitiveCount	(0)
{

}

//=============================================================================
// 頂点・インデックスバッファ生成処理
//=============================================================================
bool CMeshBase::Create(UINT vertexCount, UINT primitiveCount, UINT indexCount)
{
	auto device = CManager::Instance()->GetRenderer()->GetDevice();

	m_VertexCount = vertexCount;
	m_PrimitiveCount = primitiveCount;
	m_IndexCount = indexCount;

	// 頂点バッファ生成
	if (FAILED(device->CreateVertexBuffer(
		sizeof(VERTEX_3D) * vertexCount,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		m_pVtxBuff.GetAddressOf(),
		nullptr)))
	{
		return false;
	}

	// インデックスバッファ生成
	if (FAILED(device->CreateIndexBuffer(
		sizeof(WORD) * indexCount,
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		m_pIdxBuff.GetAddressOf(),
		nullptr)))
	{
		return false;
	}

	return true;
}

//=============================================================================
// 頂点設定
//=============================================================================
void CMeshBase::SetVertex(int index, const D3DXVECTOR3& pos, const D3DXVECTOR3& normal, const D3DXVECTOR2& uv, const D3DXCOLOR& color)
{
	VERTEX_3D* pVtx = nullptr;

	// 頂点バッファをロック
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[index].pos = pos;

	//法線ベクトルの設定
	pVtx[index].nor = normal;

	//頂点カラーの設定
	pVtx[index].col = color;

	//テクスチャ座標の設定
	pVtx[index].tex = uv;

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();
}

//=============================================================================
// インデックスバッファ設定
//=============================================================================
void CMeshBase::SetIndex(int index, WORD value)
{
	WORD* pIdx;

	//インデックスバッファのロック
	m_pIdxBuff->Lock(0, 0, (void**)&pIdx, 0);

	pIdx[index] = value;

	//インデックスバッファのアンロック
	m_pIdxBuff->Unlock();
}

//=============================================================================
// 頂点バッファロック
//=============================================================================
VERTEX_3D* CMeshBase::LockVertexBuffer(void)
{
	VERTEX_3D* p = nullptr;
	m_pVtxBuff->Lock(0, 0, (void**)&p, 0);
	return p;
}

//=============================================================================
// インデックスバッファロック
//=============================================================================
WORD* CMeshBase::LockIndexBuffer(void)
{
	WORD* pIdx = nullptr;
	m_pIdxBuff->Lock(0, 0, (void**)&pIdx, 0);
	return pIdx;
}

//=============================================================================
// 頂点バッファアンロック
//=============================================================================
void CMeshBase::UnlockVertexBuffer(void)
{
	m_pVtxBuff->Unlock();
}

//=============================================================================
// インデックスバッファアンロック
//=============================================================================
void CMeshBase::UnlockIndexBuffer(void)
{
	m_pIdxBuff->Unlock();
}

//=============================================================================
// 頂点バッファ位置取得
//=============================================================================
D3DXVECTOR3 CMeshBase::GetVertexPos(int index) const
{
	// 頂点情報のポインタを宣言
	VERTEX_3D* pVtx = nullptr;

	D3DXVECTOR3 out;

	// nullptrだったら
	if (m_pVtxBuff == nullptr)
	{
		return INIT_VEC3;
	}

	// 頂点バッファをロック
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	out = pVtx[index].pos;

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();

	return out;
}

//=============================================================================
// メッシュ描画処理
//=============================================================================
void CMeshBase::Draw(void)
{
	// デバイスの取得
	auto device = CManager::Instance()->GetRenderer()->GetDevice();

	// 頂点バッファをデバイスのデータストリームに設定
	device->SetStreamSource(0, m_pVtxBuff.Get(), 0, sizeof(VERTEX_3D));

	// インデックスバッファをデータストリームに設定
	device->SetIndices(m_pIdxBuff.Get());

	// テクスチャフォーマットの設定
	device->SetFVF(FVF_VERTEX_3D);

	// ポリゴンの描画
	device->DrawIndexedPrimitive(m_PrimitiveType, 0, 0, m_VertexCount, 0, m_PrimitiveCount);
}