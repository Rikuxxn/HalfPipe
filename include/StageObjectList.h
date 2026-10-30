//=============================================================================
//
// ステージオブジェクトリスト処理 [StageObjectList.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _OBJECT_LIST_H_// このマクロ定義がされていなかったら
#define _OBJECT_LIST_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "StageObject.h"
#include "SoundManager.h"

// 前方宣言
class CPointCircleBlock;


//*****************************************************************************
// 床ブロッククラス
//*****************************************************************************
class CFloorBlock : public CStageObject
{
public:
    CFloorBlock();
    ~CFloorBlock();

    HRESULT Init(void) override;
    void Update(void) override;

    StageObjectType GetType(void) const override
    {
        return StageObjectType::Floor;
    }

    ColliderShape GetColliderShape(void) const override
    {
        return ColliderShape::Box;
    }

    void OnCollisionEnter(CGameObject* other)override;

private:
    //CRenderTarget* m_pRenderTarget = nullptr;

};

//*****************************************************************************
// 壁ブロッククラス
//*****************************************************************************
class CWallBlock : public CStageObject
{
public:
    CWallBlock();
    ~CWallBlock();

    HRESULT Init(void) override;
    void Update(void) override;

    StageObjectType GetType(void) const override
    {
        return StageObjectType::Wall;
    }

    ColliderShape GetColliderShape(void) const override
    {
        return ColliderShape::Box;
    }

    //void OnCollisionEnter(CGameObject* other)override;

private:

};

//*****************************************************************************
// 松明ブロッククラス
//*****************************************************************************
class CTorchBlock : public CStageObject
{
public:
    CTorchBlock();
    ~CTorchBlock();

    HRESULT Init    (void) override;
    void    Update  (void) override;

    StageObjectType GetType(void) const override
    {
        return StageObjectType::Torch;
    }

    ColliderShape GetColliderShape(void) const override
    {
        return ColliderShape::Box;
    }

    //void OnCollisionEnter(CGameObject* other)override;

private:
    CSoundManager::SoundHandle m_SoundHandle;// サウンドハンドル

    CGameObject* m_ParticleObject;// パーティクル
};

//*****************************************************************************
// ハーフパイプブロッククラス
//*****************************************************************************
class CHalfPipeBlock : public CStageObject
{
public:
    CHalfPipeBlock();
    ~CHalfPipeBlock();

    HRESULT Init(void) override;
    void Update(void) override;

    StageObjectType GetType(void) const override
    {
        return StageObjectType::HalfPipe;
    }

    ColliderShape GetColliderShape(void) const override
    {
        return ColliderShape::TriangleMesh;
    }

private:
};

//*****************************************************************************
// かごブロッククラス
//*****************************************************************************
class CBasketBlock : public CStageObject
{
public:
    CBasketBlock();
    ~CBasketBlock();

    HRESULT Init(void) override;
    void Update(void) override;

    CPointCircleBlock* CreatePointCircle(
        const D3DXVECTOR3& position,
        float scale,
        int point);

    CGameObject* CreateTexPoly(
        const D3DXVECTOR3 pos,
        float scale,
        const std::string texPath,
        const D3DXCOLOR col);

    // 中心位置取得
    const D3DXVECTOR3& GetCenterPosition(void) const { return m_CenterPosition; }

    // かごの回転取得
    const D3DXVECTOR3& GetCenterRotation(void) const { return m_CenterRotation; }

    // エイム範囲取得
    float GetAimRadius(void) const { return m_AimRadius; }

    StageObjectType GetType(void) const override
    {
        return StageObjectType::Basket;
    }

    ColliderShape GetColliderShape(void) const override
    {
        return ColliderShape::Box;
    }

private:

    // 得点サークルの位置・回転を更新
    void UpdatePointCircles(void);

    // テクスチャ用板ポリの位置・回転を更新
    void UpdateTexPoly(void);

private:
    static constexpr float CIRCLE_OFFSET_Y = 60.0f;

    // バスケットの中心位置
    D3DXVECTOR3 m_CenterPosition;
    D3DXVECTOR3 m_CenterRotation;

    // 得点サークル
    std::vector<CPointCircleBlock*> m_PointCircles;

    // テクスチャ用板ポリ
    std::vector<CGameObject*> m_TexPoly;

    // エイム可能な半径
    float m_AimRadius;

    // 得点サークルの高さの間隔
    float m_HeightOffset;
};

//*****************************************************************************
// 照明ブロッククラス
//*****************************************************************************
class CRightBlock : public CStageObject
{
public:
    CRightBlock();
    ~CRightBlock();

    HRESULT Init(void) override;
    void Update(void) override;

    StageObjectType GetType(void) const override
    {
        return StageObjectType::Right;
    }

    ColliderShape GetColliderShape(void) const override
    {
        return ColliderShape::Box;
    }


private:
};

//*****************************************************************************
// 広告看板ブロッククラス
//*****************************************************************************
class CSignBoardBlock : public CStageObject
{
public:
    CSignBoardBlock();
    ~CSignBoardBlock();

    struct SignTextureInfo
    {
        int FaceIndex = 0;
        std::string TexturePath;
    };

    HRESULT Init(void) override;
    void Update(void) override;

    StageObjectType GetType(void) const override
    {
        return StageObjectType::SignBoard;
    }

    ColliderShape GetColliderShape(void) const override
    {
        return ColliderShape::Box;
    }

    void SaveToJson(json& j) override;
    void LoadFromJson(const json& j) override;

    void DrawCustomUI(void) override;

private:

    std::vector<SignTextureInfo> m_SignTextures;

    void ApplySignTextures(void);
};

//*****************************************************************************
// リフトポールブロッククラス
//*****************************************************************************
class CRiftPoleBlock : public CStageObject
{
public:
    CRiftPoleBlock();
    ~CRiftPoleBlock();

    HRESULT Init(void) override;
    void Update(void) override;

    StageObjectType GetType(void) const override
    {
        return StageObjectType::RiftPole;
    }

    ColliderShape GetColliderShape(void) const override
    {
        return ColliderShape::Box;
    }
};

//*****************************************************************************
// 得点サークルブロッククラス
//*****************************************************************************
class CPointCircleBlock : public CStageObject
{
public:
    CPointCircleBlock();
    ~CPointCircleBlock();

    HRESULT Init(void) override;
    void Update(void) override;

    void AddScore(int point);

    StageObjectType GetType(void) const override
    {
        return StageObjectType::PointCircle;
    }

    ColliderShape GetColliderShape(void) const override
    {
        return ColliderShape::Cylinder;
    }

    void OnTriggerEnter(CGameObject* other)override;

    // 得点設定
    void SetPoint(int point) { m_Point = point; }

    // 得点取得
    int GetPoint(void) const { return m_Point; }

private:
    int m_Point;    // 得点
};

//*****************************************************************************
// ゲートバナーブロッククラス
//*****************************************************************************
class CGateBannerBlock : public CStageObject
{
public:
    CGateBannerBlock();
    ~CGateBannerBlock();

    struct SignTextureInfo
    {
        int FaceIndex = 0;
        std::string TexturePath;
    };

    HRESULT Init(void) override;
    void Update(void) override;

    StageObjectType GetType(void) const override
    {
        return StageObjectType::GateBanner;
    }

    ColliderShape GetColliderShape(void) const override
    {
        return ColliderShape::TriangleMesh;
    }

    void SaveToJson(json& j) override;
    void LoadFromJson(const json& j) override;

    void DrawCustomUI(void) override;

private:

    std::vector<SignTextureInfo> m_SignTextures;

    void ApplySignTextures(void);

};

//*****************************************************************************
// リフトブロッククラス
//*****************************************************************************
class CRiftBlock : public CStageObject
{
public:
    CRiftBlock();
    ~CRiftBlock();

    HRESULT Init(void) override;
    void Update(void) override;

    StageObjectType GetType(void) const override
    {
        return StageObjectType::Rift;
    }

    ColliderShape GetColliderShape(void) const override
    {
        return ColliderShape::Box;
    }

private:

    void SaveToJson(json& j) override;
    void LoadFromJson(const json& j) override;

    void DrawCustomUI(void) override;

    void AddMovePoint(const D3DXVECTOR3& position);

    // ポイントデバッグモデル生成
    void CreateMovePointObject(const D3DXVECTOR3& pos);

    void SetStartPoint(int nPoint) { m_StartMovePoint = nPoint; }

private:
    static constexpr float REACH_DIST       = 15.0f;   // 到達判定距離
    static constexpr float ROTATION_SPEED   = 2.0f;    // 角度の補間速度


    // 移動ポイント
    std::vector<D3DXVECTOR3> m_MovePoints;

    // 自動移動ポイントデバッグ表示用
    std::vector<CGameObject*> m_pPointObj;

    // 進行方向
    D3DXVECTOR3 m_MoveDirection;

    // 現在のポイント(エディタ用)
    int m_currentPoint;

    // 移動開始ポイント
    int m_StartMovePoint;

    // 移動速度
    float m_AutoMoveSpeed;
};

//*****************************************************************************
// グレーラインブロッククラス
//*****************************************************************************
class CRainGrayBlock : public CStageObject
{
public:
    CRainGrayBlock();
    ~CRainGrayBlock();

    HRESULT Init(void) override;
    void Update(void) override;

    StageObjectType GetType(void) const override
    {
        return StageObjectType::RainGray;
    }

    ColliderShape GetColliderShape(void) const override
    {
        return ColliderShape::Box;
    }

private:
};

//*****************************************************************************
// オレンジラインブロッククラス
//*****************************************************************************
class CRainOrangeBlock : public CStageObject
{
public:
    CRainOrangeBlock();
    ~CRainOrangeBlock();

    HRESULT Init(void) override;
    void Update(void) override;

    StageObjectType GetType(void) const override
    {
        return StageObjectType::RainOrange;
    }

    ColliderShape GetColliderShape(void) const override
    {
        return ColliderShape::Box;
    }

private:
};

//*****************************************************************************
// 赤ラインブロッククラス
//*****************************************************************************
class CRainRedBlock : public CStageObject
{
public:
    CRainRedBlock();
    ~CRainRedBlock();

    HRESULT Init(void) override;
    void Update(void) override;

    StageObjectType GetType(void) const override
    {
        return StageObjectType::RainRed;
    }

    ColliderShape GetColliderShape(void) const override
    {
        return ColliderShape::Box;
    }

private:
};

//*****************************************************************************
// 黄色ラインブロッククラス
//*****************************************************************************
class CRainYellowBlock : public CStageObject
{
public:
    CRainYellowBlock();
    ~CRainYellowBlock();

    HRESULT Init(void) override;
    void Update(void) override;

    StageObjectType GetType(void) const override
    {
        return StageObjectType::RainYellow;
    }

    ColliderShape GetColliderShape(void) const override
    {
        return ColliderShape::Box;
    }

private:
};

//*****************************************************************************
// 表彰台ブロッククラス
//*****************************************************************************
class CPodiumBlock : public CStageObject
{
public:
    CPodiumBlock();
    ~CPodiumBlock();

    HRESULT Init(void) override;
    void Update(void) override;

    StageObjectType GetType(void) const override
    {
        return StageObjectType::Podium;
    }

    ColliderShape GetColliderShape(void) const override
    {
        return ColliderShape::Box;
    }

private:
};

#endif