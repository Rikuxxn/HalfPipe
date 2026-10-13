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

        if (easing == CEasing::EaseOutCubic)
        {
            return "EaseOutCubic";
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

        if (name == "EaseOutCubic")
        {
            return CEasing::EaseOutCubic;
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
            {"alpha", data.Alpha}
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