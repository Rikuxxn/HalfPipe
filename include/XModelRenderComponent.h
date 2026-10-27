//=============================================================================
//
// Xモデルレンダーコンポーネント処理 [Component.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _XMODEL_RENDER_COMPONENT_H_// このマクロ定義がされていなかったら
#define _XMODEL_RENDER_COMPONENT_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "RenderComponent.h"

// 前方宣言
class CXModelResource;

// サブセットごとのテクスチャ上書き設定
struct FaceTextureOverride
{
    int FaceIndex = 0;
    std::shared_ptr<CTextureResource> Texture;
};

//*****************************************************************************
// Xモデルレンダーコンポーネント
//*****************************************************************************
class CXModelRenderComponent : public CRenderComponent
{
public:

    std::shared_ptr<CXModelResource> m_Model;

    D3DXCOLOR m_Color;

    bool m_Visible;

public:

    CXModelRenderComponent()
    {
        //SetLayer(RenderLayer::Model);
        m_Color = D3DXCOLOR(1, 1, 1, 1);
        m_Visible = true;
    }

    D3DXVECTOR3 GetModelSize(void) const;
    std::shared_ptr<CXModelResource> GetModel(void) const;

public:

    virtual void Draw(void) override;

    HRESULT Load(const char* filepath);

    // 面のテクスチャを設定
    void SetFaceTexture(
        int faceIndex,
        const std::string& path);

    // 面全てにテクスチャを設定
    void SetAllFaceTexture(const std::string& path);

    // 面のテクスチャを削除
    void ClearFaceTexture(int faceIndex);

    // すべての面のテクスチャを削除
    void ClearFaceTextures(void);

    // 面のテクスチャ取得
    const std::unordered_map<
        int,
        std::shared_ptr<CTextureResource>>&
        GetFaceTextures(void) const;

    // 表示・非表示
    void SetVisible(bool visible) { m_Visible = visible; }
    bool IsVisible(void) const { return m_Visible; }

private:

    // シャドウ描画用
    void DrawShadow(void) override
    {
        Draw();
    }

    // 影を描画するか
    bool IsShadowCaster(void) override
    {
        return true;
    }

private:

    // サブセットごとのテクスチャ上書き
    std::unordered_map<
        int,
        std::shared_ptr<CTextureResource>
    > m_FaceTextures;
};

#endif