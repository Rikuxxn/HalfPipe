//=============================================================================
//
// UIテキストコンポーネント処理 [UITextComponent.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "UITextComponent.h"
#include "Manager.h"
#include "Renderer.h"
#include "TimeSystem.h"
#include "Scene.h"
#include "SceneManager.h"
#include "ColorConstants.h"
#include "TextureResource.h"
#include "RenderTarget.h"
#include "GameObject.h"
#include <SpriteRenderComponent.h>
#include <RenderTargetManager.h>
#include <DeviceResourceManager.h>


// 名前空間の使用
using namespace ColorConst;

unsigned int CUITextComponent::s_NextID = 0;

//=============================================================================
// コンストラクタ
//=============================================================================
CUITextComponent::CUITextComponent() :
    m_pFont         (nullptr),                  // フォント
    m_pSprite       (nullptr),                  // スプライト
    m_pRenderTarget (nullptr),                  // レンダーターゲット
    m_Color         (D3D::WHITE),               // 色
    m_FontSize      (DEFAULT_FONT_SIZE),        // フォントサイズ
    m_VisibleCount  (0),                        // 表示カウント
    m_Timer         (0.0f),                     // タイマー
    m_TypingSpeed   (0.05f),                    // 表示スピード
    m_IsFinished    (true),                     // 表示し終えたか
    m_NeedsRedraw   (false)                     // 再描画が必要かどうか
{

}

//=============================================================================
// デストラクタ
//=============================================================================
CUITextComponent::~CUITextComponent()
{
    Uninit();
}

//=============================================================================
// フォント生成処理
//=============================================================================
HRESULT CUITextComponent::CreateFont(int size)
{
    // デバイスの取得
    auto device = CManager::Instance()->GetRenderer()->GetDevice();

    if (m_pFont)
    {
        m_pFont->Release();
        m_pFont = nullptr;
    }

    // フォントサイズの設定
    m_FontSize = size;

    return D3DXCreateFont(
        device,
        m_FontSize,
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
HRESULT CUITextComponent::Init(void)
{
    // フォント生成
    HRESULT hr = CreateFont(m_FontSize);

    if (FAILED(hr))
    {
        return E_FAIL;
    }

    auto device = CManager::Instance()->GetRenderer()->GetDevice();

    // スプライト生成
    hr = D3DXCreateSprite(device, &m_pSprite);
    if (FAILED(hr))
    {
        MessageBox(nullptr, "D3DXCreateSprite Failed!", "Error", MB_OK);
        return E_FAIL;
    }

    // オブジェクトごとに固有IDを割り当て
    m_ID = s_NextID++;

    m_RenderTargetName = "TextUI_" + std::to_string(m_ID);

    // RenderTarget生成
    m_pRenderTarget = CRenderTargetManager::GetInstance().Create(
        m_RenderTargetName,
        512,
        128,
        D3DFMT_A8R8G8B8,
        D3DFMT_UNKNOWN);

    if (!m_pRenderTarget)
    {
        return E_FAIL;
    }

    // テクスチャリソース作成
    m_Texture = std::make_shared<CTextureResource>();

    // レンダーターゲットのテクスチャを設定
    m_Texture->Texture = m_pRenderTarget->GetTexture();

    // オーナーのスプライトにテクスチャを設定
    auto sprite = m_pOwner->GetComponent<CSpriteRenderComponent>();

    if (sprite)
    {
        sprite->SetTexture(m_Texture);
        sprite->SetSize(0.5f, 0.15f);
        sprite->SetLayer(RenderLayer::UI);
    }

    // デバイスリソース管理へ登録
    CDeviceResourceManager::GetInstance().Register(this);

    return S_OK;
}

//=============================================================================
// 更新処理
//=============================================================================
void CUITextComponent::Update(void)
{
    if (m_NeedsRedraw)
    {
        UpdateTexture();
        m_NeedsRedraw = false;
    }

    if (m_IsFinished)
    {
        return;
    }

    m_Timer += CManager::Instance()->GetTimeSystem()->GetDeltaTime();

    // 一定間隔で表示していく
    if (m_Timer >= m_TypingSpeed)
    {
        m_Timer = 0.0f;

        if (m_VisibleCount < (int)m_DisplayText.size())
        {
            m_VisibleCount++;

            UpdateTexture();

            if (m_VisibleCount >= (int)m_DisplayText.size())
            {
                m_IsFinished = true;
            }
        }
        else
        {
            m_IsFinished = true;
        }
    }
}

//=============================================================================
// テクスチャ更新処理
//=============================================================================
void CUITextComponent::UpdateTexture(void)
{
    if (!m_pFont || !m_pRenderTarget)
    {
        return;
    }

    auto device = CManager::Instance()->GetRenderer()->GetDevice();

    // テクスチャのバインドを解除
    device->SetTexture(0, nullptr);

    // レンダーターゲット開始
    m_pRenderTarget->Begin(D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0));

    // タグを考慮した文字列を返す
    std::string drawText = GetVisibleTaggedText(m_VisibleCount);

    auto chars = ParseColorTags(drawText);

    // 初期位置
    int x = 5;
    int y = 20;

    const int lineHeight = m_FontSize;

    HRESULT hr = m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);

    if (FAILED(hr))
    {
        OutputDebugStringA("Sprite Begin Failed\n");
    }

    for (auto& ch : chars)
    {
        if (ch.text == "\n")
        {
            x = 5;
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

    // レンダーターゲット終了
    m_pRenderTarget->End();
}

//=============================================================================
// 終了処理
//=============================================================================
void CUITextComponent::Uninit(void)
{
    // デバイスリソースマネージャーから登録解除
    CDeviceResourceManager::GetInstance().Unregister(this);

    // テクスチャリソースを解放
    m_Texture.reset();

    // RenderTargetを解放
    if (!m_RenderTargetName.empty())
    {
        CRenderTargetManager::GetInstance().Release(m_RenderTargetName);
        m_RenderTargetName.clear();
    }

    // フォントを解放
    if (m_pFont)
    {
        m_pFont->Release();
        m_pFont = nullptr;
    }

    // スプライト破棄
    if (m_pSprite)
    {
        m_pSprite->Release();
        m_pSprite = nullptr;
    }
}

//=============================================================================
// 表示開始処理
//=============================================================================
void CUITextComponent::SetText(const std::string& text)
{
    m_Text = text;

    // タグを除去した文字列
    m_DisplayText = RemoveTags(text);

    // 初期化
    m_VisibleCount = 0;
    m_Timer = 0.0f;
    m_IsFinished = false;

    // テキストを再描画する
    UpdateTexture();
}

//=============================================================================
// 全文表示
//=============================================================================
void CUITextComponent::ShowAll(void)
{
    m_VisibleCount = (int)m_DisplayText.size();

    m_IsFinished = true;

    UpdateTexture();
}

//=============================================================================
// フォントサイズ設定
//=============================================================================
void CUITextComponent::SetFontSize(int size)
{
    if (m_FontSize == size)
    {
        return;
    }

    if (SUCCEEDED(CreateFont(size)))
    {
        UpdateTexture();
    }
}

//=============================================================================
// 文字色設定
//=============================================================================
void CUITextComponent::SetColor(D3DCOLOR color)
{
    m_Color = color;

    UpdateTexture();
}

//=============================================================================
// 表示速度設定
//=============================================================================
void CUITextComponent::SetTypingSpeed(float speed)
{
    m_TypingSpeed = speed;
}

//=============================================================================
// 表示中かどうか
//=============================================================================
bool CUITextComponent::IsTyping(void) const
{
    return !m_IsFinished;
}

//=============================================================================
// 終了判定処理
//=============================================================================
bool CUITextComponent::IsFinished(void) const
{
    return m_IsFinished;
}

//=============================================================================
// テキスト非表示
//=============================================================================
void CUITextComponent::Hide(void)
{
    m_VisibleCount = 0;

    m_IsFinished = true;

    UpdateTexture();
}

//=============================================================================
// タグ解析関数
//=============================================================================
std::vector<CUITextComponent::CHAR_INFO> CUITextComponent::ParseColorTags(const std::string& text)
{
    std::vector<CHAR_INFO> chars;

    D3DCOLOR currentColor = D3D::BLACK;

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
std::string CUITextComponent::RemoveTags(const std::string& text)
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
std::string CUITextComponent::GetVisibleTaggedText(int visibleCount)
{
    std::string result;

    int visible = 0;
    bool inTag = false;

    for (size_t i = 0; i < m_Text.size(); i++)
    {
        char c = m_Text[i];

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
// デバイスロスト時
//=============================================================================
void CUITextComponent::OnLostDevice(void)
{
    // フォント
    if (m_pFont)
    {
        m_pFont->OnLostDevice();
    }

    // スプライト
    if (m_pSprite)
    {
        m_pSprite->OnLostDevice();
    }

    // テキスト側のテクスチャ参照を解除
    if (m_Texture)
    {
        m_Texture->Texture.Reset();
    }
}

//=============================================================================
// デバイスリセット時
//=============================================================================
void CUITextComponent::OnResetDevice(void)
{
    if (m_pFont)
    {
        m_pFont->OnResetDevice();
    }

    if (m_pSprite)
    {
        m_pSprite->OnResetDevice();
    }

    if (!m_pRenderTarget || !m_Texture)
    {
        return;
    }

    // 再生成されたレンダーターゲットのテクスチャを設定
    m_Texture->Texture = m_pRenderTarget->GetTexture();

    auto sprite = m_pOwner->GetComponent<CSpriteRenderComponent>();

    if (sprite)
    {
        sprite->SetTexture(m_Texture);
    }

    // 再描画フラグ
    m_NeedsRedraw = true;
}

//=============================================================================
// リソースの破棄
//=============================================================================
void CUITextComponent::ReleaseResource(void)
{
    Uninit();
}