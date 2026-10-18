//=============================================================================
//
// UIアニメーションコンポーネント処理 [UIAnimationComponent.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "UIAnimationComponent.h"
#include "GameObject.h"
#include "Manager.h"
#include "TimeSystem.h"
#include "MathUtility.h"
#include "MathConstants.h"

// 名前空間の使用
using namespace MathUtil;
using namespace MathConst;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CUIAnimationComponent::Init(void)
{
    m_pUI = m_pOwner->GetComponent<CUIComponent>();

    if (!m_pUI)
    {
        return E_FAIL;
    }

    return S_OK;
}

//=============================================================================
// 更新処理
//=============================================================================
void CUIAnimationComponent::Update(void)
{
    if (!m_pUI)
    {
        return;
    }

    // デルタタイムの取得
    float deltaTime = CManager::Instance()->GetTimeSystem()->GetDeltaTime();

    for (auto& animation : m_Animations)
    {
        if (!animation.IsPlaying)
        {
            continue;
        }

        UpdateAnimation(animation, deltaTime);
    }
}

//=============================================================================
// アニメーション更新
//=============================================================================
void CUIAnimationComponent::UpdateAnimation(UIAnimationState& animation, float deltaTime)
{
    auto& clip = animation.Clip;

    // 再生時間が不正なら終了
    if (clip.Duration <= 0.0f)
    {
        ApplyValue(animation, clip.To);

        animation.IsPlaying = false;
        animation.IsFinished = true;

        return;
    }

    // 不正なデルタタイムは無視
    if (deltaTime < 0.0f)
    {
        return;
    }

    // 経過時間を更新
    animation.ElapsedTime += deltaTime;

    // 遅延時間中は再生しない
    if (animation.ElapsedTime < clip.Delay)
    {
        return;
    }

    // アニメーション開始からの経過時間
    float elapsed = animation.ElapsedTime - clip.Delay;

    // 進行度
    float t = 0.0f;

    // 終了判定
    bool finished = false;

    switch (clip.PlaybackMode)
    {
        //=================================================
        // 1回だけ再生
        //=================================================
    case UIPlaybackMode::Once:
    {
        t = elapsed / clip.Duration;

        if (t >= 1.0f)
        {
            t = 1.0f;
            finished = true;
        }

        break;
    }

    //=================================================
    // ループ再生
    //=================================================
    case UIPlaybackMode::Loop:
    {
        int completedLoops = static_cast<int>(elapsed / clip.Duration);

        // 再生回数の制限
        if (clip.LoopCount > 0 &&
            completedLoops >= clip.LoopCount)
        {
            t = 1.0f;
            finished = true;
        }
        else
        {
            float localTime = std::fmod(elapsed, clip.Duration);

            t = localTime / clip.Duration;
        }

        animation.CompletedLoops = completedLoops;

        break;
    }

    //=================================================
    // 往復再生
    //=================================================
    case UIPlaybackMode::PingPong:
    {
        // 1往復にかかる時間
        float cycleDuration = clip.Duration * F_DOUBLE;

        // 完了した往復回数
        int completedLoops = static_cast<int>(elapsed / cycleDuration);

        // 指定回数の往復が終了した場合
        if (clip.LoopCount > 0 &&
            completedLoops >= clip.LoopCount)
        {
            t = 0.0f;

            animation.IsReversing = false;
            animation.CompletedLoops = clip.LoopCount;

            finished = true;
        }
        else
        {
            // 現在の往復内の時間
            float localTime = std::fmod(elapsed, cycleDuration);

            if (localTime < clip.Duration)
            {
                // From → To
                t = localTime / clip.Duration;

                animation.IsReversing = false;
            }
            else
            {
                // To → From
                t = 1.0f - (localTime - clip.Duration) / clip.Duration;

                animation.IsReversing = true;
            }

            animation.CompletedLoops = completedLoops;
        }

        break;
    }
    }

    // 終了状態を更新
    if (finished)
    {
        animation.IsPlaying = false;
        animation.IsFinished = true;
    }

    // 進行度を制限
    t = Clamp(t, 0.0f, 1.0f);

    // イージングを適用
    if (clip.Easing)
    {
        t = clip.Easing(t);
    }

    // From～Toの値を補間
    float value = CEasing::Lerp(
        clip.From,
        clip.To,
        t);

    // UIに反映
    ApplyValue(animation, value);
}

//=============================================================================
// アニメーションの追加
//=============================================================================
void CUIAnimationComponent::AddAnimation(const UIAnimationClip& clip)
{
    UIAnimationState state;

    state.Clip = clip;

    state.ElapsedTime = 0.0f;
    state.CompletedLoops = 0;

    state.IsPlaying = false;
    state.IsFinished = false;
    state.IsReversing = false;

    m_Animations.push_back(state);
}

//=============================================================================
// アニメーションの全削除
//=============================================================================
void CUIAnimationComponent::ClearAnimations(void)
{
    m_Animations.clear();
}

//=============================================================================
// 値の適用
//=============================================================================
void CUIAnimationComponent::ApplyValue(UIAnimationState& animation, float value)
{
    auto& transform = m_pUI->GetTransform();

    switch (animation.Clip.Property)
    {
    case UIProperty::Alpha:
        transform.Alpha = value;
        break;

    case UIProperty::PositionX:
        transform.Position.x = value;
        break;

    case UIProperty::PositionY:
        transform.Position.y = value;
        break;

    case UIProperty::ScaleX:
        transform.Scale.x = value;
        break;

    case UIProperty::ScaleY:
        transform.Scale.y = value;
        break;

    case UIProperty::Rotation:
        transform.Rotation = value;
        break;

    case UIProperty::ColorR:
        transform.Color.r = value;
        break;

    case UIProperty::ColorG:
        transform.Color.g = value;
        break;

    case UIProperty::ColorB:
        transform.Color.b = value;
        break;
    }
}

//=============================================================================
// 全アニメーション再生
//=============================================================================
void CUIAnimationComponent::Play(void)
{
    // UIコンポーネントが未取得なら再取得
    if (!m_pUI && m_pOwner)
    {
        m_pUI = m_pOwner->GetComponent<CUIComponent>();
    }

    if (!m_pUI)
    {
        return;
    }

    for (auto& animation : m_Animations)
    {
        // 再生状態の初期化
        animation.ElapsedTime = 0.0f;
        animation.CompletedLoops = 0;

        animation.IsPlaying = true;
        animation.IsFinished = false;
        animation.IsReversing = false;

        // 開始値を適用
        ApplyValue(animation, animation.Clip.From);
    }
}

//=============================================================================
// 指定したアニメーションだけ再生
//=============================================================================
void CUIAnimationComponent::PlayAnimation(const std::string& name)
{
    // UIコンポーネントが未取得なら再取得
    if (!m_pUI && m_pOwner)
    {
        m_pUI = m_pOwner->GetComponent<CUIComponent>();
    }

    if (!m_pUI)
    {
        return;
    }

    // 登録されているアニメーションから名前で検索
    for (auto& animation : m_Animations)
    {
        // 指定した名前以外は再生しない
        if (animation.Clip.Name != name)
        {
            continue;
        }

        // 再生状態の初期化
        animation.ElapsedTime = 0.0f;
        animation.CompletedLoops = 0;

        animation.IsPlaying = true;
        animation.IsFinished = false;
        animation.IsReversing = false;

        // 開始値を適用
        ApplyValue(animation, animation.Clip.From);

        // 対象が見つかったので終了
        break;
    }
}

//=============================================================================
// 全アニメーション停止
//=============================================================================
void CUIAnimationComponent::Stop(void)
{
    for (auto& animation : m_Animations)
    {
        animation.IsPlaying = false;
    }
}

//=============================================================================
// アニメーション設定の置き換え
//=============================================================================
void CUIAnimationComponent::SetAnimations(
    const std::vector<UIAnimationClip>& clips,
    bool play)
{
    // 現在のアニメーションを削除
    ClearAnimations();

    // 新しいアニメーションを登録
    for (const auto& clip : clips)
    {
        AddAnimation(clip);
    }

    // 必要なら再生
    if (play && !m_Animations.empty())
    {
        Play();
    }
}