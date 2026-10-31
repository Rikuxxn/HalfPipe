//=============================================================================
//
// カウント描画コンポーネント処理 [CountRenderComponent.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "CountRenderComponent.h"
#include "GameObject.h"
#include "UIComponent.h"
#include "Manager.h"
#include "Renderer.h"
#include "TextureResource.h"
#include "MathConstants.h"
#include "TimeSystem.h"


// 名前空間の使用
using namespace MathConst;


//=============================================================================
// コンストラクタ
//=============================================================================
CCountRenderComponent::CCountRenderComponent() :
    m_CountType     (CountType::Add),   // カウントの種類
    m_Count         (0),                // カウント
    m_TargetCount   (0),                // 目標カウント
    m_CountSpeed    (200.0f),           // 1秒あたりの加算ポイント数
    m_CountRemainder(0.0f),             // 加算量の小数部分を保持 
    m_DigitWidth    (50.0f),            // 1文字当たりの幅
    m_DigitHeight   (50.0f),            // 1文字当たりの高さ
    m_IsVisible     (true)              // 表示するかどうか
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CCountRenderComponent::Init(void)
{
    // デバイスの取得
    auto device = CManager::Instance()->GetRenderer()->GetDevice();

    // 頂点バッファの作成
    HRESULT hr = device->CreateVertexBuffer(
        sizeof(VERTEX_2D) * VERTEX,
        0,
        FVF_VERTEX_2D,
        D3DPOOL_MANAGED,
        m_pVtxBuff.GetAddressOf(),
        nullptr
    );

    if (FAILED(hr))
    {
        return hr;
    }

    return S_OK;
}

//=============================================================================
// 更新処理
//=============================================================================
void CCountRenderComponent::Update(void)
{
    if (!m_IsVisible)
    {
        return;
    }

    // デルタタイムの取得
    float deltaTime = CManager::Instance()->GetTimeSystem()->GetDeltaTime();

    if (m_CountType == CountType::Add)
    {
        // 目標カウントに到達している場合
        if (m_Count >= m_TargetCount)
        {
            m_Count = m_TargetCount;
            m_CountRemainder = 0.0f;
            return;
        }

        // このフレームで加算する量
        float addAmount = m_CountSpeed * deltaTime;

        // 小数部分を保持して、低フレームレートでも加算が止まらないようにする
        m_CountRemainder += addAmount;

        int addValue = static_cast<int>(m_CountRemainder);

        if (addValue <= 0)
        {
            return;
        }

        // 加算した整数部分だけ残りの小数を保持
        m_CountRemainder -= static_cast<float>(addValue);

        // 目標カウントを超えないように加算
        int remaining = m_TargetCount - m_Count;

        if (addValue >= remaining)
        {
            m_Count = m_TargetCount;
            m_CountRemainder = 0.0f;
        }
        else
        {
            m_Count += addValue;
        }
    }
    else if (m_CountType == CountType::Dec)
    {
        // 目標カウントに到達している場合
        if (m_Count <= m_TargetCount)
        {
            m_Count = m_TargetCount;
            m_CountRemainder = 0.0f;
            return;
        }

        // このフレームで加算する量
        float addAmount = m_CountSpeed * deltaTime;

        // 小数部分を保持して、低フレームレートでも加算が止まらないようにする
        m_CountRemainder += addAmount;

        int addValue = static_cast<int>(m_CountRemainder);

        if (addValue <= 0)
        {
            return;
        }

        // 加算した整数部分だけ残りの小数を保持
        m_CountRemainder -= static_cast<float>(addValue);

        // 目標カウントを超えないように加算
        int remaining = m_TargetCount - m_Count;

        if (addValue >= remaining)
        {
            m_Count = m_TargetCount;
            m_CountRemainder = 0.0f;
        }
        else
        {
            m_Count -= addValue;
        }
    }
}

//=============================================================================
// 描画処理
//=============================================================================
void CCountRenderComponent::Draw(void)
{
    if (!m_IsVisible || !m_pOwner || !m_pVtxBuff)
    {
        return;
    }

    //// テクスチャが設定されていなければ描画しない
    //if (!m_TextureResource || !m_TextureResource->Texture)
    //{
    //    return;
    //}

    // UIコンポーネントの取得
    auto ui = m_pOwner->GetComponent<CUIComponent>();

    if (!ui)
    {
        return;
    }

    const auto& transform = ui->GetTransform();

    // デバイスの取得
    auto device = CManager::Instance()->GetRenderer()->GetDevice();

    // 画面サイズ
    float screenW = (float)CManager::Instance()->GetRenderer()->GetBackBufferWidth();
    float screenH = (float)CManager::Instance()->GetRenderer()->GetBackBufferHeight();

    // スコアを文字列に変換
    std::string scoreText = std::to_string(m_Count);

    // 1文字の描画サイズ
    float w = screenW * m_DigitWidth * transform.Scale.x;
    float h = screenH * m_DigitHeight * transform.Scale.y;

    // UIの中心位置
    float centerX = transform.Position.x * screenW;
    float centerY = transform.Position.y * screenH;

    //// 左端から描画する
    //float startX = centerX;

    // スコア全体の幅
    float totalWidth = scoreText.size() * w;

    // 右端を固定して、左から描画する
    float startX = centerX - totalWidth;

    // 色と透明度
    D3DXCOLOR color = transform.Color;
    color.a *= transform.Alpha;

    // 各数字を描画
    for (size_t i = 0; i < scoreText.size(); i++)
    {
        // 文字を数字に変換
        int digit = scoreText[i] - '0';

        // UVを計算
        float u0 = digit / 10.0f;
        float u1 = (digit + 1) / 10.0f;

        float v0 = 0.0f;
        float v1 = 1.0f;

        // 描画位置
        float left = startX + i * w;
        float right = left + w;

        float top = centerY - h * MathConst::HALF;
        float bottom = centerY + h * MathConst::HALF;

        // 頂点バッファをロック
        VERTEX_2D* vertex = nullptr;
        HRESULT hr = m_pVtxBuff->Lock(0, 0, (void**)&vertex, 0);

        if (FAILED(hr))
        {
            continue;
        }

        // 頂点位置
        vertex[0].pos = { left,  top,    0.0f };
        vertex[1].pos = { right, top,    0.0f };
        vertex[2].pos = { left,  bottom, 0.0f };
        vertex[3].pos = { right, bottom, 0.0f };

        // 頂点設定
        for (int j = 0; j < MathConst::VERTEX; j++)
        {
            vertex[j].rhw = 1.0f;
            vertex[j].col = color;
        }

        // UV設定
        vertex[0].tex = { u0, v0 };
        vertex[1].tex = { u1, v0 };
        vertex[2].tex = { u0, v1 };
        vertex[3].tex = { u1, v1 };

        // 頂点バッファをアンロック
        m_pVtxBuff->Unlock();

        // テクスチャ設定
        device->SetTexture(0, m_TextureResource ? m_TextureResource->Texture.Get() : nullptr);

        // 頂点バッファをデータストリームに設定
        device->SetStreamSource(0, m_pVtxBuff.Get(), 0, sizeof(VERTEX_2D));

        // 頂点フォーマット
        device->SetFVF(FVF_VERTEX_2D);

        // ポリゴンの描画
        device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
    }
}

//=============================================================================
// 1文字のサイズ設定
//=============================================================================
void CCountRenderComponent::SetDigitSize(float width, float height)
{
    m_DigitWidth = width;
    m_DigitHeight = height;
}

//=============================================================================
// カウントの直接設定
//=============================================================================
void CCountRenderComponent::SetCount(int count)
{
    m_Count = std::max(0, count);

    m_TargetCount = m_Count;

    // 加算途中の小数部分をリセット
    m_CountRemainder = 0.0f;
}

//=============================================================================
// 加算速度設定
//=============================================================================
void CCountRenderComponent::SetCountSpeed(float speed)
{
    m_CountSpeed = std::max(0.0f, speed);
}

//=============================================================================
// カウント加算
//=============================================================================
void CCountRenderComponent::AddCount(int count)
{
    if (count <= 0)
    {
        return;
    }

    // 目標スコアを加算
    m_TargetCount += count;
}

//=============================================================================
// カウント減算
//=============================================================================
void CCountRenderComponent::DecCount(int count)
{
    if (count <= 0)
    {
        return;
    }

    // 目標カウントを減算
    m_TargetCount -= count;
}