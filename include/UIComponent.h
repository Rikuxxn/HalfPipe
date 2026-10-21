//=============================================================================
//
// UIコンポーネント処理 [UIComponent.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _UI_COMPONENT_H_// このマクロ定義がされていなかったら
#define _UI_COMPONENT_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Component.h"
#include "Easing.h"

//*****************************************************************************
// UIの構造体
//*****************************************************************************
struct UITransform
{
    D3DXVECTOR2 Position = { 0.5f, 0.5f };   // 位置(画面比率)
    D3DXVECTOR2 Size = { 0.5f, 0.5f };   // サイズ(画面比率)
    D3DXVECTOR2 Scale = { 1.0f, 1.0f };   // 拡縮率

    D3DXCOLOR Color = D3DXCOLOR(1, 1, 1, 1);

    float Rotation = 0.0f;
    float Alpha = 1.0f;

    // UV座標
    D3DXVECTOR2 UVPosition = { 0.0f, 0.0f };

    // UVサイズ
    D3DXVECTOR2 UVSize = { 1.0f, 1.0f };
};

//*****************************************************************************
// UIのアニメーション対象の種類
//*****************************************************************************
enum class UIProperty
{
    PositionX,
    PositionY,
    ScaleX,
    ScaleY,
    Rotation,
    Alpha,
    ColorR,
    ColorG,
    ColorB
};

//*****************************************************************************
// アニメーション再生方法
//*****************************************************************************
enum class UIPlaybackMode
{
    Once,       // 1回だけ再生
    Loop,       // 終了したら最初に戻る
    PingPong    // 往復を繰り返す
};

//*****************************************************************************
// 1つのアニメーション構造体
//*****************************************************************************
struct UIAnimationClip
{
    std::string Name = "NewAnimation";

    UIProperty Property = UIProperty::Alpha;

    float From = 0.0f;
    float To = 1.0f;

    float Duration = 1.0f;
    float Delay = 0.0f;

    UIPlaybackMode PlaybackMode = UIPlaybackMode::Once;

    // 0     : 無限ループ
    // 1以上 : 指定回数だけ再生
    int LoopCount = 1;

    // イージング関数
    float (*Easing)(float) = CEasing::Linear;
};

//*****************************************************************************
// アニメーション再生状態
//*****************************************************************************
struct UIAnimationState
{
    UIAnimationClip Clip;

    float ElapsedTime = 0.0f;

    int CompletedLoops = 0;

    bool IsPlaying = false;
    bool IsFinished = false;

    // PingPong用
    bool IsReversing = false;
};

//*****************************************************************************
// UIコンポーネントクラス
//*****************************************************************************
class CUIComponent : public CComponent
{
private:

    UITransform m_Transform;

public:

    CUIComponent() = default;
    ~CUIComponent() override = default;

    // UI情報の取得
    UITransform& GetTransform(void) { return m_Transform; }

    const UITransform& GetTransform(void) const { return m_Transform; }

    // 位置
    void SetPosition(const D3DXVECTOR2& pos) { m_Transform.Position = pos; }

    // サイズ
    void SetSize(const D3DXVECTOR2& size) { m_Transform.Size = size; }

    // 拡大率
    void SetScale(const D3DXVECTOR2& scale) { m_Transform.Scale = scale; }

    // 回転
    void SetRotation(float rotation) { m_Transform.Rotation = rotation; }

    // 色
    void SetColor(const D3DXCOLOR& color) { m_Transform.Color = color; }

    // 透明度
    void SetAlpha(float alpha) { m_Transform.Alpha = alpha; }

    // UV座標
    void SetUVPosition(const D3DXVECTOR2& pos) { m_Transform.UVPosition = pos; }

    // UVサイズ
    void SetUVSize(const D3DXVECTOR2& size) { m_Transform.UVSize = size; }

    // UV座標とサイズをまとめて設定
    void SetUV(const D3DXVECTOR2& pos, const D3DXVECTOR2& size)
    {
        m_Transform.UVPosition = pos;
        m_Transform.UVSize = size;
    }

    // 指定したフレームを表示
    // @param frameIndex : 何枚目を表示するか
    // @param frameCount : そのテクスチャの枚数
    void SetUVFrame(int frameIndex, int frameCount);
};

#endif