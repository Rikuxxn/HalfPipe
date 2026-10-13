//=============================================================================
//
// トリガーオブジェクトエディター処理 [TriggerObjectEditor.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _TRIGGER_OBJECT_EDITOR_H_// このマクロ定義がされていなかったら
#define _TRIGGER_OBJECT_EDITOR_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "TriggerObject.h"

// 前方宣言
class CTransformGizmo;

//*****************************************************************************
// トリガーオブジェクトエディタークラス
//*****************************************************************************
class CTriggerObjectEditor
{
private:

    // 読み込み用構造体
    struct TriggerObjectInfo
    {
        CTriggerObject::TriggerObjectType   Type;       // 種類
        std::string                         Name;       // 名前
    };

public:
    CTriggerObjectEditor();
    ~CTriggerObjectEditor();

    HRESULT Init    (void);
    void    Uninit  (void);
    void    Update  (void);
    void CreateGizmo(void);

    const TriggerObjectInfo* FindTriggerObjectInfo(CTriggerObject::TriggerObjectType type) const;

    const std::vector<CTriggerObject*>& GetTriggerObjects(void) const
    {
        return m_TriggerObjects;
    }

    CGameObject* GetSelectedObject(void) const { return m_SelectedObject; }

private:

    void DrawTriggerList(void);
    void DrawInspector(CTriggerObject* selectedObject, std::vector<CTriggerObject*> triggerObjects);
    void DrawSaveLoad(void);

    void PickTriggerObject(void);

    // トリガーリストの読み込み
    void LoadTriggerList(const char* filename);

private:

    std::vector<TriggerObjectInfo> m_ObjectInfos;       // オブジェクト
    std::vector<CTriggerObject*> m_TriggerObjects;      // トリガーオブジェクト
    CTransformGizmo* m_TransformGizmo;                  // 移動ギズモ
    CTriggerObject*  m_SelectedObject;                  // 選択中のトリガー
    CTriggerObject*  m_PrevSelectedObject;              // 前回の選択中のトリガー
    D3DXVECTOR3      m_prevSize;                        // 前回のサイズ
    bool             m_isDragging;                      // ドラッグ中か
};

#endif