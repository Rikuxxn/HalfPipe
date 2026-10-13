//=============================================================================
//
// ポーズマネージャー処理 [PauseManager.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "PauseManager.h"
#include "PauseItemComponent.h"
#include "Manager.h"
#include "Fade.h"
#include "Game.h"
#include "SceneManager.h"
#include "Scene.h"
#include "TransformComponent.h"
#include "Renderer.h"
#include "SpriteRenderComponent.h"
#include "Input.h"
#include "InputManager.h"


//=============================================================================
// コンストラクタ
//=============================================================================
CPauseManager::CPauseManager()
{

}

//=============================================================================
// デストラクタ
//=============================================================================
CPauseManager::~CPauseManager()
{

}

//=============================================================================
// 項目生成
//=============================================================================
void CPauseManager::CreateItem(const char* path, float x, float y, float w, float h, std::function<void()> func)
{
    auto scene = m_Owner;

    auto obj = scene->CreateObject();

    auto transform = obj->AddComponent<CTransformComponent>();

    transform->Position = { x, y, 0.0f };

    auto sprite = obj->AddComponent<CSpriteRenderComponent>();

    // 描画順の設定
    sprite->SetLayer(RenderLayer::Pause);

    // テクスチャの設定
    sprite->SetTexture(path);

    // サイズの設定
    sprite->SetSize(w, h);

    sprite->SetIsVisible(false);

    auto pause = obj->AddComponent<CPauseItemComponent>();

    pause->OnExecute = func;

    m_Items.push_back(
        {
            obj,
            pause
        });
}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CPauseManager::Init(CGame* owner)
{
    m_Owner = owner;

    //---------------------------------
    // 背景生成
    //---------------------------------
    
    // ポーズ背景オブジェクトの生成
    auto back = m_Owner->CreateObject();

    back->SetTag("PauseBack");
    
    // トランスフォームコンポーネントに追加
    auto transform = back->AddComponent<CTransformComponent>();

    // 位置を設定
    transform->Position = { 0.5f,0.5f,0.0f };

    // スプライトレンダーコンポーネントに追加
    auto sprite = back->AddComponent<CSpriteRenderComponent>();

    // 描画順の設定
    sprite->SetLayer(RenderLayer::Pause);

    // テクスチャの設定
    sprite->SetTexture("data/TEXTURE/.png");

    // サイズの設定
    sprite->SetSize(1920.0f, 1080.0f);

    // カラーの設定
    sprite->SetColor({0.0f, 0.0f, 0.0f, 0.5f});

    sprite->SetIsVisible(false);

    CreateItem(
        "data/TEXTURE/continue.png",
        0.5f,
        0.35f,
        300.0f,
        160.0f,
        [this]()
        {
            // 続ける
            m_Owner->SetEnablePause(false);
        });

    CreateItem(
        "data/TEXTURE/retry.png",
        0.5f,
        0.55f,
        300.0f,
        160.0f,
        []()
        {
            // ゲーム画面に遷移
            CManager::Instance()->GetFade()->StartFadeOut(CScene::Mode::Game);
        });

    CreateItem(
        "data/TEXTURE/quit.png",
        0.5f,
        0.75f,
        300.0f,
        160.0f,
        []()
        {
            // タイトル画面に遷移
            CManager::Instance()->GetFade()->StartFadeOut(CScene::Mode::Title);
        });

    for (auto& item : m_Items)
    {
        item.Component->Selected = false;
    }

    m_SelectIndex = 0;

    m_Items[0].Component->Selected = true;

    return S_OK;
}

//=============================================================================
// 更新処理
//=============================================================================
void CPauseManager::Update(void)
{
    if (!m_Enable)
    {
        return;
    }

    auto keyboard = CManager::Instance()->GetInput()->GetKeyboard();

    //---------------------------------
    // 選択移動
    //---------------------------------

    int old = m_SelectIndex;

    if (keyboard->GetTrigger(DIK_UP))
    {
        m_SelectIndex--;
    }

    if (keyboard->GetTrigger(DIK_DOWN))
    {
        m_SelectIndex++;
    }

    //---------------------------------
    // ループ
    //---------------------------------

    if (m_SelectIndex < 0)
    {
        m_SelectIndex = (int)m_Items.size() - 1;
    }

    if (m_SelectIndex >= m_Items.size())
    {
        m_SelectIndex = 0;
    }

    //---------------------------------
    // 選択状態更新
    //---------------------------------

    if (old != m_SelectIndex)
    {
        m_Items[old].Component->Selected = false;
        m_Items[m_SelectIndex].Component->Selected = true;
    }

    //---------------------------------
    // 決定
    //---------------------------------

    if (keyboard->GetTrigger(DIK_RETURN))
    {
        auto func = m_Items[m_SelectIndex].Component->OnExecute;

        if (func)
        {
            func();
        }
    }

    //---------------------------------
    // アイテム更新
    //---------------------------------

    for (auto& item : m_Items)
    {
        item.Object->Update();
    }
}

//=============================================================================
// 描画処理
//=============================================================================
void CPauseManager::Draw(void)
{
    if (!m_Enable)
    {
        return;
    }
}

//=============================================================================
// 使用処理
//=============================================================================
void CPauseManager::SetEnable(bool enable)
{
    m_Enable = enable;

    //---------------------------------
    // 背景
    //---------------------------------

    auto back = m_Owner->FindGameObject("PauseBack");

    if (back)
    {
        auto sprite = back->GetComponent<CSpriteRenderComponent>();

        if (sprite)
        {
            sprite->SetIsVisible(enable);
        }
    }

    //---------------------------------
    // 項目
    //---------------------------------

    for (auto& item : m_Items)
    {
        auto sprite = item.Object->GetComponent<CSpriteRenderComponent>();

        if (sprite)
        {
            sprite->SetIsVisible(enable);
        }
    }
}