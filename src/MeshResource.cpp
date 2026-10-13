//=============================================================================
//
// メッシュリソース処理 [MeshResource.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "MeshResource.h"
#include "Manager.h"
#include "Renderer.h"
#include "MathConstants.h"
#include "ColorConstants.h"



//=============================================================================
// メッシュフィールド生成処理
//=============================================================================
HRESULT CMeshResource::CreatePlane(float width, float depth, int segX, int segZ)
{
	// 名前空間 ColorConstの使用
	using namespace ColorConst;

	// プリミティブタイプの設定
	SetPrimitiveType(D3DPT_TRIANGLESTRIP);

	// 頂点計算
	int vertexCount = (segX + 1) * (segZ + 1);	// 頂点数
	int primitiveCount = (((segX * segZ) * 2)) + (4 * (segZ - 1)); // ポリゴン数
	int idxCount = primitiveCount + 2; // インデックス数

	// 頂点・インデックスバッファ生成
	if (!Create(vertexCount, primitiveCount, idxCount))
	{
		return E_FAIL;
	}

	// 頂点バッファの取得をして、ロック
	VERTEX_3D* pVtx = LockVertexBuffer();

	// テクスチャ座標を計算する変数
	float fTexX = TEX_SPRITES_NUM / segX;
	float fTexY = TEX_SPRITES_NUM / segZ;
	int index = 0;

	// 縦
	for (int nCntZ = 0; nCntZ <= segZ; nCntZ++)
	{
		// 横
		for (int nCntX = 0; nCntX <= segX; nCntX++)
		{
			// 頂点座標を計算
			float px = ((width / segX) * nCntX) - (width * MathConst::HALF);
			float py = 0.0f;
			float pz = depth - ((depth / segZ) * nCntZ) - (depth * MathConst::HALF);

			// 頂点座標の設定
			pVtx[index].pos = { px, py, pz };

			// 法線ベクトルの設定
			pVtx[index].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

			// 頂点カラーの設定
			pVtx[index].col = D3D::WHITE;

			// テクスチャ座標の設定
			pVtx[index].tex = D3DXVECTOR2(fTexX * nCntX, nCntZ * fTexY);

			// 加算
			index++;
		}
	}

	// 頂点バッファアンロック
	UnlockVertexBuffer();

	// インデックスバッファのポインタ
	/*
	* 使える頂点番号(16bitインデックス)は0～65535まで
	*/
	WORD* pIdx = LockIndexBuffer();
	WORD IndxNum = (WORD)segX + 1;// X
	WORD IdxCnt = 0;// 配列
	WORD i = 0;

	// インデックスの設定
	for (int IndxCount1 = 0; IndxCount1 < segZ; IndxCount1++)
	{
		for (int IndxCount2 = 0; IndxCount2 <= segX; IndxCount2++, IndxNum++, i++)
		{
			pIdx[IdxCnt] = IndxNum;
			pIdx[IdxCnt + 1] = i;
			IdxCnt += 2;
		}

		// 最後の行じゃなかったら
		if (IndxCount1 < segZ - 1)
		{
			pIdx[IdxCnt] = i - 1;
			pIdx[IdxCnt + 1] = IndxNum;
			IdxCnt += 2;
		}
	}

	// インデックスバッファアンロック
	UnlockIndexBuffer();

	return S_OK;
}

//=============================================================================
// メッシュスフィア生成処理
//=============================================================================
HRESULT CMeshResource::CreateSphere(float radius, int slices, int stacks)
{
	// 名前空間 ColorConstの使用
	using namespace ColorConst;

	// プリミティブタイプの設定
	SetPrimitiveType(D3DPT_TRIANGLELIST);

	int vertexCount = (slices + 1) * (stacks + 1);
	int primitiveCount = slices * stacks * 2;
	int indexCount = primitiveCount * 3;

	// 頂点・インデックスバッファ生成
	if (!Create(vertexCount, primitiveCount, indexCount))
	{
		return E_FAIL;
	}

	// 頂点バッファを取得して、ロック
	VERTEX_3D* pVtx = LockVertexBuffer();

	int index = 0;

	for (int stack = 0; stack <= stacks; stack++)
	{
		float v = (float)stack / stacks;
		float phi = D3DX_PI * v;

		for (int slice = 0; slice <= slices; slice++)
		{
			float u = (float)slice / slices;
			float theta = D3DX_PI * 2.0f * u;

			float x = radius * sinf(phi) * cosf(theta);
			float y = radius * cosf(phi);
			float z = radius * sinf(phi) * sinf(theta);

			D3DXVECTOR3 pos(x, y, z);

			// 頂点座標の設定
			pVtx[index].pos = pos;

			// 法線ベクトルの設定
			pVtx[index].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

			// 色の設定
			pVtx[index].col = D3D::WHITE;

			// テクスチャ座標の設定
			pVtx[index].tex = D3DXVECTOR2(u, v);

			index++;
		}
	}

	// アンロック
	UnlockVertexBuffer();


	WORD* pIdx = LockIndexBuffer();

	int idx = 0;

	for (int stack = 0; stack < stacks; stack++)
	{
		for (int slice = 0; slice < slices; slice++)
		{
			WORD a =  static_cast<WORD>(stack * (slices + 1) + slice);
			WORD b =  static_cast<WORD>(a + slices + 1);
			WORD c = a + 1;
			WORD d = b + 1;

			// Triangle 1
			pIdx[idx++] = a;
			pIdx[idx++] = b;
			pIdx[idx++] = c;

			// Triangle 2
			pIdx[idx++] = c;
			pIdx[idx++] = b;
			pIdx[idx++] = d;
		}
	}

	UnlockIndexBuffer();

	return S_OK;
}