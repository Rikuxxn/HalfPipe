//=============================================================================
//
// UIアニメーションシリアライザー [UIAnimationSerializer.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "UIAnimationSerializer.h"
#include "json.hpp"

//*****************************************************************************
// 型定義
//*****************************************************************************
using json = nlohmann::json;

namespace
{
    //=========================================================================
    // UIProperty → 文字列
    //=========================================================================
    const char* ToString(UIProperty property)
    {
        switch (property)
        {
        case UIProperty::PositionX: return "PositionX";
        case UIProperty::PositionY: return "PositionY";
        case UIProperty::ScaleX:    return "ScaleX";
        case UIProperty::ScaleY:    return "ScaleY";
        case UIProperty::Rotation:  return "Rotation";
        case UIProperty::Alpha:     return "Alpha";
        case UIProperty::ColorR:    return "ColorR";
        case UIProperty::ColorG:    return "ColorG";
        case UIProperty::ColorB:    return "ColorB";
        default:
            throw std::runtime_error("Unknown UIProperty");
        }
    }

    //=========================================================================
    // 文字列 → UIProperty
    //=========================================================================
    UIProperty ToUIProperty(const std::string& name)
    {
        if (name == "PositionX") return UIProperty::PositionX;
        if (name == "PositionY") return UIProperty::PositionY;
        if (name == "ScaleX")    return UIProperty::ScaleX;
        if (name == "ScaleY")    return UIProperty::ScaleY;
        if (name == "Rotation")  return UIProperty::Rotation;
        if (name == "Alpha")     return UIProperty::Alpha;
        if (name == "ColorR")    return UIProperty::ColorR;
        if (name == "ColorG")    return UIProperty::ColorG;
        if (name == "ColorB")    return UIProperty::ColorB;

        throw std::runtime_error("Unknown UIProperty: " + name);
    }

    //=========================================================================
    // UIPlaybackMode → 文字列
    //=========================================================================
    const char* ToString(UIPlaybackMode mode)
    {
        switch (mode)
        {
        case UIPlaybackMode::Once:     return "Once";
        case UIPlaybackMode::Loop:     return "Loop";
        case UIPlaybackMode::PingPong: return "PingPong";
        default:
            throw std::runtime_error("Unknown UIPlaybackMode");
        }
    }

    //=========================================================================
    // 文字列 → UIPlaybackMode
    //=========================================================================
    UIPlaybackMode ToUIPlaybackMode(const std::string& name)
    {
        if (name == "Once")     return UIPlaybackMode::Once;
        if (name == "Loop")     return UIPlaybackMode::Loop;
        if (name == "PingPong") return UIPlaybackMode::PingPong;

        throw std::runtime_error("Unknown UIPlaybackMode: " + name);
    }

    //=========================================================================
    // Easing → 文字列
    //=========================================================================
    const char* ToString(float (*easing)(float))
    {
        if (easing == CEasing::Linear)
        {
            return "Linear";
        }

        if (easing == CEasing::EaseInSine)
        {
            return "EaseInSine";
        }

        if (easing == CEasing::EaseOutSine)
        {
            return "EaseOutSine";
        }

        if (easing == CEasing::EaseInOutSine)
        {
            return "EaseInOutSine";
        }

        if (easing == CEasing::EaseInQuad)
        {
            return "EaseInQuad";
        }

        if (easing == CEasing::EaseOutQuad)
        {
            return "EaseOutQuad";
        }

        if (easing == CEasing::EaseInOutQuad)
        {
            return "EaseInOutQuad";
        }

        if (easing == CEasing::EaseInCubic)
        {
            return "EaseInCubic";
        }

        if (easing == CEasing::EaseOutCubic)
        {
            return "EaseOutCubic";
        }

        if (easing == CEasing::EaseInOutCubic)
        {
            return "EaseInOutCubic";
        }

        if (easing == CEasing::EaseInQuart)
        {
            return "EaseInQuart";
        }

        if (easing == CEasing::EaseOutQuart)
        {
            return "EaseOutQuart";
        }

        if (easing == CEasing::EaseInOutQuart)
        {
            return "EaseInOutQuart";
        }

        if (easing == CEasing::EaseInQuint)
        {
            return "EaseInQuint";
        }

        if (easing == CEasing::EaseOutQuint)
        {
            return "EaseOutQuint";
        }

        if (easing == CEasing::EaseInOutQuint)
        {
            return "EaseInOutQuint";
        }

        if (easing == CEasing::EaseInExpo)
        {
            return "EaseInExpo";
        }

        if (easing == CEasing::EaseOutExpo)
        {
            return "EaseOutExpo";
        }

        if (easing == CEasing::EaseInOutExpo)
        {
            return "EaseInOutExpo";
        }

        if (easing == CEasing::EaseInCirc)
        {
            return "EaseInCirc";
        }

        if (easing == CEasing::EaseOutCirc)
        {
            return "EaseOutCirc";
        }

        if (easing == CEasing::EaseInOutCirc)
        {
            return "EaseInOutCirc";
        }

        if (easing == CEasing::EaseInElastic)
        {
            return "EaseInElastic";
        }

        if (easing == CEasing::EaseOutElastic)
        {
            return "EaseOutElastic";
        }

        if (easing == CEasing::EaseInOutElastic)
        {
            return "EaseInOutElastic";
        }

        if (easing == CEasing::EaseInBounce)
        {
            return "EaseInBounce";
        }

        if (easing == CEasing::EaseOutBounce)
        {
            return "EaseOutBounce";
        }

        if (easing == CEasing::EaseInOutBounce)
        {
            return "EaseInOutBounce";
        }

        throw std::runtime_error("Unknown Easing");
    }

    //=========================================================================
    // 文字列 → Easing
    //=========================================================================
    float (*ToEasingFunction(const std::string& name))(float)
    {
        if (name == "Linear")
        {
            return CEasing::Linear;
        }

        if (name == "EaseInSine")
        {
            return CEasing::EaseInSine;
        }

        if (name == "EaseOutSine")
        {
            return CEasing::EaseOutSine;
        }

        if (name == "EaseInOutSine")
        {
            return CEasing::EaseInOutSine;
        }

        if (name == "EaseInQuad")
        {
            return CEasing::EaseInQuad;
        }

        if (name == "EaseOutQuad")
        {
            return CEasing::EaseOutQuad;
        }

        if (name == "EaseInOutQuad")
        {
            return CEasing::EaseInOutQuad;
        }

        if (name == "EaseInCubic")
        {
            return CEasing::EaseInCubic;
        }

        if (name == "EaseOutCubic")
        {
            return CEasing::EaseOutCubic;
        }

        if (name == "EaseInOutCubic")
        {
            return CEasing::EaseInOutCubic;
        }

        if (name == "EaseInQuart")
        {
            return CEasing::EaseInQuart;
        }

        if (name == "EaseOutQuart")
        {
            return CEasing::EaseOutQuart;
        }

        if (name == "EaseInOutQuart")
        {
            return CEasing::EaseInOutQuart;
        }

        if (name == "EaseInQuint")
        {
            return CEasing::EaseInQuint;
        }

        if (name == "EaseOutQuint")
        {
            return CEasing::EaseOutQuint;
        }

        if (name == "EaseInOutQuint")
        {
            return CEasing::EaseInOutQuint;
        }

        if (name == "EaseInExpo")
        {
            return CEasing::EaseInExpo;
        }

        if (name == "EaseOutExpo")
        {
            return CEasing::EaseOutExpo;
        }

        if (name == "EaseInOutExpo")
        {
            return CEasing::EaseInOutExpo;
        }

        if (name == "EaseInCirc")
        {
            return CEasing::EaseInCirc;
        }

        if (name == "EaseOutCirc")
        {
            return CEasing::EaseOutCirc;
        }

        if (name == "EaseInOutCirc")
        {
            return CEasing::EaseInOutCirc;
        }

        if (name == "EaseInElastic")
        {
            return CEasing::EaseInElastic;
        }

        if (name == "EaseOutElastic")
        {
            return CEasing::EaseOutElastic;
        }

        if (name == "EaseInOutElastic")
        {
            return CEasing::EaseInOutElastic;
        }

        if (name == "EaseInBounce")
        {
            return CEasing::EaseInBounce;
        }

        if (name == "EaseOutBounce")
        {
            return CEasing::EaseOutBounce;
        }

        if (name == "EaseInOutBounce")
        {
            return CEasing::EaseInOutBounce;
        }

        throw std::runtime_error("Unknown Easing: " + name);
    }
}

//=============================================================================
// JSON保存
//=============================================================================
bool CUIAnimationSerializer::Save(const std::string& filePath, const UIAnimationData& data)
{
    try
    {
        json root;

        //=====================================================================
        // UI基本情報
        //=====================================================================
        root["objectTag"] = data.ObjectTag;
        root["animationName"] = data.AnimationName;
        root["texture"] = data.TexturePath;

        root["size"] = 
        {
            {"width", data.Width},
            {"height", data.Height}
        };

        root["transform"] = 
        {
            {"position", { data.Position.x, data.Position.y }},
            {"scale", { data.Scale.x, data.Scale.y }},
            {"alpha", data.Alpha},
            {"uvPosition", { data.UVPosition.x, data.UVPosition.y }},
            {"uvSize", { data.UVSize.x, data.UVSize.y }}
        };

        //=====================================================================
        // アニメーション
        //=====================================================================
        root["animations"] = json::array();

        for (const auto& clip : data.Animations)
        {
            json animation;

            animation["name"] = clip.Name;
            animation["property"] = ToString(clip.Property);

            animation["from"] = clip.From;
            animation["to"] = clip.To;

            animation["duration"] = clip.Duration;
            animation["delay"] = clip.Delay;

            animation["easing"] = ToString(clip.Easing);

            animation["playbackMode"] = ToString(clip.PlaybackMode);

            animation["loopCount"] = clip.LoopCount;

            root["animations"].push_back(animation);
        }

        //=====================================================================
        // ファイル出力
        //=====================================================================
        std::ofstream file(filePath);

        if (!file.is_open())
        {
            return false;
        }

        file << root.dump(4);

        return file.good();
    }
    catch (const std::exception&)
    {
        return false;
    }
}

//=============================================================================
// JSON読み込み
//=============================================================================
bool CUIAnimationSerializer::Load(const std::string& filePath, UIAnimationData& data)
{
    try
    {
        //=====================================================================
        // ファイル読み込み
        //=====================================================================
        std::ifstream file(filePath);

        if (!file.is_open())
        {
            return false;
        }

        json root;
        file >> root;

        //=====================================================================
        // UI基本情報
        //=====================================================================
        UIAnimationData loadedData;

        loadedData.ObjectTag = root.at("objectTag").get<std::string>();

        loadedData.AnimationName = root.at("animationName").get<std::string>();

        loadedData.TexturePath = root.at("texture").get<std::string>();

        loadedData.Width =root.at("size").at("width").get<float>();

        loadedData.Height = root.at("size").at("height").get<float>();

        const auto& transform = root.at("transform");

        loadedData.Position.x = transform.at("position").at(0).get<float>();

        loadedData.Position.y = transform.at("position").at(1).get<float>();

        loadedData.Scale.x = transform.at("scale").at(0).get<float>();

        loadedData.Scale.y = transform.at("scale").at(1).get<float>();

        loadedData.Alpha = transform.at("alpha").get<float>();

        //=====================================================================
        // UV設定
        //=====================================================================

        // UV情報が存在する場合のみ読み込む
        if (transform.contains("uvPosition") &&
            transform["uvPosition"].is_array() &&
            transform["uvPosition"].size() >= 2)
        {
            loadedData.UVPosition.x =
                transform["uvPosition"][0].get<float>();

            loadedData.UVPosition.y =
                transform["uvPosition"][1].get<float>();
        }

        if (transform.contains("uvSize") &&
            transform["uvSize"].is_array() &&
            transform["uvSize"].size() >= 2)
        {
            loadedData.UVSize.x =
                transform["uvSize"][0].get<float>();

            loadedData.UVSize.y =
                transform["uvSize"][1].get<float>();
        }

        //=====================================================================
        // アニメーション
        //=====================================================================
        for (const auto& animation : root.at("animations"))
        {
            UIAnimationClip clip;

            clip.Name = animation.at("name").get<std::string>();

            clip.Property = ToUIProperty(animation.at("property").get<std::string>());

            clip.From = animation.at("from").get<float>();

            clip.To = animation.at("to").get<float>();

            clip.Duration = animation.at("duration").get<float>();

            clip.Delay = animation.at("delay").get<float>();

            clip.Easing = ToEasingFunction(animation.at("easing").get<std::string>());

            clip.PlaybackMode = ToUIPlaybackMode(animation.at("playbackMode").get<std::string>());

            clip.LoopCount = animation.at("loopCount").get<int>();

            loadedData.Animations.push_back(clip);
        }

        //=====================================================================
        // 読み込み成功後に反映
        //=====================================================================
        data = std::move(loadedData);

        return true;
    }
    catch (const std::exception&)
    {
        return false;
    }
}