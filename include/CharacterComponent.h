//=============================================================================
//
// キャラクターコンポーネント処理 [CharacterComponent.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _CHARACTER_COMPONENT_H_// このマクロ定義がされていなかったら
#define _CHARACTER_COMPONENT_H_// 2重インクルード防止のマクロ定義


//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Component.h"
#include "CharacterAsset.h"

// 前方宣言
class CTransformComponent;

//*****************************************************************************
// キャラクターコンポーネントクラス
//*****************************************************************************
class CCharacterComponent : public CComponent
{
private:

    //---------------------------------
    // パーツオブジェクト
    //---------------------------------

    std::vector<CGameObject*> m_Parts;

    //---------------------------------
    // Transformキャッシュ
    //---------------------------------

    std::vector<CTransformComponent*> m_Transforms;

    //---------------------------------
    // キャラクターアセット
    //---------------------------------

    std::shared_ptr<CCharacterAsset> m_Asset;

    //---------------------------------
    // 再生状態
    //---------------------------------

    int m_MotionIndex;
    int m_KeyIndex;
    int m_FrameCounter;

    //---------------------------------
    // ブレンド
    //---------------------------------

    int  m_BlendMotionIndex;
    int  m_BlendKeyIndex;
    int  m_BlendFrameCounter;
    int  m_BlendTimer;
    int  m_BlendDuration;
    bool m_IsBlending;

public:

    CCharacterComponent() = default;
    ~CCharacterComponent() = default;

    HRESULT Init    (void) override;
    void    Update  (void) override;

public:

    bool Load(const char* filepath);

    void PlayMotion(int index, int blendFrame);

    //bool FinishMotion(int index);

private:

    void CreateParts(void);

    void ApplyMotion(void);

    D3DXVECTOR3 LerpPos(
        const CCharacterAsset::KEY_FRAME::PART_KEY& a,
        const CCharacterAsset::KEY_FRAME::PART_KEY& b,
        float t);

    D3DXVECTOR3 LerpRot(
        const CCharacterAsset::KEY_FRAME::PART_KEY& a,
        const CCharacterAsset::KEY_FRAME::PART_KEY& b,
        float t);

};

#endif
