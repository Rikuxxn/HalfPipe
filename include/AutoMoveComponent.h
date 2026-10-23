//=============================================================================
//
// 自動移動コンポーネント処理 [AutoMoveComponent.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _AUTO_MOVE_COMPONENT_H_// このマクロ定義がされていなかったら
#define _AUTO_MOVE_COMPONENT_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Component.h"
#include "json.hpp"
#include "JumpTiming.h"


//*****************************************************************************
// 型定義
//*****************************************************************************
using json = nlohmann::json;


//=============================================================================
// 移動ポイントの種類
//=============================================================================
enum class MovePointType
{
    Normal,         // 通常ポイント
    JumpStart,      // タイミング判定を開始する地点
    JumpBranch,     // タイミング結果に応じて分岐する地点
    JumpExtraLarge, // 特大ジャンプポイント
    JumpLarge,      // 大ジャンプポイント
    JumpMedium,     // 中ジャンプポイント
    JumpSmall       // 小ジャンプポイント
};

//=============================================================================
// 移動ポイント
//=============================================================================
struct MovePoint
{
    D3DXVECTOR3 Position = INIT_VEC3;
    MovePointType Type = MovePointType::Normal;
};

//*****************************************************************************
// 自動移動コンポーネントクラス
//*****************************************************************************
class CAutoMoveComponent : public CComponent
{
public:
    CAutoMoveComponent();
    ~CAutoMoveComponent();

    // 自動移動ポイントエディター描画
    void DrawMovePointEditor(void);

    // 更新
    void Update(void) override;

    // 通常の自動移動
    void UpdateAutoMove(float deltaTime);

    // ジャンプ更新
    void UpdateJump(float deltaTime);

    // 自動移動開始
    void StartAutoMove(void)
    {
        m_IsAutoMove = true;
        m_CurrentMovePoint = 1;
    }

    // 自動移動停止
    void StopAutoMove(void)
    {
        m_IsAutoMove = false;
    }

    // ポイント追加
    void AddMovePoint(
        const D3DXVECTOR3& position,
        MovePointType type = MovePointType::Normal);

    // ポイントデバッグモデル生成
    void CreateMovePointObject(const D3DXVECTOR3& pos, MovePointType type);

    // ポイント削除
    void ClearMovePoints(void);

    // 自動移動中か
    bool IsAutoMove(void) const
    {
        return m_IsAutoMove;
    }

    // 進行方向の取得
    D3DXVECTOR3 GetMoveDirection(void) const { return m_MoveDirection; }

    // タイミング結果から移動先の種類を取得
    MovePointType GetJumpTargetType(CJumpTiming::TIMING timing);

    void SaveMovePoint(const char* filename);
    void LoadMovePoint(const char* filename);

private:

    // ポイントの種類に応じてモデルを変更
    void UpdateMovePointModel(size_t index);

    D3DXVECTOR3 Jumppoint(int nCount);

    void JumpTiming(void);

private:

    // 移動ポイント
    std::vector<MovePoint> m_MovePoints;

    // 自動移動ポイントデバッグ表示用
    std::vector<CGameObject*> m_pPointObj;

    // 進行方向
    D3DXVECTOR3 m_MoveDirection;

    // 現在の移動ポイント
    size_t m_CurrentMovePoint;

    // 現在の移動ポイント
    size_t m_CurrentMovePointMinas;


    // 現在の移動ポイント
    size_t m_CurrentMovePointPlus;


    // 現在の移動ポイント
    size_t m_CurrentMovePointPlusPlus;


    // ジャンプ開始ポイント
    size_t m_JumpStartPoint;

    // ジャンプ先ポイント
    size_t m_JumpTargetPoint;

    // 現在のポイント(エディタ用)
    int m_currentPoint;

    // スプライン補間時間
    float m_CurrentTime;

    // 長押し時間
    float m_fPressCount;

    // ジャンプの最終的な高さ
    float m_fJumpHeight;

    // 自動移動中か
    bool m_IsAutoMove;

    // 移動速度
    float m_AutoMoveSpeed;

    // JumpStartで判定を開始済みか
    bool m_JumpStartHandled;

    // タイミング判定中か
    bool m_IsTimingActive;

    // ジャンプポイントへの分岐待ちか
    bool m_IsWaitingForJumpPoint;

    // タイミング結果が確定しているか
    bool m_HasTimingResult;

    // 長押し中か
    bool m_Press;

    // 長押しを放すゾーンにいるか
    bool m_ReleaseZone;

    // 確定したタイミング結果
    CJumpTiming::TIMING m_TimingResult;
};

#endif