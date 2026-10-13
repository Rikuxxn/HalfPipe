//=============================================================================
//
// トリガーオブジェクト処理 [TriggerObject.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _TRIGGER_OBJECT_H_// このマクロ定義がされていなかったら
#define _TRIGGER_OBJECT_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "GameObject.h"
#include "json.hpp"

//*****************************************************************************
// 型定義
//*****************************************************************************
using json = nlohmann::json;


//*****************************************************************************
// トリガーオブジェクトクラス
//*****************************************************************************
class CTriggerObject : public CGameObject
{
public:
    CTriggerObject() = default;
    virtual ~CTriggerObject();

    // トリガーオブジェクトの種類
    enum class TriggerObjectType
    {
        EventArea,
        SpawnArea,

        Max
    };

    // トリガーシェイプの種類
    enum class TriggerShape
    {
        Box,
        Sphere,
        Capsule
    };

    virtual HRESULT Init(void) override { return S_OK; }
    virtual void Update(void) override;

    // トリガーオブジェクトの種類
    virtual TriggerObjectType GetType(void) const = 0;

    // コライダーの種類
    virtual TriggerShape GetTriggerShape(void) const = 0;

    // オブジェクト情報の保存・読み込み(共通部分)
    virtual void SaveToJson(json& j);
    virtual void LoadFromJson(const json& j);

    // トリガー固有処理
    virtual void DrawCustomUI(void) {}

    // トリガーのサイズ
    virtual D3DXVECTOR3 GetTriggerSize() const = 0;
};

#endif