//=============================================================================
//
// プレイヤー処理 [Player.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _PLAYER_H_// このマクロ定義がされていなかったら
#define _PLAYER_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "GameObject.h"
#include "State.h"

//*****************************************************************************
// プレイヤークラス
//*****************************************************************************
class CPlayer : public CGameObject
{
public:
    CPlayer();
    virtual ~CPlayer();

    static constexpr float SPEED        = 180.0f;	// 通常移動時スピード

    HRESULT Init            (void) override;
    HRESULT ConnectInit     (void);
    void    Update          (void) override;

#ifdef _DEBUG
    // Imguiのデバッグ描画
    void DrawDebugGUI(void) override;

#endif

    // 入力データ構造体
    struct InputData
    {
        D3DXVECTOR3 moveDir;	// 前後移動ベクトル
    };

    void SetControlFlag(bool flag) { m_bControlFlag = flag; }

    InputData GatherInput(void);

    bool IsMoving(void) const { return m_bIsMoving; }

    // ステート用のフラグ更新処理
    void UpdateMovementFlags(const D3DXVECTOR3& moveDir);

public:

    // ネットワーク関係
    void SetNetworkControlled(bool enable) { m_NetworkControlled = enable; }
    bool IsNetworkControlled(void) const { return m_NetworkControlled; }
    void SetNetworkInput(const InputData& input) { m_NetworkInput = input; }

    // プレイヤーID
    void SetPlayerId(uint8_t id) { m_PlayerId = id; }
    uint8_t GetPlayerId(void) const { return m_PlayerId; }

    // ローカルプレイヤー
    void SetLocalPlayer(bool enable) { m_IsLocalPlayer = enable; }
    bool IsLocalPlayer(void) const { return m_IsLocalPlayer; }

    // モーション更新
    void UpdateNetworkMotion(bool isMoving);

private:

    void UpdateSurfaceAlignment(float deltaTime);

private:

    // 雪玉アイコン UI
    CGameObject* m_pSnowBallIconUI;

    CGameObject* m_ParticleObject;// パーティクル

    D3DXVECTOR3 m_rotDest;          // 目標の向き
    bool        m_bIsMoving;	    // 移動入力フラグ
    bool        m_bControlFlag;     // 移動許可フラグ

    // ステートを管理するクラスのインスタンス
    StateMachine<CPlayer> m_stateMachine;

    bool m_NetworkControlled = false;
    bool m_IsLocalPlayer = false;
    bool m_NetworkMoving = false;
    InputData m_NetworkInput{};
    uint8_t m_PlayerId = 0;

    // Network interpolation
    D3DXVECTOR3 m_NetworkTargetPosition = INIT_VEC3;
    D3DXVECTOR3 m_NetworkTargetRotation = INIT_VEC3;

    // ハーフパイプの面に沿って向きを変える用
    D3DXVECTOR3     m_SurfaceNormal;        // 面の法線
    D3DXVECTOR3     m_SurfaceForward;       // 面に沿う進行方向
    bool            m_IsGrounded;           // 接地中か
    bool            m_WasGrounded;          // 前フレームの接地状態
    bool            m_AlignToSurface;       // 面に姿勢を合わせるか
};

#endif
