//=============================================================================
//
// テキストスプライト処理 [TextSprite.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "TextSprite.h"
#include "Manager.h"
#include "Renderer.h"
#include "TimeSystem.h"
#include "GameObject.h"
#include "Scene.h"
#include "SceneManager.h"
#include "TransformComponent.h"
#include "SpriteRenderComponent.h"
#include "ColorConstants.h"
#include "TextureResource.h"

// 名前空間の使用
using namespace ColorConst;


//=============================================================================
// コンストラクタ
//=============================================================================
CTextSprite::CTextSprite() :
    m_pSprite       (nullptr),      // スプライト
    m_pFontTex      (nullptr),      // 文字を描画する用のビルボード
    m_pBack         (nullptr),      // 背景用のビルボード
    m_pFont         (nullptr),      // フォント
    m_visibleCount  (0),            // 表示数
    m_timer         (0.0f),         // タイマー(表示間隔用)
    m_finished      (false),        // 表示し終えたか
    m_worldPos      (INIT_VEC3),    // 位置
    m_fontSize      (0),            // フォントサイズ
    m_visibleSpeed  (0.05f)         // 文字表示スピード
{
    // 値のクリア
}

//=============================================================================
// デストラクタ
//=============================================================================
CTextSprite::~CTextSprite()
{
    Uninit();
}

//=============================================================================
// フォント生成処理
//=============================================================================
HRESULT CTextSprite::CreateFont(int size)
{
    // デバイスの取得
    auto device = CManager::Instance()->GetRenderer()->GetDevice();

    if (m_pFont)
    {
        m_pFont->Release();
        m_pFont = nullptr;
    }

    // フォントサイズの設定
    m_fontSize = size;

    return D3DXCreateFont(
        device,
        m_fontSize,
        0,
        0,
        0,
        FALSE,
        SHIFTJIS_CHARSET,
        OUT_DEFAULT_PRECIS,
        ANTIALIASED_QUALITY,
        DEFAULT_PITCH,
        "MS Gothic",
        &m_pFont);
}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CTextSprite::Init(void)
{
    auto device = CManager::Instance()->GetRenderer()->GetDevice();

    // フォントの生成
    if (FAILED(CreateFont(DEFAULT_FONT_SIZE)))
    {
        MessageBox(nullptr, "D3DXCreateFont Failed!", "Error", MB_OK);
        return E_FAIL;
    }

    // スプライト生成
    HRESULT hr = D3DXCreateSprite(device, &m_pSprite);
    if (FAILED(hr))
    {
        MessageBox(nullptr, "D3DXCreateSprite Failed!", "Error", MB_OK);
        return E_FAIL;
    }

    // 描画先テクスチャ
    device->CreateTexture(
        512,
        128,
        1,
        D3DUSAGE_RENDERTARGET,
        D3DFMT_A8R8G8B8,
        D3DPOOL_DEFAULT,
        &m_pTexture->Texture,
        nullptr);

    m_pTexture->Texture->GetSurfaceLevel(
        0,
        &m_pRenderSurface);

    // シーンの取得
    auto scene = CManager::Instance()->GetSceneManager()->GetScene();

    if (!scene)
    {
        return E_FAIL;
    }

    // 背景用の2Dスプライト
    m_pBack = scene->CreateObject();

    // トランスフォームコンポーネントの追加
    auto transformBack = m_pBack->AddComponent<CTransformComponent>();

    transformBack->Position = { 0.0f, 0.0f, 0.0f };

    // スプライトレンダーコンポーネントの追加
    auto spriteBack = m_pBack->AddComponent<CSpriteRenderComponent>();

    spriteBack->SetLayer(RenderLayer::UI);

    // テクスチャの設定
    spriteBack->SetTexture("data/TEXTURE/.png");

    spriteBack->SetIsVisible(true);

    // 文字を描画する用の2Dスプライト
    m_pFontTex = scene->CreateObject();

    // トランスフォームコンポーネントの追加
    auto transformFont = m_pFontTex->AddComponent<CTransformComponent>();

    transformFont->Position = { 0.0f, 0.0f, 0.0f };

    // スプライトレンダーコンポーネントの追加
    auto spriteFont = m_pFontTex->AddComponent<CSpriteRenderComponent>();

    spriteFont->SetLayer(RenderLayer::UI);

    // テクスチャの設定
    spriteFont->SetTexture("data/TEXTURE/.png");

    spriteFont->SetIsVisible(true);

    spriteFont->SetTexture(m_pTexture);

    // 初期は透明にしておいて、文字を表示する際に一緒に表示
    spriteBack->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));
    spriteFont->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));

    return S_OK;
}

//=============================================================================
// テクスチャ更新処理
//=============================================================================
void CTextSprite::UpdateTexture(void)
{
    if (!m_pTexture ||
        !m_pRenderSurface ||
        !m_pFont)
    {
        return;
    }

    auto device = CManager::Instance()->GetRenderer()->GetDevice();

    device->SetTexture(0, nullptr);

    device->GetRenderTarget(0, &m_pOldSurface);

    // Zバッファの退避と解除 sato (前の文字が表示されないやつの原因)
    IDirect3DSurface9* pDepthStencil = nullptr;
    device->GetDepthStencilSurface(&pDepthStencil);
    device->SetDepthStencilSurface(nullptr);

    device->SetRenderTarget(0, m_pRenderSurface);

    device->Clear(
        0,
        nullptr,
        D3DCLEAR_TARGET,
        D3DCOLOR_ARGB(0, 0, 0, 0),
        1.0f,
        0);

    // タグを考慮した文字列を返す
    std::string drawText = GetVisibleTaggedText(m_visibleCount);

    auto chars = ParseColorTags(drawText);

    // 初期位置
    int x = 5;
    int y = 20;

    const int lineHeight = m_fontSize;

    HRESULT hr = m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);

    if (FAILED(hr))
    {
        OutputDebugStringA("Sprite Begin Failed\n");
    }

    for (auto& ch : chars)
    {
        if (ch.text == "\n")
        {
            x = 0;
            y += lineHeight;
            continue;
        }

        RECT drawRect =
        {
            x,
            y,
            512,
            128
        };

        m_pFont->DrawTextA(
            m_pSprite,
            ch.text.c_str(),
            -1,
            &drawRect,
            /*DT_CENTER |*/ DT_LEFT /* | DT_WORDBREAK*/,
            ch.color);

        RECT calcRect =
        {
            0,
            0,
            0,
            0
        };

        m_pFont->DrawTextA(
            nullptr,
            ch.text.c_str(),
            -1,
            &calcRect,
            DT_CALCRECT,
            ch.color);

        int width = calcRect.right - calcRect.left;

        x += width;
    }

    m_pSprite->End();

    device->SetRenderTarget(0, m_pOldSurface);

    // Zバッファを戻す sato
    if (pDepthStencil)
    {
        device->SetDepthStencilSurface(pDepthStencil);
        pDepthStencil->Release();
        pDepthStencil = nullptr;
    }

    if (m_pOldSurface)
    {
        m_pOldSurface->Release();
        m_pOldSurface = nullptr;
    }
}

//=============================================================================
// 終了処理
//=============================================================================
void CTextSprite::Uninit(void)
{
    if (m_pFont != nullptr)
    {
        m_pFont->Release();
        m_pFont = nullptr;
    }

    //if (m_pTexture)
    //{
    //    m_pTexture->Release();
    //    m_pTexture = nullptr;
    //}

    if (m_pRenderSurface != nullptr)
    {
        m_pRenderSurface->Release();
        m_pRenderSurface = nullptr;
    }

    if (m_pOldSurface != nullptr)
    {
        m_pOldSurface->Release();
        m_pOldSurface = nullptr;
    }

    if (m_pSprite)
    {
        m_pSprite->Release();
        m_pSprite = nullptr;
    }
}

//=============================================================================
// 表示開始処理
//=============================================================================
void CTextSprite::Start(const std::string& text, int fontSize, float visibleSpeed)
{
    if (fontSize != m_fontSize)
    {
        CreateFont(fontSize);
    }

    m_displayText = RemoveTags(text);
    m_text = text;

    // 表示スピード
    m_visibleSpeed = visibleSpeed;

    // 最初は0文字表示
    m_visibleCount = 0;

    // タイマーリセット
    m_timer = 0.0f;

    m_finished = false;

    // 吹き出し表示
    if (m_pBack)
    {
        auto sprite = m_pBack->GetComponent<CSpriteRenderComponent>();

        sprite->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
    }

    // 文字表示
    if (m_pFontTex)
    {
        auto sprite = m_pFontTex->GetComponent<CSpriteRenderComponent>();

        sprite->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
    }
}

//=============================================================================
// 全文表示
//=============================================================================
void CTextSprite::ShowAll(void)
{
    m_visibleCount = (int)m_displayText.size();

    m_finished = true;

    UpdateTexture();
}

//=============================================================================
// 表示中かどうか
//=============================================================================
bool CTextSprite::IsTyping(void) const
{
    return !m_finished;
}

//=============================================================================
// テキスト非表示
//=============================================================================
void CTextSprite::Hide(void)
{
    if (m_pBack)
    {
        auto sprite = m_pBack->GetComponent<CSpriteRenderComponent>();

        sprite->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));
    }

    if (m_pFontTex)
    {
        auto sprite = m_pFontTex->GetComponent<CSpriteRenderComponent>();

        sprite->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));
    }
}

//=============================================================================
// 更新処理
//=============================================================================
void CTextSprite::Update(void)
{
    m_timer += CManager::Instance()->GetTimeSystem()->GetDeltaTime();

    // 一定間隔で表示していく
    if (m_timer >= m_visibleSpeed)
    {
        m_timer = 0.0f;

        if (m_visibleCount < (int)m_displayText.size())
        {
            m_visibleCount++;

            UpdateTexture();

            if (m_visibleCount >= (int)m_displayText.size())
            {
                m_finished = true;
            }
        }
        else
        {
            m_finished = true;
        }
    }
}

//=============================================================================
// タグ解析関数
//=============================================================================
std::vector<CTextSprite::CHAR_INFO> CTextSprite::ParseColorTags(const std::string& text)
{
    std::vector<CHAR_INFO> chars;

    D3DCOLOR currentColor = D3DCOLOR_ARGB(255, 0, 0, 0);

    std::string tag;

    for (size_t i = 0; i < text.size();)
    {
        if (text[i] == '<')
        {
            tag.clear();

            while (i < text.size())
            {
                tag += text[i];

                if (text[i] == '>')
                {
                    break;
                }

                i++;
            }

            // 赤色
            if (tag == "<red>")
            {
                currentColor = D3D::RED;
            }
            // オレンジ色
            else if (tag == "<orange>")
            {
                currentColor = D3D::ORANGE;
            }
            // 青色
            else if (tag == "<dodgerblue>")
            {
                currentColor = D3DCOLOR_ARGB(255, 30, 144, 255);
            }
            // 紫色
            else if (tag == "<darkorchid>")
            {
                currentColor = D3DCOLOR_ARGB(255, 153, 50, 204);
            }
            // 灰色色
            else if (tag == "<gray>")
            {
                currentColor = D3D::GRAY;
            }
            else if (tag == "</red>" ||
                tag == "</orange>" ||
                tag == "</dodgerblue>" ||
                tag == "</darkorchid>" ||
                tag == "</gray>")
            {
                currentColor = D3DCOLOR_ARGB(255, 0, 0, 0);
            }

            i++;
            continue;
        }

        CHAR_INFO info;
        info.color = currentColor;

        // Shift-JIS 2バイト文字
        if (IsDBCSLeadByte((BYTE)text[i]))
        {
            info.text += text[i];
            info.text += text[i + 1];

            i += 2;
        }
        else
        {
            info.text += text[i];
            i++;
        }

        chars.push_back(info);
    }

    return chars;
}

//=============================================================================
// タグ除去関数(表示しない)
//=============================================================================
std::string CTextSprite::RemoveTags(const std::string& text)
{
    std::string result;

    bool inTag = false;

    for (char c : text)
    {
        if (c == '<')
        {
            inTag = true;
            continue;
        }

        if (c == '>')
        {
            inTag = false;
            continue;
        }

        if (!inTag)
        {
            result += c;
        }
    }

    return result;
}

//=============================================================================
// タグを考慮して表示文字列を取得
//=============================================================================
std::string CTextSprite::GetVisibleTaggedText(int visibleCount)
{
    std::string result;

    int visible = 0;
    bool inTag = false;

    for (size_t i = 0; i < m_text.size(); i++)
    {
        char c = m_text[i];

        // タグ開始
        if (c == '<')
        {
            inTag = true;
        }

        // タグ中は表示文字数に数えない
        if (inTag)
        {
            result += c;

            if (c == '>')
            {
                inTag = false;
            }

            continue;
        }

        // 表示文字数上限
        if (visible >= visibleCount)
        {
            break;
        }

        result += c;
        visible++;
    }

    return result;
}

//=============================================================================
// 位置設定処理
//=============================================================================
void CTextSprite::SetPos(D3DXVECTOR3 pos)
{
    m_worldPos = pos;

    if (m_pBack)
    {
        auto transform = m_pBack->GetComponent<CTransformComponent>();

        transform->Position = pos;
    }

    if (m_pFontTex)
    {
        auto transform = m_pFontTex->GetComponent<CTransformComponent>();

        transform->Position = pos;
    }
}

//=============================================================================
// 終了判定処理
//=============================================================================
bool CTextSprite::IsFinished(void) const
{
    return m_finished;
}

//=============================================================================
// デバイスロスト時
//=============================================================================
void CTextSprite::OnLostDevice(void)
{
    if (m_pFont)
    {
        m_pFont->OnLostDevice();
    }

    if (m_pSprite)
    {
        m_pSprite->OnLostDevice();
    }

    //if (m_pTexture)
    //{
    //    m_pTexture->Release();
    //    m_pTexture = nullptr;
    //}

    if (m_pRenderSurface != nullptr)
    {
        m_pRenderSurface->Release();
        m_pRenderSurface = nullptr;
    }

}

//=============================================================================
// デバイスリセット時
//=============================================================================
void CTextSprite::OnResetDevice(void)
{
    auto device = CManager::Instance()->GetRenderer()->GetDevice();

    if (m_pFont)
    {
        m_pFont->OnResetDevice();
    }

    if (m_pSprite)
    {
        m_pSprite->OnResetDevice();
    }

    device->CreateTexture(
        512,
        128,
        1,
        D3DUSAGE_RENDERTARGET,
        D3DFMT_A8R8G8B8,
        D3DPOOL_DEFAULT,
        &m_pTexture->Texture,
        nullptr);

    m_pTexture->Texture->GetSurfaceLevel(
        0,
        &m_pRenderSurface);

    if (m_pFontTex)
    {
        auto sprite = m_pFontTex->GetComponent<CSpriteRenderComponent>();

        sprite->SetTexture(m_pTexture);
    }

    UpdateTexture();
}