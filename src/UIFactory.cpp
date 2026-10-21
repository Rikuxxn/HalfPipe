//=============================================================================
//
// UIファクトリー処理 [UIFactory.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "UIFactory.h"
#include "Scene.h"
#include "GameObject.h"
#include "UIComponent.h"
#include "SpriteRenderComponent.h"
#include "UIAnimationComponent.h"

//=============================================================================
// UI生成
//=============================================================================
CGameObject* CUIFactory::Create(CScene* scene, const UIAnimationData& data)
{
    if (!scene)
    {
        return nullptr;
    }

    // UIオブジェクト生成
    auto object = scene->CreateObject();

    if (!object)
    {
        return nullptr;
    }

    // タグ設定
    object->SetTag(data.ObjectTag);

    //---------------------------------
    // UIコンポーネント
    //---------------------------------

    auto ui = object->AddComponent<CUIComponent>();

    auto& transform = ui->GetTransform();

    transform.Position = data.Position;
    transform.Scale = data.Scale;
    transform.Alpha = data.Alpha;

    //---------------------------------
    // スプライトコンポーネント
    //---------------------------------

    auto sprite = object->AddComponent<CSpriteRenderComponent>();

    sprite->SetLayer(RenderLayer::UI);
    sprite->SetTexture(data.TexturePath);
    sprite->SetSize(data.Width, data.Height);

    //---------------------------------
    // アニメーションコンポーネント
    //---------------------------------

    if (!data.Animations.empty())
    {
        auto animation = object->AddComponent<CUIAnimationComponent>();

        for (const auto& clip : data.Animations)
        {
            animation->AddAnimation(clip);
        }

        animation->Play();
    }

    return object;
}

//=============================================================================
// 基本設定の反映
//=============================================================================
void CUIFactory::ApplyTransform(CGameObject* object, const UIAnimationData& data)
{
    if (!object)
    {
        return;
    }

    auto ui = object->GetComponent<CUIComponent>();
    auto sprite = object->GetComponent<CSpriteRenderComponent>();

    if (ui)
    {
        auto& transform = ui->GetTransform();

        transform.Position = data.Position;
        transform.Scale = data.Scale;
        transform.Alpha = data.Alpha;
        transform.UVPosition = data.UVPosition;
        transform.UVSize = data.UVSize;
    }

    if (sprite)
    {
        sprite->SetTexture(data.TexturePath);
        sprite->SetSize(data.Width, data.Height);
    }
}

//=============================================================================
// アニメーション設定の反映
//=============================================================================
void CUIFactory::ApplyAnimations(CGameObject* object, const UIAnimationData& data)
{
    if (!object)
    {
        return;
    }

    // UIアニメーションコンポーネントの取得
    auto animation = object->GetComponent<CUIAnimationComponent>();

    if (!animation)
    {
        if (data.Animations.empty())
        {
            return;
        }

        // 無かったら追加する
        animation = object->AddComponent<CUIAnimationComponent>();
    }

    animation->SetAnimations(data.Animations, true);
}