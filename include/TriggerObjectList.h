//=============================================================================
//
// トリガーオブジェクトリスト処理 [TriggerObjectList.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _TRIGGER_LIST_H_// このマクロ定義がされていなかったら
#define _TRIGGER_LIST_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "TriggerObject.h"




//*****************************************************************************
// イベントエリアトリガークラス
//*****************************************************************************
class CEventTrigger : public CTriggerObject
{
public:

    // トリガーの種類
    TriggerObjectType GetType(void) const override
    {
        return TriggerObjectType::EventArea;
    }

    // トリガーシェイプの種類
    TriggerShape GetTriggerShape(void) const override
    {
        return TriggerShape::Box;
    }

    // トリガーサイズ
    D3DXVECTOR3 GetTriggerSize(void) const override
    {
        return { 500.0f, 100.0f, 500.0f };
    }

private:

};

//*****************************************************************************
// ゴールエリアトリガークラス
//*****************************************************************************
class CGoalTrigger : public CTriggerObject
{
public:
    CGoalTrigger();
    ~CGoalTrigger();

    HRESULT Init    (void) override;
    void    Update  (void) override;

    // トリガーの種類
    TriggerObjectType GetType(void) const override
    {
        return TriggerObjectType::GoalArea;
    }

    // トリガーシェイプの種類
    TriggerShape GetTriggerShape(void) const override
    {
        return TriggerShape::Box;
    }

    // トリガーサイズ
    D3DXVECTOR3 GetTriggerSize(void) const override
    {
        return { 500.0f, 100.0f, 500.0f };
    }

    void OnTriggerEnter(CGameObject* other) override;

private:
    CGameObject* m_FinishUI;    // FInishUIオブジェクト

    bool m_IsGoal;              // ゴールしたかどうか
};

#endif
