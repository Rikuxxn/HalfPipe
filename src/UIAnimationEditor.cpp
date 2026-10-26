//=============================================================================
//
// UIアニメーションエディター [UIAnimationEditor.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "UIAnimationEditor.h"
#include "UIAnimationManager.h"
#include "UIAnimationSerializer.h"
#include "imguimaneger.h"
#include "imgui_internal.h"
#include "FileDialogUtils.h"
#include "UIFactory.h"
#include "Manager.h"
#include "SceneManager.h"
#include "Scene.h"
#include "MathUtility.h"

// 名前空間の使用
using namespace MathUtil;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CUIAnimationEditor::Init(CUIAnimationManager* manager)
{
    m_Manager = manager;

    if (!m_Manager)
    {
        return E_FAIL;
    }

    return S_OK;
}

//=============================================================================
// 更新処理
//=============================================================================
void CUIAnimationEditor::Update(void)
{
    if (!m_IsOpen || !m_Manager)
    {
        return;
    }

    // ウィンドウの初期サイズ
    ImGui::SetNextWindowSize(
        ImVec2(500.0f, 400.0f),
        ImGuiCond_FirstUseEver);

    if (!ImGui::Begin("UI Animation Editor", &m_IsOpen))
    {
        ImGui::End();
        return;
    }

    // 左側：アニメーション一覧
    DrawAnimationList();

    ImGui::SameLine();

    // 右側：アニメーション設定
    ImGui::BeginChild(
        "AnimationSettings",
        ImVec2(330.0f, 0.0f),
        false);

    UIAnimationData* data = m_Manager->GetData(m_SelectedAnimationName);

    if (data)
    {
        // アニメーション設定の描画
        bool settingsChanged = DrawAnimationSettings(*data);

        ImGui::Separator();

        // クリップ一覧
        DrawClipList(*data);

        bool animationChanged = false;

        if (m_SelectedClipIndex >= 0 &&
            m_SelectedClipIndex <
            static_cast<int>(data->Animations.size()))
        {
            ImGui::Dummy(ImVec2(0.0f, 5.0f)); // 空白を空ける
            ImGui::Separator();

            animationChanged = DrawClipSettings(data->Animations[m_SelectedClipIndex]);
        }

        auto it = m_GeneratedObjects.find(m_SelectedAnimationName);

        // 基本設定が変更された場合
        if (settingsChanged &&
            (it != m_GeneratedObjects.end() && it->second))
        {
            CUIFactory::ApplyTransform(it->second, *data);
        }

        // クリップが変更された場合のみ、アニメーションを更新
        if (animationChanged &&
            (it != m_GeneratedObjects.end() && it->second))
        {
            CUIFactory::ApplyAnimations(it->second, *data);
        }
    }
    else
    {
        ImGui::TextWrapped("Choose an animation from the list on the left, or create a new one");
    }

    ImGui::Dummy(ImVec2(0.0f, 5.0f)); // 空白を空ける
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.0f, 5.0f)); // 空白を空ける

    // シーンの取得
    auto scene = CManager::Instance()->GetSceneManager()->GetScene();

    // UI生成
    if (ImGui::Button("Generate UI"))
    {
        if (data && scene)
        {
            CGameObject* object = CUIFactory::Create(scene, *data);

            if (object)
            {
                m_GeneratedObjects[data->AnimationName] = object;
            }
        }
    }

    ImGui::Dummy(ImVec2(0.0f, 5.0f)); // 空白を空ける
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.0f, 5.0f)); // 空白を空ける

    // 選択しているアニメーションがなくても読み込める
    DrawSaveAndLoadButtons();

    ImGui::EndChild();

    ImGui::End();
}

//=============================================================================
// アニメーションリストの描画
//=============================================================================
void CUIAnimationEditor::DrawAnimationList(void)
{
    ImGui::BeginChild(
        "AnimationList",
        ImVec2(220.0f, 400.0f),
        true);

    ImGui::Text("Animations");
    ImGui::Separator();

    const auto names = m_Manager->GetAnimationNames();

    for (const auto& name : names)
    {
        const bool selected = (m_SelectedAnimationName == name);

        ImGui::Dummy(ImVec2(0.0f, 5.0f)); // 空白を空ける

        if (ImGui::Selectable(name.c_str(), selected))
        {
            m_SelectedAnimationName = name;
            m_SelectedClipIndex = -1;
        }
    }

    ImGui::Dummy(ImVec2(0.0f, 5.0f)); // 空白を空ける

    ImGui::Separator();

    ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 空白を空ける

    // 新規アニメーション
    ImGui::SetNextItemWidth(120.0f);
    ImGui::InputText(
        "New Name",
        m_NewAnimationName,
        sizeof(m_NewAnimationName));

    ImGui::Dummy(ImVec2(0.0f, 5.0f)); // 空白を空ける

    if (ImGui::Button("Create"))
    {
        if (m_NewAnimationName[0] != '\0')
        {
            UIAnimationData data;
            data.AnimationName = m_NewAnimationName;

            if (m_Manager->Register(data))
            {
                m_SelectedAnimationName = data.AnimationName;
                m_SelectedClipIndex = -1;

                m_NewAnimationName[0] = '\0';
            }
        }
    }

    ImGui::EndChild();
}

//=============================================================================
// アニメーション基本情報の描画
//=============================================================================
bool CUIAnimationEditor::DrawAnimationSettings(UIAnimationData& data)
{
    bool changed = false;

    ImGui::Dummy(ImVec2(10.0f, 0.0f)); // 空白を空ける

    ImGui::Text("Animation Settings");
    ImGui::Separator();

    ImGui::Dummy(ImVec2(10.0f, 0.0f)); // 空白を空ける

    // 名前
    char animationName[128] = {};
    strncpy_s(
        animationName,
        sizeof(animationName),
        data.AnimationName.c_str(),
        _TRUNCATE);

    ImGui::SetNextItemWidth(130.0f);
    if (ImGui::InputText("Animation Name", animationName, sizeof(animationName)))
    {
        // 名前を変更する場合は、マネージャーの登録キーも変更する必要がある。
        // そのため、ここでは名前の直接変更を行わない。
    }

    // GameObjectのタグ
    char objectTag[128] = {};
    strncpy_s(
        objectTag,
        sizeof(objectTag),
        data.ObjectTag.c_str(),
        _TRUNCATE);

    ImGui::SetNextItemWidth(130.0f);
    if (ImGui::InputText("Object Tag", objectTag, sizeof(objectTag)))
    {
        data.ObjectTag = objectTag;
        changed = true;
    }

    // テクスチャ
    char texturePath[256] = {};
    strncpy_s(
        texturePath,
        sizeof(texturePath),
        data.TexturePath.c_str(),
        _TRUNCATE);

    ImGui::Dummy(ImVec2(0.0f, 5.0f)); // 空白を空ける

    ImGui::SetNextItemWidth(130.0f);
    if (ImGui::InputText("Texture", texturePath, sizeof(texturePath)))
    {
        data.TexturePath = texturePath;
        changed = true;
    }

    ImGui::Dummy(ImVec2(0.0f, 5.0f)); // 空白を空ける

    // サイズ
    ImGui::SetNextItemWidth(130.0f);
    if (ImGui::SliderFloat("Width", &data.Width, 0.0f, 1.0f))
    {
        changed = true;
    }

    ImGui::SetNextItemWidth(130.0f);
    if (ImGui::SliderFloat("Height", &data.Height, 0.0f, 1.0f))
    {
        changed = true;
    }

    ImGui::Dummy(ImVec2(0.0f, 5.0f)); // 空白を空ける

    // 位置
    ImGui::SetNextItemWidth(130.0f);
    if (ImGui::DragFloat2(
        "Position",
        &data.Position.x,
        0.005f,
        0.0f,
        1.0f))
    {
        changed = true;
    }

    ImGui::Dummy(ImVec2(0.0f, 5.0f)); // 空白を空ける

    // スケール
    ImGui::SetNextItemWidth(130.0f);
    if (ImGui::DragFloat2(
        "Scale",
        &data.Scale.x,
        0.01f,
        0.0f,
        10.0f))
    {
        changed = true;
    }

    ImGui::Dummy(ImVec2(0.0f, 5.0f)); // 空白を空ける

    // 透明度
    ImGui::SetNextItemWidth(130.0f);
    if (ImGui::SliderFloat(
        "Alpha",
        &data.Alpha,
        0.0f,
        1.0f))
    {
        changed = true;
    }

    ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 空白を空ける

    //=============================================================================
// UV設定
//=============================================================================
    ImGui::Dummy(ImVec2(0.0f, 5.0f));

    if (ImGui::CollapsingHeader("UV Settings", ImGuiTreeNodeFlags_DefaultOpen))
    {
        // UV開始位置
        ImGui::SetNextItemWidth(130.0f);
        if (ImGui::DragFloat2(
            "UV Position",
            &data.UVPosition.x,
            0.01f,
            0.0f,
            1.0f))
        {
            changed = true;
        }

        // UVサイズ
        ImGui::SetNextItemWidth(130.0f);
        if (ImGui::DragFloat2(
            "UV Size",
            &data.UVSize.x,
            0.01f,
            0.0f,
            1.0f))
        {
            changed = true;
        }

        // 値の範囲を制限
        data.UVPosition.x = Clamp(data.UVPosition.x, 0.0f, 1.0f);
        data.UVPosition.y = Clamp(data.UVPosition.y, 0.0f, 1.0f);

        data.UVSize.x = Clamp(data.UVSize.x, 0.0f, 1.0f);
        data.UVSize.y = Clamp(data.UVSize.y, 0.0f, 1.0f);
    }

    ImGui::Dummy(ImVec2(0.0f, 10.0f));

    return changed;
}

//=============================================================================
// アニメーションクリップ一覧描画
//=============================================================================
void CUIAnimationEditor::DrawClipList(UIAnimationData& data)
{
    ImGui::Text("Animation Clips");

    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.0f, 5.0f)); // 空白を空ける

    for (int i = 0; i < static_cast<int>(data.Animations.size()); ++i)
    {
        const auto& clip = data.Animations[i];

        std::string label = clip.Name.empty()
            ? "Clip " + std::to_string(i)
            : clip.Name;

        label += "##Clip" + std::to_string(i);

        ImGui::Dummy(ImVec2(0.0f, 5.0f)); // 空白を空ける
        if (ImGui::Selectable(
            label.c_str(),
            m_SelectedClipIndex == i))
        {
            m_SelectedClipIndex = i;
        }
    }

    ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 空白を空ける

    if (ImGui::Button("Add Clip"))
    {
        UIAnimationClip clip;

        // 初期値
        clip.Name = "New Clip";
        clip.Property = UIProperty::Alpha;
        clip.From = 0.0f;
        clip.To = 1.0f;
        clip.Duration = 1.0f;
        clip.Delay = 0.0f;
        clip.LoopCount = 1;
        clip.PlaybackMode = UIPlaybackMode::Once;
        clip.Easing = CEasing::Linear;

        // Propertyの初期値は、実際のUIAnimationClipの定義に合わせる
        data.Animations.push_back(clip);

        m_SelectedClipIndex = static_cast<int>(data.Animations.size()) - 1;
    }

    ImGui::SameLine();

    if (ImGui::Button("Remove Clip"))
    {
        if (m_SelectedClipIndex >= 0 &&
            m_SelectedClipIndex < static_cast<int>(data.Animations.size()))
        {
            data.Animations.erase(data.Animations.begin() + m_SelectedClipIndex);

            m_SelectedClipIndex = -1;
        }
    }
}

//=============================================================================
// クリップの詳細設定描画
//=============================================================================
bool CUIAnimationEditor::DrawClipSettings(UIAnimationClip& clip)
{
    bool changed = false;

    ImGui::Text("Clip Settings");
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.0f, 5.0f)); // 空白を空ける

    // クリップ名
    char name[128] = {};

    strncpy_s(
        name,
        sizeof(name),
        clip.Name.c_str(),
        _TRUNCATE);

    ImGui::SetNextItemWidth(130.0f);
    if (ImGui::InputText("Clip Name", name, sizeof(name)))
    {
        clip.Name = name;
        changed = false;
    }

    //=========================================================================
    // プロパティ
    //=========================================================================

    const char* properties[] =
    {
        "Position X",
        "Position Y",
        "Scale X",
        "Scale Y",
        "Rotation",
        "Alpha",
        "Color R",
        "Color G",
        "Color B"
    };

    int property = static_cast<int>(clip.Property);

    if (property < 0 ||
        property >= IM_ARRAYSIZE(properties))
    {
        property = 0;
    }

    ImGui::SetNextItemWidth(130.0f);
    if (ImGui::Combo(
        "Property",
        &property,
        properties,
        IM_ARRAYSIZE(properties)))
    {
        clip.Property = static_cast<UIProperty>(property);
        changed = false;
    }

    //=========================================================================
    // From / To
    //=========================================================================

    ImGui::SetNextItemWidth(130.0f);
    if (ImGui::DragFloat("From", &clip.From, 0.01f))
    {
        changed = true;
    }

    ImGui::SetNextItemWidth(130.0f);
    if (ImGui::DragFloat("To", &clip.To, 0.01f))
    {
        changed = true;
    }

    //=========================================================================
    // 時間
    //=========================================================================

    ImGui::SetNextItemWidth(130.0f);
    if (ImGui::DragFloat(
        "Duration",
        &clip.Duration,
        0.01f,
        0.01f,
        60.0f))
    {
        changed = true;
    }

    ImGui::SetNextItemWidth(130.0f);
    if (ImGui::DragFloat(
        "Delay",
        &clip.Delay,
        0.01f,
        0.0f,
        60.0f))
    {
        changed = true;
    }

    //=========================================================================
    // プレイバックモード
    //=========================================================================

    const char* playbackModes[] =
    {
        "Once",
        "Loop",
        "PingPong"
    };

    int playbackMode = static_cast<int>(clip.PlaybackMode);

    if (playbackMode < 0 ||
        playbackMode >= IM_ARRAYSIZE(playbackModes))
    {
        playbackMode = 0;
    }

    ImGui::SetNextItemWidth(130.0f);
    if (ImGui::Combo(
        "Playback Mode",
        &playbackMode,
        playbackModes,
        IM_ARRAYSIZE(playbackModes)))
    {
        clip.PlaybackMode = static_cast<UIPlaybackMode>(playbackMode);
        changed = true;
    }

    //=========================================================================
    // ループカウント
    //=========================================================================

    ImGui::SetNextItemWidth(130.0f);
    if (ImGui::InputInt("Loop Count", &clip.LoopCount))
    {
        if (clip.LoopCount < 0)
        {
            clip.LoopCount = 0;
        }

        changed = true;
    }

    if (clip.LoopCount < 0)
    {
        clip.LoopCount = 0;
    }

    ImGui::TextDisabled("Loop Count = 0 : Infinite");

    //=========================================================================
    // イージング
    //=========================================================================

    const char* easingNames[] =
    {
        "Linear",
        "EaseInSine",
        "EaseOutSine",
        "EaseInOutSine",
        "EaseInQuad",
        "EaseOutQuad",
        "EaseInOutQuad",
        "EaseInCubic",
        "EaseOutCubic",
        "EaseInOutCubic",
        "EaseInQuart",
        "EaseOutQuart",
        "EaseInOutQuart",
        "EaseInQuint",
        "EaseOutQuint",
        "EaseInOutQuint",
        "EaseInExpo",
        "EaseOutExpo",
        "EaseInOutExpo",
        "EaseInCirc",
        "EaseOutCirc",
        "EaseInOutCirc",
        "EaseInElastic",
        "EaseOutElastic",
        "EaseInOutElastic",
        "EaseOutBounce",
        "EaseInBounce",
        "EaseInOutBounce",
    };

    int easingIndex = 0;

    if (clip.Easing == CEasing::EaseOutCubic)
    {
        easingIndex = 1;
    }

    ImGui::SetNextItemWidth(130.0f);
    if (ImGui::Combo(
        "Easing",
        &easingIndex,
        easingNames,
        IM_ARRAYSIZE(easingNames)))
    {
        switch (easingIndex)
        {
        case 0:
            clip.Easing = CEasing::Linear;
            break;

        case 1:
            clip.Easing = CEasing::EaseOutCubic;
            break;
        }

        changed = true;
    }

    return changed;
}

//=============================================================================
// 保存・読み込みボタン描画
//=============================================================================
void CUIAnimationEditor::DrawSaveAndLoadButtons(void)
{
    //=========================================================================
    // 保存
    //=========================================================================

    if (ImGui::Button("Save"))
    {
        UIAnimationData* data = m_Manager->GetData(m_SelectedAnimationName);

        if (data)
        {
            std::string path = OpenWindowsSaveFileDialog();

            if (!path.empty())
            {
                if (CUIAnimationSerializer::Save(path, *data))
                {
                    // 保存成功
                }
                else
                {
                    // 保存失敗
                }
            }
        }
    }

    ImGui::SameLine(0);

    //=========================================================================
    // 読み込み
    //=========================================================================

    if (ImGui::Button("Load"))
    {
        std::string path = OpenWindowsOpenFileDialog();

        if (!path.empty())
        {
            UIAnimationData data;

            if (CUIAnimationSerializer::Load(path, data))
            {
                const std::string loadedName = data.AnimationName;

                if (m_Manager->Register(data))
                {
                    m_SelectedAnimationName = loadedName;
                    m_SelectedClipIndex = -1;
                }
            }
            else
            {
                // 読み込み失敗
            }
        }
    }
}
