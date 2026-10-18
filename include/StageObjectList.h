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

    StageObjectType GetType(void) const override
    {
        return StageObjectType::Basket;
    }

    ColliderShape GetColliderShape(void) const override
    {
        return ColliderShape::Box;
    }

private:
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

#endif