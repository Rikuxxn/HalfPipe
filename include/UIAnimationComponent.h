//=============================================================================
//
// UIアニメーションコンポーネント処理 [UIAnimationComponent.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _UI_ANIMATION_COMPONENT_H_// このマクロ定義がされていなかったら
#define _UI_ANIMATION_COMPONENT_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Component.h"
#include "UIComponent.h"


//*****************************************************************************
// UIアニメーションコンポーネントクラス
//*****************************************************************************
class CUIAnimationComponent : public CComponent
{
private:

    std::vector<UIAnimationState> m_Animations;

    CUIComponent* m_pUI = nullptr;

public:

    CUIAnimationComponent() = default;
    ~CUIAnimationComponent() override = default;

    HRESULT Init(void) override;
    void    Update(void) override;

    // アニメーション追加
    void AddAnimation(const UIAnimationClip& clip);

    // アニメーションをすべてクリア
    void ClearAnimations(void);

    // 全アニメーション再生
    void Play(void);

    // 指定したアニメーションだけ再生
    void PlayAnimation(const std::string& name);

    // 全アニメーション停止
    void Stop(void);

    // アニメーション設定を置き換える
    void SetAnimations(
        const std::vector<UIAnimationClip>& clips,
        bool play = true);

private:

    // アニメーション更新
    void UpdateAnimation(
        UIAnimationState& animation,
        float deltaTime);

    // 値の適用
    void ApplyValue(
        UIAnimationState& animation,
        float value);
};

#endif
