//=============================================================================
//
// 当たり判定デバッグ描画処理 [DebugDrawer.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _DEBUG_DRAWER_H_// このマクロ定義がされていなかったら
#define _DEBUG_DRAWER_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Manager.h"
#include "Renderer.h"


//*****************************************************************************
// 当たり判定デバッグ描画クラス
//*****************************************************************************
class CDebugDrawer : public btIDebugDraw
{
public:
    int m_debugMode = DBG_DrawWireframe;

    // ライン描画関数
    void DrawLine(const D3DXVECTOR3& start, const D3DXVECTOR3& end, D3DXCOLOR color)
    {
        // デバイスの取得
        auto pDevice = CManager::Instance()->GetRenderer()->GetDevice();

        struct VERTEX
        {
            D3DXVECTOR3 pos;
            D3DCOLOR color;
        };

        D3DXCOLOR col = D3DCOLOR_XRGB(
            (int)(color.r * 255.0f),
            (int)(color.g * 255.0f),
            (int)(color.b * 255.0f)
        );

        VERTEX v[2] =
        {
            { start, col },
            { end,   col }
        };

        // 頂点フォーマット設定
        pDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);

        pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);         // ライトを無効にする
        pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE); // αテストを無効にする

        // ライン描画
        pDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1, v, sizeof(VERTEX));

        pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);  // αテストを有効にする
        pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);          // ライトを有効にする
    }

    void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override
    {
        D3DXVECTOR3 p0(
            static_cast<FLOAT>(from.x()),
            static_cast<FLOAT>(from.y()),
            static_cast<FLOAT>(from.z()));

        D3DXVECTOR3 p1(
            static_cast<FLOAT>(to.x()), 
            static_cast<FLOAT>(to.y()), 
            static_cast<FLOAT>(to.z()));

        // ライン描画
        DrawLine(p0, p1, D3DCOLOR_XRGB(
            (int)(color.x() * 255),
            (int)(color.y() * 255),
            (int)(color.z() * 255)
        ));
    }

    void setDebugMode(int mode) override { m_debugMode = mode; }
    int getDebugMode() const override { return m_debugMode; }

    void reportErrorWarning(const char* /*warning*/) override {}
    void draw3dText(const btVector3&, const char*) override {}
    void drawContactPoint(const btVector3&, const btVector3&, btScalar, int, const btVector3&) override {}

    // 選択中のオブジェクト
    void SetSelectedObject(CGameObject* object) { m_pSelectedObject = object; }
    CGameObject* GetSelectedObject(void) const { return m_pSelectedObject; }

private:

    CGameObject* m_pSelectedObject = nullptr;
};

#endif