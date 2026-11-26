//=============================================================================
//
// UIテキストコンポーネント処理 [UITextComponent.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _UI_TEXT_COMPONENT_H_// このマクロ定義がされていなかったら
#define _UI_TEXT_COMPONENT_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Component.h"
#include "DeviceResource.h"

// 前方宣言
class CTextureResource;
class CRenderTarget;

//*****************************************************************************
// UIテキストコンポーネントクラス
//*****************************************************************************
class CUITextComponent : public CComponent, public IDeviceResource
{
public:
    CUITextComponent();
    ~CUITextComponent();

    HRESULT Init    (void) override;
    void    Uninit  (void) override;
    void    Update  (void) override;

    void SetText        (const std::string& text);
    void SetFontSize    (int size);
    void SetColor       (D3DCOLOR color);
    void SetTypingSpeed (float speed);

    void ShowAll        (void);
    void Hide           (void);

    bool IsTyping   (void) const;
    bool IsFinished (void) const;

    void OnLostDevice(void) override;
    void OnResetDevice(void) override;
    void ReleaseResource(void) override;

private:

    HRESULT CreateFont     (int size);
    void UpdateTexture  (void);

    // 一文字ごとの構造体
    struct CHAR_INFO
    {
        std::string text;
        D3DCOLOR color;
    };

    // タグ解析関数
    std::vector<CHAR_INFO> ParseColorTags(const std::string& text);

    // タグ除去関数
    std::string RemoveTags(const std::string& text);

    // タグを表示しない
    std::string GetVisibleTaggedText(int visibleCount);

private:
    static constexpr int DEFAULT_FONT_SIZE = 28;            // デフォルトのフォントサイズ

    LPD3DXFONT                          m_pFont;            // フォント
    LPD3DXSPRITE                        m_pSprite;          // スプライト
    CRenderTarget*                      m_pRenderTarget;    // レンダーターゲット
    std::shared_ptr<CTextureResource>   m_Texture;          // テクスチャ
    std::string                         m_Text;             // 文字(タグなどを含む)
    std::string                         m_DisplayText;      // 表示する文字(タグなどを除いた)
    D3DCOLOR                            m_Color;            // 色
    int                                 m_FontSize;         // フォントサイズ
    int                                 m_VisibleCount;     // 表示カウント
    float                               m_Timer;            // タイマー
    float                               m_TypingSpeed;      // 表示スピード
    bool                                m_IsFinished;       // 表示し終えたか
    std::string m_RenderTargetName;
    static unsigned int s_NextID;
    unsigned int m_ID = 0;
    bool m_NeedsRedraw;
};

#endif