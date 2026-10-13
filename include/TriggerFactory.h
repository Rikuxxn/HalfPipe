//=============================================================================
//
// トリガーファクトリー処理 [TriggerFactory.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _TRIGGER_FACTORY_H_// このマクロ定義がされていなかったら
#define _TRIGGER_FACTORY_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "TriggerObject.h"

// 前方宣言
class CScene;

//*****************************************************************************
// トリガーファクトリークラス
//*****************************************************************************
class CTriggerFactory
{
public:

    // 型定義
    using Factory = std::function<CTriggerObject* (CScene*)>;

    // 生成処理
    static CTriggerObject* Create(CScene* scene, CTriggerObject::TriggerObjectType type);

private:

    static std::unordered_map<CTriggerObject::TriggerObjectType, Factory> m_Factory;

    static void InitFactory(void);
};

#endif