//=============================================================================
//
// キャラクターコンポーネント理 [CharacterComponent.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "CharacterComponent.h"
#include "GameObject.h"
#include "XModelRenderComponent.h"
#include "Manager.h"
#include "SceneManager.h"
#include "TransformComponent.h"


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CCharacterComponent::Init(void)
{
    //---------------------------------
    // 再生状態
    //---------------------------------

    m_MotionIndex   = 0;
    m_KeyIndex      = 0;
    m_FrameCounter  = 0;

    //---------------------------------
    // ブレンド
    //---------------------------------

    m_IsBlending        = false;
    m_BlendMotionIndex  = 0;
    m_BlendKeyIndex     = 0;
    m_BlendFrameCounter = 0;
    m_BlendTimer        = 0;
    m_BlendDuration     = 0;

    return S_OK;
}

//=============================================================================
// 更新処理
//=============================================================================
void CCharacterComponent::Update(void)
{

    ApplyMotion();
}

//=============================================================================
// 読み込み処理
//=============================================================================
bool CCharacterComponent::Load(const char* filepath)
{
    m_Asset = CCharacterAsset::Load(filepath);

    if (!m_Asset)
    {
        return false;
    }

    // パーツ生成
    CreateParts();

    // デフォルトモーション
    PlayMotion(0, 10);

    return true;
}

//=============================================================================
// パーツ生成
//=============================================================================
void CCharacterComponent::CreateParts(void)
{
    auto scene = m_pOwner->GetScene();

    if (!scene)
    {
        return;
    }

    auto& parts = m_Asset->Parts;

    //---------------------------------
    // パーツ生成
    //---------------------------------

    for (auto& part : parts)
    {
        auto obj = scene->CreateObject();

        //---------------------------------
        // Transform
        //---------------------------------

        auto transform = obj->AddComponent<CTransformComponent>();

        transform->Position = part.offsetPos;

        transform->Rotation = part.offsetRot;

        //---------------------------------
        // Model
        //---------------------------------

        auto render = obj->AddComponent<CXModelRenderComponent>();

        // 描画順の設定
        render->SetLayer(RenderLayer::Player);

        render->Load(part.path.c_str());

        //---------------------------------
        // 保存
        //---------------------------------

        m_Parts.push_back(obj);

        m_Transforms.push_back(transform);
    }

    //---------------------------------
    // 親子設定
    //---------------------------------

    auto ownerTransform = m_pOwner->GetComponent<CTransformComponent>();

    for (int i = 0; i < parts.size(); i++)
    {
        int parent = parts[i].parentIndex;

        if (parent < 0)
        {
            m_Transforms[i]->SetParent(ownerTransform);
            continue;
        }

        m_Transforms[i]->SetParent(m_Transforms[parent]);
    }
}

//=============================================================================
// モーション再生
//=============================================================================
void CCharacterComponent::PlayMotion(int index, int blendFrame)
{
    if (!m_Asset)
    {
        return;
    }

    // indexが範囲外だったら飛ばす
    if (index < 0 || index >= m_Asset->Motions.size())
    {
        return;
    }

    //---------------------------------
    // 即切り替え(ブレンドなし)
    //---------------------------------

    if (blendFrame <= 0)
    {
        m_MotionIndex   = index;
        m_KeyIndex      = 0;
        m_FrameCounter  = 0;
        m_IsBlending    = false;

        return;
    }

   //---------------------------------
   // ブレンド開始
   //---------------------------------

    m_IsBlending            = true;
    m_BlendMotionIndex      = index;
    m_BlendKeyIndex         = 0;
    m_BlendFrameCounter     = 0;
    m_BlendTimer            = 0;
    m_BlendDuration         = blendFrame;
}

//=============================================================================
// モーション適用
//=============================================================================
void CCharacterComponent::ApplyMotion(void)
{
    if (!m_Asset)
    {
        return;
    }

    //---------------------------------
    // 現在モーション
    //---------------------------------

    auto& motion =
        m_Asset->Motions[m_MotionIndex];

    if (motion.aKeyInfo.empty())
    {
        return;
    }

    //---------------------------------
    // 現在キー
    //---------------------------------

    auto& key = motion.aKeyInfo[m_KeyIndex];

    //---------------------------------
    // ブレンド中
    //---------------------------------

    if (m_IsBlending)
    {
        //---------------------------------
        // ブレンド先モーション
        //---------------------------------

        auto& blendMotion = m_Asset->Motions[m_BlendMotionIndex];

        auto& blendKey = blendMotion.aKeyInfo[m_BlendKeyIndex];

        //---------------------------------
        // 現在モーション次キー
        //---------------------------------

        int nextKeyIndex = m_KeyIndex + 1;

        if (nextKeyIndex >= motion.aKeyInfo.size())
        {
            nextKeyIndex = motion.bLoop ? 0 : m_KeyIndex;
        }

        auto& nextKey = motion.aKeyInfo[nextKeyIndex];

        //---------------------------------
        // ブレンド先次キー
        //---------------------------------

        int blendNextKeyIndex = m_BlendKeyIndex + 1;

        if (blendNextKeyIndex >= blendMotion.aKeyInfo.size())
        {
            blendNextKeyIndex = blendMotion.bLoop ? 0 : m_BlendKeyIndex;
        }

        auto& blendNextKey = blendMotion.aKeyInfo[blendNextKeyIndex];

        //---------------------------------
        // 補間率
        //---------------------------------

        float currentT = (float)m_FrameCounter / (float)key.nFrame;

        float blendT = (float)m_BlendFrameCounter / (float)blendKey.nFrame;

        float blendRate = (float)m_BlendTimer / (float)m_BlendDuration;

        //---------------------------------
        // パーツ適用
        //---------------------------------

        for (int i = 0; i < m_Transforms.size(); i++)
        {
            //---------------------------------
            // 現在モーション姿勢
            //---------------------------------

            D3DXVECTOR3 curPos = LerpPos(key.aKey[i], nextKey.aKey[i], currentT);
            D3DXVECTOR3 curRot = LerpRot(key.aKey[i], nextKey.aKey[i], currentT);

            //---------------------------------
            // ブレンド先姿勢
            //---------------------------------

            D3DXVECTOR3 nextPos = LerpPos(blendKey.aKey[i], blendNextKey.aKey[i], blendT);
            D3DXVECTOR3 nextRot = LerpRot(blendKey.aKey[i], blendNextKey.aKey[i], blendT);

            //---------------------------------
            // 最終ブレンド
            //---------------------------------

            D3DXVECTOR3 finalPos = curPos + (nextPos - curPos) * blendRate;
            D3DXVECTOR3 finalRot = curRot + (nextRot - curRot) * blendRate;

            //---------------------------------
            // 適用
            //---------------------------------

            m_Transforms[i]->Position = m_Asset->Parts[i].offsetPos + finalPos;

            m_Transforms[i]->Rotation = m_Asset->Parts[i].offsetRot + finalRot;
        }

        //---------------------------------
        // 現在モーション進行
        //---------------------------------

        m_FrameCounter++;

        if (m_FrameCounter >= key.nFrame)
        {
            m_FrameCounter = 0;

            m_KeyIndex++;

            if (m_KeyIndex >= motion.aKeyInfo.size())
            {
                if (motion.bLoop)
                {
                    m_KeyIndex = 0;
                }
                else
                {
                    m_KeyIndex = (int)motion.aKeyInfo.size() - 1;
                }
            }
        }

        //---------------------------------
        // ブレンド先進行
        //---------------------------------

        m_BlendFrameCounter++;

        if (m_BlendFrameCounter >= blendKey.nFrame)
        {
            m_BlendFrameCounter = 0;

            m_BlendKeyIndex++;

            if (m_BlendKeyIndex >= blendMotion.aKeyInfo.size())
            {
                if (blendMotion.bLoop)
                {
                    m_BlendKeyIndex = 0;
                }
                else
                {
                    m_BlendKeyIndex = (int)blendMotion.aKeyInfo.size() - 1;
                }
            }
        }

        //---------------------------------
        // ブレンド進行
        //---------------------------------

        m_BlendTimer++;

        //---------------------------------
        // ブレンド終了
        //---------------------------------

        if (m_BlendTimer >= m_BlendDuration)
        {
            m_IsBlending = false;

            m_MotionIndex = m_BlendMotionIndex;

            m_KeyIndex = m_BlendKeyIndex;

            m_FrameCounter = m_BlendFrameCounter;
        }

        return;
    }

    //---------------------------------
    // 通常再生
    //---------------------------------

    int nextKeyIndex = m_KeyIndex + 1;

    if (nextKeyIndex >= motion.aKeyInfo.size())
    {
        nextKeyIndex = motion.bLoop ? 0 : m_KeyIndex;
    }

    auto& nextKey = motion.aKeyInfo[nextKeyIndex];

    //---------------------------------
    // 補間率
    //---------------------------------

    float t = (float)m_FrameCounter / (float)key.nFrame;

    //---------------------------------
    // パーツ適用
    //---------------------------------

    for (int i = 0; i < m_Transforms.size(); i++)
    {
        D3DXVECTOR3 pos = LerpPos(key.aKey[i], nextKey.aKey[i], t);
        D3DXVECTOR3 rot = LerpRot(key.aKey[i], nextKey.aKey[i], t);

        m_Transforms[i]->Position = m_Asset->Parts[i].offsetPos + pos;

        m_Transforms[i]->Rotation = m_Asset->Parts[i].offsetRot + rot;
    }

    //---------------------------------
    // フレーム進行
    //---------------------------------

    m_FrameCounter++;

    //---------------------------------
    // 次キー
    //---------------------------------

    if (m_FrameCounter >= key.nFrame)
    {
        m_FrameCounter = 0;

        m_KeyIndex++;

        //---------------------------------
        // ループ
        //---------------------------------

        if (m_KeyIndex >= motion.aKeyInfo.size())
        {
            if (motion.bLoop)
            {
                m_KeyIndex = 0;
            }
            else
            {
                m_KeyIndex = (int)motion.aKeyInfo.size() - 1;
            }
        }
    }
}

//=============================================================================
// 位置補間
//=============================================================================
D3DXVECTOR3 CCharacterComponent::LerpPos(
    const CCharacterAsset::KEY_FRAME::PART_KEY& a,
    const CCharacterAsset::KEY_FRAME::PART_KEY& b,
    float t)
{
    return D3DXVECTOR3(
        a.pos.x + (b.pos.x - a.pos.x) * t,
        a.pos.y + (b.pos.y - a.pos.y) * t,
        a.pos.z + (b.pos.z - a.pos.z) * t
    );
}

//=============================================================================
// 回転補間
//=============================================================================
D3DXVECTOR3 CCharacterComponent::LerpRot(
    const CCharacterAsset::KEY_FRAME::PART_KEY& a,
    const CCharacterAsset::KEY_FRAME::PART_KEY& b,
    float t)
{
    auto delta = [](float from, float to)
    {
        float d = to - from;

        if (d > D3DX_PI)
        {
            d -= D3DX_PI * 2.0f;
        }
        else if (d < -D3DX_PI)
        {
            d += D3DX_PI * 2.0f;
        }

        return d;
    };

    return D3DXVECTOR3(
        a.rot.x + delta(a.rot.x, b.rot.x) * t,
        a.rot.y + delta(a.rot.y, b.rot.y) * t,
        a.rot.z + delta(a.rot.z, b.rot.z) * t
    );
}