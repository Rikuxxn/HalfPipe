//=============================================================================
//
// ステージファクトリー処理 [StageFactory.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _STAGE_FACTORY_H_// このマクロ定義がされていなかったら
#define _STAGE_FACTORY_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "StageObject.h"


// 前方宣言
class CScene;

//*****************************************************************************
// ステージファクトリークラス
//*****************************************************************************
class CStageFactory
{
public:

    // 型定義
    using Factory = std::function<CStageObject* (CScene*)>;

    // 生成処理
    static CStageObject* Create(CScene* scene, CStageObject::StageObjectType type);

private:

    static std::unordered_map<CStageObject::StageObjectType, Factory> m_Factory;

    static void InitFactory(void);
};

#endif