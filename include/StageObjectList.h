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
        float scale);

    // 中心位置取得
    const D3DXVECTOR3& GetCenterPosition(void) const { return m_CenterPosition; }

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

    // バスケットの中心位置
    D3DXVECTOR3 m_CenterPosition;

    // エイム可能な半径
    float m_AimRadius;
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

private:

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

    StageObjectType GetType(void) const override
    {
        return StageObjectType::PointCircle;
    }

    ColliderShape GetColliderShape(void) const override
    {
        return ColliderShape::Cylinder;
    }

    void OnCollisionEnter(CGameObject* other)override;

private:
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

#endif