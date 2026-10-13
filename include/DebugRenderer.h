//=============================================================================
//
// デバッグ描画処理 [DebugRenderer.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _DEBUG_RENDERER_H_
#define _DEBUG_RENDERER_H_

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Grid.h"

// 前方宣言

//*****************************************************************************
// デバッグ描画クラス
//*****************************************************************************
class CDebugRenderer
{
public:
    CDebugRenderer() = default;
    ~CDebugRenderer() = default;

    HRESULT Init(void);
    void Draw(void);
    void DrawGrid(void);
    void AddLine(
        const D3DXVECTOR3& p1,
        const D3DXVECTOR3& p2,
        D3DCOLOR color);

private:

    std::unique_ptr<CGrid> m_Grid;
};

#endif