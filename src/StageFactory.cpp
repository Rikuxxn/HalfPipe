//=============================================================================
//
// ステージファクトリー処理 [StageFactory.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "StageFactory.h"
#include "Scene.h"
#include "StageObjectList.h"


//*****************************************************************************
// 静的メンバ変数宣言
//*****************************************************************************
std::unordered_map<CStageObject::StageObjectType, CStageFactory::Factory> CStageFactory::m_Factory;


//*****************************************************************************
// 生成処理
//*****************************************************************************
CStageObject* CStageFactory::Create(CScene* scene, CStageObject::StageObjectType type)
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
void CStageFactory::InitFactory(void)
{
    // ファクトリーのクリア
    m_Factory.clear();

    m_Factory[CStageObject::StageObjectType::Floor] = [](CScene* scene) { return scene->CreateObject<CFloorBlock>(); };
    m_Factory[CStageObject::StageObjectType::Wall]  = [](CScene* scene) { return scene->CreateObject<CWallBlock>(); };
    m_Factory[CStageObject::StageObjectType::Torch] = [](CScene* scene) { return scene->CreateObject<CTorchBlock>(); };

    m_Factory[CStageObject::StageObjectType::HalfPipe]      = [](CScene* scene) { return scene->CreateObject<CHalfPipeBlock>(); };
    m_Factory[CStageObject::StageObjectType::Basket]        = [](CScene* scene) { return scene->CreateObject<CBasketBlock>(); };
    m_Factory[CStageObject::StageObjectType::Right]         = [](CScene* scene) { return scene->CreateObject<CRightBlock>(); };
    m_Factory[CStageObject::StageObjectType::SignBoard]     = [](CScene* scene) { return scene->CreateObject<CSignBoardBlock>(); };
    m_Factory[CStageObject::StageObjectType::RiftPole]      = [](CScene* scene) { return scene->CreateObject<CRiftPoleBlock>(); };
    m_Factory[CStageObject::StageObjectType::PointCircle]   = [](CScene* scene) { return scene->CreateObject<CPointCircleBlock>(); };
    m_Factory[CStageObject::StageObjectType::GateBanner]    = [](CScene* scene) { return scene->CreateObject<CGateBannerBlock>(); };
}