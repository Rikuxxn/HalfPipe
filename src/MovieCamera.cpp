//=============================================================================
//
// ムービーカメラ処理 [MovieCamera.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "MovieCamera.h"
#include "Manager.h"
#include "TimeSystem.h"
#include "MathUtility.h"
#include "Easing.h"
#include "json.hpp"

// 名前空間の使用
using namespace MathUtil;

//*****************************************************************************
// 型定義
//*****************************************************************************
using json = nlohmann::json;

//=============================================================================
// コンストラクタ
//=============================================================================
CMovieCamera::CMovieCamera() :
	m_CurrentKey	(0),
	m_CurrentTime	(0.0f),
	m_IsPlaying		(false)
{

}

//=============================================================================
// 更新処理
//=============================================================================
void CMovieCamera::Update(CameraTransform& transform)
{
    // 再生じゃなかったら
    if (!m_IsPlaying)
    {
        return;
    }

    // 空の場合はスキップ
    if (m_Keys.empty())
    {
        m_IsPlaying = false;
        return;
    }

    // 最後のキーフレーム
    if (m_CurrentKey >= static_cast<int>(m_Keys.size()) - 1)
    {
        const KeyFrame& key = m_Keys.back();

        transform.posV      = key.posV;
        transform.posR      = key.posR;
        transform.rotation  = key.rotation;

        m_IsPlaying = false;
        return;
    }

    const KeyFrame& current = m_Keys[m_CurrentKey];
    const KeyFrame& next    = m_Keys[m_CurrentKey + 1];

    // デルタタイム
    float deltaTime = CManager::Instance()->GetTimeSystem()->GetDeltaTime();

    m_CurrentTime += deltaTime;

    // 待機時間
    if (m_CurrentTime < current.delay)
    {
        transform.posV      = current.posV;
        transform.posR      = current.posR;
        transform.rotation  = current.rotation;

        return;
    }

    // 補間用時間
    float time = m_CurrentTime - current.delay;

    float t = 1.0f;

    if (current.duration > 0.0f)
    {
        t = time / current.duration;
    }

    t = Clamp(t, 0.0f, 1.0f);

    // キーフレームに設定されたイージングを適用
    float easedT = t;

    if (current.easing)
    {
        easedT = current.easing(t);
    }

    //=========================================================
    // カメラ位置
    //=========================================================

    D3DXVec3Lerp(&transform.posV, &current.posV, &next.posV, t);

    //=========================================================
    // 注視点
    //=========================================================

    D3DXVec3Lerp(&transform.posR, &current.posR, &next.posR, t);

    //=========================================================
    // カメラ回転
    //=========================================================

    transform.rotation.x = LerpAngle(current.rotation.x, next.rotation.x, t);
    transform.rotation.y = LerpAngle(current.rotation.y, next.rotation.y, t);
    transform.rotation.z = LerpAngle(current.rotation.z, next.rotation.z, t);

    //=========================================================
    // 次のキーフレームへ
    //=========================================================

    if (time >= current.duration)
    {
        m_CurrentKey++;
        m_CurrentTime = 0.0f;
    }
}

//=============================================================================
// キーフレームの設定
//=============================================================================
void CMovieCamera::SetKeyFrames(const std::vector<KeyFrame>& keys)
{
    m_Keys = keys;

    m_CurrentKey    = 0;
    m_CurrentTime   = 0.0f;
    m_IsPlaying     = false;
}

//=============================================================================
// 開始
//=============================================================================
void CMovieCamera::Start(void)
{
    if (m_Keys.empty())
    {
        return;
    }

    m_CurrentKey    = 0;
    m_CurrentTime   = 0.0f;
    m_IsPlaying     = true;
}

//=============================================================================
// 停止
//=============================================================================
void CMovieCamera::Stop(void)
{
    m_IsPlaying = false;
}

//=============================================================================
// 保存処理
//=============================================================================
void CMovieCamera::Save(const char* filename)
{
    json j;

    // キーフレーム配列を作成
    j["keyFrames"] = json::array();

    for (const auto& key : m_Keys)
    {
        json frame;

        frame["posV"] = 
        {
            { "x", key.posV.x },
            { "y", key.posV.y },
            { "z", key.posV.z }
        };

        frame["posR"] = 
        {
            { "x", key.posR.x },
            { "y", key.posR.y },
            { "z", key.posR.z }
        };

        frame["rotation"] = 
        {
            { "x", key.rotation.x },
            { "y", key.rotation.y },
            { "z", key.rotation.z }
        };

        frame["distance"] = key.distance;
        frame["duration"] = key.duration;
        frame["delay"] = key.delay;

        // easingは関数なので文字列として保存
        frame["easing"] = GetEasingName(key.easing);
   
        j["keyFrames"].push_back(frame);
    }

    std::ofstream file(filename);

    if (!file.is_open())
    {
        return;
    }

    file << std::setw(4) << j;

    file.close();
}

//=============================================================================
// 読み込み処理
//=============================================================================
void CMovieCamera::Load(const char* filename)
{
    std::ifstream ifs(filename);

    if (!ifs)
    {
        return;
    }

    json json;
    ifs >> json;

    m_Keys.clear();

    for (const auto& frame : json["keyFrames"])
    {
        KeyFrame key;

        key.posV = D3DXVECTOR3(
            frame["posV"]["x"].get<float>(),
            frame["posV"]["y"].get<float>(),
            frame["posV"]["z"].get<float>());

        key.posR = D3DXVECTOR3(
            frame["posR"]["x"].get<float>(),
            frame["posR"]["y"].get<float>(),
            frame["posR"]["z"].get<float>());

        key.rotation = D3DXVECTOR3(
            frame["rotation"]["x"].get<float>(),
            frame["rotation"]["y"].get<float>(),
            frame["rotation"]["z"].get<float>());

        key.distance    = frame["distance"].get<float>();
        key.duration    = frame["duration"].get<float>();
        key.delay       = frame["delay"].get<float>();

        key.easing = GetEasingFunction(frame["easing"].get<std::string>());

        m_Keys.push_back(key);
    }
}

//=============================================================================
// イージングの名前取得
//=============================================================================
std::string CMovieCamera::GetEasingName(const std::function<float(float)>& easing)
{
    if (easing.target<float(*)(float)>() == nullptr)
    {
        return "Linear";
    }

    auto func = *easing.target<float(*)(float)>();

    if (func == CEasing::Linear)
        return "Linear";

    if (func == CEasing::EaseInSine)
        return "EaseInSine";

    if (func == CEasing::EaseOutSine)
        return "EaseOutSine";

    if (func == CEasing::EaseInOutSine)
        return "EaseInOutSine";

    if (func == CEasing::EaseInQuad)
        return "EaseInQuad";

    if (func == CEasing::EaseOutQuad)
        return "EaseOutQuad";

    if (func == CEasing::EaseInOutQuad)
        return "EaseInOutQuad";

    if (func == CEasing::EaseInCubic)
        return "EaseInCubic";

    if (func == CEasing::EaseOutCubic)
        return "EaseOutCubic";

    if (func == CEasing::EaseInOutCubic)
        return "EaseInOutCubic";

    return "Linear";
}

//=============================================================================
// イージングのファンクション
//=============================================================================
std::function<float(float)> CMovieCamera::GetEasingFunction(const std::string& name)
{
    if (name == "Linear")
        return CEasing::Linear;

    if (name == "EaseInSine")
        return CEasing::EaseInSine;

    if (name == "EaseOutSine")
        return CEasing::EaseOutSine;

    if (name == "EaseInOutSine")
        return CEasing::EaseInOutSine;

    if (name == "EaseInQuad")
        return CEasing::EaseInQuad;

    if (name == "EaseOutQuad")
        return CEasing::EaseOutQuad;

    if (name == "EaseInOutQuad")
        return CEasing::EaseInOutQuad;

    if (name == "EaseInCubic")
        return CEasing::EaseInCubic;

    if (name == "EaseOutCubic")
        return CEasing::EaseOutCubic;

    if (name == "EaseInOutCubic")
        return CEasing::EaseInOutCubic;

    // 不明な場合
    return CEasing::Linear;
}