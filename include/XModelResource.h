//=============================================================================
//
// Xモデルリソース処理 [XModelResource.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _XMODELRESOURCE_H_// このマクロ定義がされていなかったら
#define _XMODELRESOURCE_H_// 2重インクルード防止のマクロ定義

// 前方宣言
class CTextureResource;

//*****************************************************************************
// Xモデルリソースクラス
//*****************************************************************************
class CXModelResource
{
public:
    CXModelResource() = default;
    ~CXModelResource() = default;

    static std::shared_ptr<CXModelResource> Load(const char* filepath);

    const D3DXVECTOR3& GetSize(void) const { return Size; }

    ID3DXBaseMesh* GetMesh(void) const { return Mesh.Get(); }

    // Bullet用の三角形メッシュを生成
    btTriangleMesh* CreateBulletTriangleMesh(void) const;

private:

    struct SUBSET
    {
        DWORD FaceStart;
        DWORD FaceCount;
        DWORD VertexStart;
        DWORD VertexCount;
        UINT MaterialIndex;
    };

public:

    ComPtr<ID3DXMesh>                       Mesh;       // メッシュ
    std::vector<D3DMATERIAL9>               Materials;  // マテリアル
    std::vector<ComPtr<IDirect3DTexture9>>  Textures;   // テクスチャ

    D3DXVECTOR3 Min;    // 最小値
    D3DXVECTOR3 Max;    // 最大値
    D3DXVECTOR3 Size;   // サイズ

    // インスタンシング用
    ComPtr<IDirect3DVertexBuffer9> VertexBuffer;
    ComPtr<IDirect3DIndexBuffer9> IndexBuffer;

    DWORD FVF = 0;

    UINT VertexStride = 0;

    UINT NumVertices = 0;
    UINT NumFaces = 0;
    UINT NumSubsets = 0;

    std::vector<SUBSET> Subsets;
};

#endif
