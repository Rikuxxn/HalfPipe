//=============================================================================
//
// UIアニメーションマネージャー [UIAnimationManager.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "UIAnimationManager.h"
#include "UIAnimationSerializer.h"
#include <algorithm>

//=============================================================================
// デストラクタ
//=============================================================================
CUIAnimationManager::~CUIAnimationManager()
{
    // なし
}

//=============================================================================
// UI保存処理
//=============================================================================
bool CUIAnimationManager::Save(const std::string& animationName, const std::string& filePath)
{
    // データ検索
    auto it = m_Datas.find(animationName);

    if (it == m_Datas.end())
    {
        return false;
    }

    // JSON保存
    if (!CUIAnimationSerializer::Save(filePath, it->second))
    {
        return false;
    }

    return true;
}

//=============================================================================
// UI読み込み処理
//=============================================================================
bool CUIAnimationManager::Load(const std::string& filePath)
{
    // JSON読み込み
    UIAnimationData data;

    if (!CUIAnimationSerializer::Load(filePath, data))
    {
        return false;
    }

    // データ登録
    if (!Register(data))
    {
        return false;
    }

    return true;
}

//=============================================================================
// UI登録処理
//=============================================================================
bool CUIAnimationManager::Register(const UIAnimationData& data)
{
    if (data.AnimationName.empty())
    {
        return false;
    }

    m_Datas[data.AnimationName] = data;

    return true;
}

//=============================================================================
// UI削除処理
//=============================================================================
bool CUIAnimationManager::Remove(const std::string& animationName)
{
    auto it = m_Datas.find(animationName);

    if (it == m_Datas.end())
    {
        return false;
    }

    m_Datas.erase(it);

    return true;
}

//=============================================================================
// データ検索処理
//=============================================================================
UIAnimationData* CUIAnimationManager::GetData(const std::string& animationName)
{
    auto it = m_Datas.find(animationName);

    if (it == m_Datas.end())
    {
        return nullptr;
    }

    return &it->second;
}

//=============================================================================
// データ一覧取得処理
//=============================================================================
std::vector<std::string> CUIAnimationManager::GetAnimationNames(void) const
{
    std::vector<std::string> names;

    names.reserve(m_Datas.size());

    for (const auto& pair : m_Datas)
    {
        names.push_back(pair.first);
    }

    std::sort(names.begin(), names.end());

    return names;
}