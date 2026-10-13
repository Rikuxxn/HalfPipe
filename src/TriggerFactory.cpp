//=============================================================================
//
// トリガーファクトリー処理 [TriggerFactory.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "TriggerFactory.h"
#include "Scene.h"
#include "TriggerObjectList.h"


//*****************************************************************************
// 静的メンバ変数宣言
//*****************************************************************************
std::unordered_map<CTriggerObject::TriggerObjectType, CTriggerFactory::Factory> CTriggerFactory::m_Factory;


//*****************************************************************************
// 生成処理
//*****************************************************************************
CTriggerObject* CTriggerFactory::Create(CScene* scene, CTriggerObject::TriggerObjectType type)
{
    if (m_Factory.empty())
    {
        InitFactory();
    }

    auto it = m_Factory.find(type);

    if (it == m_Factory.end())
    {
        return nullptr;
    }

    return it->second(scene);
}

//*****************************************************************************
// ファクトリーの初期化処理
//*****************************************************************************
void CTriggerFactory::InitFactory(void)
{
    // ファクトリーのクリア
    m_Factory.clear();

    m_Factory[CTriggerObject::TriggerObjectType::EventArea] = [](CScene* scene) { return scene->CreateObject<CEventTrigger>(); };
    m_Factory[CTriggerObject::TriggerObjectType::SpawnArea] = [](CScene* scene) { return scene->CreateObject<CSpawnTrigger >(); };
}