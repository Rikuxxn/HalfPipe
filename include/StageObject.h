//=============================================================================
//
// ステージオブジェクト処理 [StageObject.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _STAGE_OBJECT_H_// このマクロ定義がされていなかったら
#define _STAGE_OBJECT_H_// 2重インクルード防止のマクロ定義

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
// ステージオブジェクトクラス
//*****************************************************************************
class CStageObject : public CGameObject
{
public:
    CStageObject() = default;
    virtual ~CStageObject();

    // ステージオブジェクトの種類
    enum class StageObjectType
    {
        Floor,      // テスト : 床
        Wall,       // テスト : 壁
        Torch,      // テスト : 松明
        HalfPipe,   // ハーフパイプステージ
        Basket,     // かご
        Right,      // 照明
        SignBoard,  // 広告看板
        RiftPole,   // リフトのポール

        Max
    };

    // コライダーの種類
    enum class ColliderShape
    {
        Box,
        Sphere,
        Capsule,
        TriangleMesh
    };

    virtual HRESULT Init(void) override { return S_OK; }
    virtual void Update(void) override;

    // ステージオブジェクトの種類
    virtual StageObjectType GetType(void) const = 0;

    // コライダーの種類
    virtual ColliderShape GetColliderShape(void) const = 0;

    // オブジェクト情報の保存・読み込み(共通部分)
    virtual void SaveToJson(json& j);
    virtual void LoadFromJson(const json& j);

    // 派生クラスのGUI特殊処理用
    virtual void DrawCustomUI(void) {}

    // コライダーのサイズ(見えない物理判定壁用)
    D3DXVECTOR3 GetColliderSize(void) { return { 100.0f, 100.0f, 100.0f }; }
};

#endif