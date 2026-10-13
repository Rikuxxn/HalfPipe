//=============================================================================
//
// Xモデルマネージャー処理 [XModelManager.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "XModelResource.h"
#include "Manager.h"
#include "Renderer.h"
#include <cassert>

//=============================================================================
// 読み込み処理
//=============================================================================
std::shared_ptr<CXModelResource> CXModelResource::Load(const char* filepath)
{
    auto model = std::make_shared<CXModelResource>();

    auto device = CManager::Instance()->GetRenderer()->GetDevice();

    LPD3DXBUFFER materialBuffer = nullptr;

    DWORD materialNum = 0;

    if (FAILED(
        D3DXLoadMeshFromX(
            filepath,
            D3DXMESH_MANAGED,
            device,
            nullptr,
            &materialBuffer,
            nullptr,
            &materialNum,
            &model->Mesh)))
    {
        return nullptr;
    }

    // メッシュ最適化
    std::vector<DWORD> adjacency(model->Mesh->GetNumFaces() * 3);

    model->Mesh->GenerateAdjacency(1e-6f, adjacency.data());

    model->Mesh->OptimizeInplace(
        D3DXMESHOPT_VERTEXCACHE |
        D3DXMESHOPT_ATTRSORT |
        D3DXMESHOPT_COMPACT,
        adjacency.data(),
        nullptr,
        nullptr,
        nullptr
    );
    

    //---------------------------------
    // メッシュ情報取得
    //---------------------------------

    model->Mesh->GetVertexBuffer(&model->VertexBuffer);
    model->Mesh->GetIndexBuffer(&model->IndexBuffer);

    model->FVF = model->Mesh->GetFVF();

    model->VertexStride = D3DXGetFVFVertexSize(model->FVF);
    model->NumVertices = model->Mesh->GetNumVertices();
    model->NumFaces = model->Mesh->GetNumFaces();


    //---------------------------------
    // サブセット情報取得
    //---------------------------------

    DWORD count = 0;

    model->Mesh->GetAttributeTable(nullptr, &count);

    std::vector<D3DXATTRIBUTERANGE> table(count);

    model->Mesh->GetAttributeTable(table.data(), &count);

    model->Subsets.reserve(count);

    for (DWORD i = 0; i < count; i++)
    {
        SUBSET subset;

        subset.FaceStart = table[i].FaceStart;
        subset.FaceCount = table[i].FaceCount;

        subset.VertexStart = table[i].VertexStart;
        subset.VertexCount = table[i].VertexCount;
        subset.MaterialIndex = table[i].AttribId;

        model->Subsets.push_back(subset);
    }

    model->NumSubsets = count;

    //---------------------------------
    // モデルサイズ取得
    //---------------------------------

    BYTE* pVertices = nullptr;

    model->Mesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVertices);

    DWORD vertexCount = model->Mesh->GetNumVertices();

    DWORD stride = D3DXGetFVFVertexSize(model->Mesh->GetFVF());

    // 最大値と最小値
    D3DXVECTOR3 vMin(FLT_MAX, FLT_MAX, FLT_MAX);
    D3DXVECTOR3 vMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);

    for (DWORD i = 0; i < vertexCount; i++)
    {
        D3DXVECTOR3* pos = (D3DXVECTOR3*)(pVertices + stride * i);

        vMin.x = std::min(vMin.x, pos->x);
        vMin.y = std::min(vMin.y, pos->y);
        vMin.z = std::min(vMin.z, pos->z);

        vMax.x = std::max(vMax.x, pos->x);
        vMax.y = std::max(vMax.y, pos->y);

        vMax.z = std::max(vMax.z, pos->z);
    }

    model->Mesh->UnlockVertexBuffer();

    model->Min = vMin;
    model->Max = vMax;

    // 元サイズ = 最大 - 最小
    model->Size = vMax - vMin;

    //// スムーズシェーディング用の法線を生成する(スムーズ化)
    //{
    //    // 一時メッシュポインタ
    //    ID3DXMesh* pTempMesh = nullptr;

    //    // 隣接情報の確保と作成
    //    DWORD* pAdjacency = new DWORD[m_pMesh->GetNumFaces() * 3];
    //    m_pMesh->GenerateAdjacency(1e-6f, pAdjacency);

    //    // 法線生成（スムーズ化）
    //    HRESULT hr = D3DXComputeNormals(m_pMesh, pAdjacency);

    //    if (FAILED(hr))
    //    {
    //        // モデルに法線が無い場合、いったんクローンして法線を付与
    //        D3DVERTEXELEMENT9 decl[MAX_FVF_DECL_SIZE];
    //        m_pMesh->GetDeclaration(decl);
    //        m_pMesh->CloneMesh(D3DXMESH_SYSTEMMEM, decl, device, &pTempMesh);

    //        if (pTempMesh)
    //        {
    //            pTempMesh->GenerateAdjacency(1e-6f, pAdjacency);
    //            D3DXComputeNormals(pTempMesh, pAdjacency);

    //            // メッシュに差し替え
    //            m_pMesh->Release();
    //            m_pMesh = pTempMesh;
    //        }
    //    }

    //    // 隣接情報解放
    //    delete[] pAdjacency;
    //}

    //---------------------------------
    // Material
    //---------------------------------

    auto mats = (D3DXMATERIAL*)materialBuffer->GetBufferPointer();

    for (DWORD i = 0; i < materialNum; i++)
    {
        model->Materials.push_back(mats[i].MatD3D);

        //---------------------------------
        // Texture
        //---------------------------------

        ComPtr<IDirect3DTexture9> tex;

        if (mats[i].pTextureFilename)
        {
            D3DXCreateTextureFromFile(
                device,
                mats[i].pTextureFilename,
                &tex);
        }

        model->Textures.push_back(tex);
    }

    materialBuffer->Release();

    return model;
}

//=============================================================================
// Bullet Physics三角形メッシュ生成
//=============================================================================
btTriangleMesh* CXModelResource::CreateBulletTriangleMesh(void) const
{
    if (!Mesh)
    {
        return nullptr;
    }

    auto* triangleMesh = new btTriangleMesh();

    BYTE* vertexData = nullptr;
    void* indexData = nullptr;

    //=========================================================
    // 頂点バッファをロック
    //=========================================================
    HRESULT hr = Mesh->LockVertexBuffer(
        D3DLOCK_READONLY,
        reinterpret_cast<void**>(&vertexData)
    );

    if (FAILED(hr))
    {
        delete triangleMesh;
        return nullptr;
    }

    //=========================================================
    // インデックスバッファをロック
    //=========================================================
    hr = Mesh->LockIndexBuffer(
        D3DLOCK_READONLY,
        &indexData
    );

    if (FAILED(hr))
    {
        Mesh->UnlockVertexBuffer();

        delete triangleMesh;
        return nullptr;
    }

    //=========================================================
    // Index Format
    //=========================================================
    ComPtr<IDirect3DIndexBuffer9> indexBuffer;

    hr = Mesh->GetIndexBuffer(indexBuffer.GetAddressOf());

    if (FAILED(hr))
    {
        Mesh->UnlockVertexBuffer();
        delete triangleMesh;
        return nullptr;
    }

    D3DINDEXBUFFER_DESC indexDesc{};

    hr = indexBuffer->GetDesc(&indexDesc);

    if (FAILED(hr))
    {
        Mesh->UnlockVertexBuffer();
        delete triangleMesh;
        return nullptr;
    }

    const bool is32Bit = (indexDesc.Format == D3DFMT_INDEX32);

    //=========================================================
    // 三角形
    //=========================================================
    for (UINT face = 0; face < NumFaces; ++face)
    {
        UINT i0;
        UINT i1;
        UINT i2;

        if (is32Bit)
        {
            const DWORD* indices = reinterpret_cast<const DWORD*>(indexData);

            i0 = indices[face * 3 + 0];
            i1 = indices[face * 3 + 1];
            i2 = indices[face * 3 + 2];
        }
        else
        {
            const WORD* indices = reinterpret_cast<const WORD*>(indexData);

            i0 = indices[face * 3 + 0];
            i1 = indices[face * 3 + 1];
            i2 = indices[face * 3 + 2];
        }

        //=====================================================
        // 頂点取得
        //=====================================================
        const BYTE* v0 = vertexData + i0 * VertexStride;
        const BYTE* v1 = vertexData + i1 * VertexStride;
        const BYTE* v2 = vertexData + i2 * VertexStride;

        const D3DXVECTOR3& p0 = *reinterpret_cast<const D3DXVECTOR3*>(v0);
        const D3DXVECTOR3& p1 = *reinterpret_cast<const D3DXVECTOR3*>(v1);
        const D3DXVECTOR3& p2 = *reinterpret_cast<const D3DXVECTOR3*>(v2);

        //=====================================================
        // Bullet TriangleMeshへ追加
        //=====================================================
        triangleMesh->addTriangle(
            btVector3(p0.x, p0.y, p0.z),
            btVector3(p1.x, p1.y, p1.z),
            btVector3(p2.x, p2.y, p2.z)
        );
    }

    //=========================================================
    // アンロック
    //=========================================================
    Mesh->UnlockIndexBuffer();
    Mesh->UnlockVertexBuffer();

    return triangleMesh;
}