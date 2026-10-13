//=============================================================================
//
// メッシュベース処理 [MeshBase.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _MESH_BASE_H_// このマクロ定義がされていなかったら
#define _MESH_BASE_H_// 2重インクルード防止のマクロ定義


//*****************************************************************************
// インクルードファイル
//*****************************************************************************


//*****************************************************************************
// メッシュベースクラス
//*****************************************************************************
class CMeshBase
{
public:
    CMeshBase();
    ~CMeshBase() = default;

    bool Create(UINT vertexCount,
        UINT primitiveCount,
        UINT indexCount);

    void SetVertex(
        int index,
        const D3DXVECTOR3& pos,
        const D3DXVECTOR3& normal,
        const D3DXVECTOR2& uv,
        const D3DXCOLOR& color);

    void SetIndex(int index, WORD value);

    void SetPrimitiveType(D3DPRIMITIVETYPE type) { m_PrimitiveType = type; }

    VERTEX_3D* LockVertexBuffer(void);
    WORD* LockIndexBuffer(void);

    void UnlockVertexBuffer(void);
    void UnlockIndexBuffer(void);

    D3DXVECTOR3 GetVertexPos(int index) const;

    void Draw(void);

    IDirect3DVertexBuffer9* GetVertexBuffer(void) const
    {
        return m_pVtxBuff.Get();
    }

    IDirect3DIndexBuffer9* GetIndexBuffer(void) const
    {
        return m_pIdxBuff.Get();
    }

    UINT GetVertexCount(void) const
    {
        return m_VertexCount;
    }

    UINT GetPrimitiveCount(void) const
    {
        return m_PrimitiveCount;
    }

private:

    ComPtr<IDirect3DVertexBuffer9> m_pVtxBuff;
    ComPtr<IDirect3DIndexBuffer9>  m_pIdxBuff;

    UINT m_VertexCount;
    UINT m_IndexCount;
    UINT m_PrimitiveCount;

    D3DPRIMITIVETYPE m_PrimitiveType = D3DPT_TRIANGLESTRIP; // プリミティブタイプ
};

#endif