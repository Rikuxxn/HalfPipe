//=============================================================================
//
// テキストスプライト処理 [TextSprite.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _TEXT_SPRITE_H_// このマクロ定義がされていなかったら
#define _TEXT_SPRITE_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************

// 前方宣言
class CGameObject;
class CTextureResource;

//*****************************************************************************
// テキストスプライトクラス
//*****************************************************************************
class CTextSprite
{
public:
    CTextSprite();
    ~CTextSprite();

    HRESULT CreateFont(int size);
    HRESULT Init(void);
    void Start(const std::string& text, int fontSize = DEFAULT_FONT_SIZE, float visibleSpeed = 0.05f);    // 表示開始処理
    void ShowAll(void);                     // 全文表示
    bool IsTyping(void) const;              // 表示中かどうか

    void Uninit(void);
    void Update(void);
    bool IsFinished(void) const;
    void UpdateTexture(void);
    void Hide(void);

    void SetPos(D3DXVECTOR3 pos);

    void OnLostDevice(void);
    void OnResetDevice(void);

private:

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
    static constexpr int DEFAULT_FONT_SIZE = 28;    // デフォルトのフォントサイズ

    LPD3DXFONT              m_pFont;                // フォント
    LPD3DXSPRITE            m_pSprite;              // スプライト
    LPDIRECT3DSURFACE9      m_pRenderSurface;       // レンダーサーフェス
    LPDIRECT3DSURFACE9      m_pOldSurface;          // オールドサーフェス
    std::shared_ptr<CTextureResource>      m_pTexture;             // テクスチャへのポインタ
    CGameObject*            m_pFontTex;             // 文字を描画する用の2Dスプライト
    CGameObject*            m_pBack;                // 文字の背景2Dスプライト
    D3DXVECTOR3             m_worldPos;             // 位置
    std::string             m_text;                 // 文字(タグなどを含む)
    std::string             m_displayText;          // 表示する文字(タグなどを除いた)
    int                     m_visibleCount;         // 表示数
    int                     m_fontSize;             // フォントサイズ
    float                   m_timer;                // タイマー(表示間隔用)
    float                   m_visibleSpeed;         // 表示スピード
    bool                    m_finished;             // 表示し終えたか
};

#endif