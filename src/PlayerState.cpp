//=============================================================================
//
// プレイヤーの状態処理 [PlayerState.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "PlayerState.h"
#include "Player.h"
#include "Input.h"
#include "InputManager.h"
#include "Manager.h"
#include "CharacterComponent.h"
#include "RigidBodyComponent.h"
#include "TimeSystem.h"
#include "AutoMoveComponent.h"
#include <AimComponent.h>
#include "TransformComponent.h"


//=============================================================================
// 待機状態の開始処理
//=============================================================================
void CPlayerStandState::OnStart(CPlayer* pPlayer)
{
	// キャラクターコンポーネントの取得
	auto character = pPlayer->GetComponent<CCharacterComponent>();

	if (!character)
	{
		return;
	}

	// 待機モーション
	character->PlayMotion(0, 10);
}
//=============================================================================
// 待機状態の更新処理
//=============================================================================
void CPlayerStandState::OnUpdate(CPlayer* pPlayer)
{
	// 入力を取得
	CPlayer::InputData input = pPlayer->GatherInput();

	//// 入力状態を更新
	//pPlayer->UpdateMovementFlags(input.moveDir);

	//// 移動入力されていたら
	//if (pPlayer->IsMoving())
	//{
	//	// 移動状態へ移行
	//	m_pMachine->ChangeState<CPlayerMoveState>();
	//}

	// 自動移動コンポーネントの取得
	auto autoMove = pPlayer->GetComponent<CAutoMoveComponent>();

	// 自動移動中だったら
	if (autoMove && autoMove->IsAutoMove())
	{
		// 移動状態へ移行
		m_pMachine->ChangeState<CPlayerMoveState>();
	}
}


//=============================================================================
// 移動状態の開始処理
//=============================================================================
void CPlayerMoveState::OnStart(CPlayer* pPlayer)
{
	// キャラクターコンポーネントの取得
	auto character = pPlayer->GetComponent<CCharacterComponent>();

	if (!character)
	{
		return;
	}

	// 移動モーション
	character->PlayMotion(1, 10);
}
//=============================================================================
// 移動状態の更新処理
//=============================================================================
void CPlayerMoveState::OnUpdate(CPlayer* pPlayer)
{
	// 入力取得
	CPlayer::InputData input = pPlayer->GatherInput();

	// フラグ更新
	pPlayer->UpdateMovementFlags(input.moveDir);

	// リジッドボディコンポーネントの取得
	auto rigid = pPlayer->GetComponent<CRigidBodyComponent>();

	if (!rigid)
	{
		return;
	}

	//// デルタタイム
	//float deltaTime = CManager::Instance()->GetTimeSystem()->GetDeltaTime();

	// 自動移動コンポーネントの取得
	auto autoMove = pPlayer->GetComponent<CAutoMoveComponent>();

	if (!autoMove)
	{
		return;
	}

	// ジャンプ中ならクレイルグラブ
	if (autoMove->IsJump())
	{
		m_pMachine->ChangeState<CCrailGrabState>();
		return;
	}

	// Press中なら溜め状態
	if (autoMove->IsPress())
	{
		m_pMachine->ChangeState<CPlayerChargeState>();
		return;
	}

	//
	//// 物理移動の設定
	//rigid->MovePhysics(
	//	input.moveDir, 
	//	CPlayer::SPEED, 
	//	10.0f,     // 加速
	//	15.0f,     // 減速
	//	deltaTime);

	//// 移動していなかったら
	//if (!pPlayer->IsMoving())
	//{
	//	// 待機状態へ移行
	//	m_pMachine->ChangeState<CPlayerStandState>();
	//}
}

//=============================================================================
// 溜め状態の開始処理
//=============================================================================
void CPlayerChargeState::OnStart(CPlayer* pPlayer)
{
	// キャラクターコンポーネントの取得
	auto character = pPlayer->GetComponent<CCharacterComponent>();

	if (!character)
	{
		return;
	}

	// 溜めモーション
	character->PlayMotion(2, 10);
}
//=============================================================================
// 溜め状態の更新処理
//=============================================================================
void CPlayerChargeState::OnUpdate(CPlayer* pPlayer)
{
	// 入力を取得
	CPlayer::InputData input = pPlayer->GatherInput();

	// 自動移動コンポーネントの取得
	auto autoMove = pPlayer->GetComponent<CAutoMoveComponent>();

	if (!autoMove)
	{
		return;
	}

	// ジャンプ中ならクレイルグラブ状態へ移行
	if (autoMove->IsJump())
	{
		m_pMachine->ChangeState<CCrailGrabState>();

		return;
	}

	// Pressが解除されたら移動状態へ移行
	if (!autoMove->IsPress())
	{
		m_pMachine->ChangeState<CPlayerMoveState>();

		return;
	}
}

//=============================================================================
// クレイルグラブ状態の開始処理
//=============================================================================
void CCrailGrabState::OnStart(CPlayer* pPlayer)
{
	// キャラクターコンポーネントの取得
	auto character = pPlayer->GetComponent<CCharacterComponent>();

	if (!character)
	{
		return;
	}

	// クレイルグラブモーション
	character->PlayMotion(3, 15);

	//=============================================================
	// ターゲット方向へプレイヤーを向ける
	//=============================================================

	// コンポーネント取得
	auto aim = pPlayer->GetComponent<CAimComponent>();
	auto transform = pPlayer->GetComponent<CTransformComponent>();

	if (!aim || !transform)
	{
		return;
	}

	// プレイヤーの位置
	D3DXVECTOR3 position = transform->Position;

	// ターゲットの中心位置
	D3DXVECTOR3 targetCenter = aim->GetTargetCenter();

	// ターゲットへの方向
	D3DXVECTOR3 direction = targetCenter - position;

	// Y方向は無視（水平方向だけを向く）
	direction.y = 0.0f;

	if (D3DXVec3LengthSq(&direction) < 0.0001f)
	{
		return;
	}

	// 正規化
	D3DXVec3Normalize(&direction, &direction);

	// Y軸回転角度を計算
	float rotY = atan2f(-direction.x, -direction.z);

	// クォータニオン作成
	D3DXQUATERNION quaternion;

	D3DXQuaternionRotationYawPitchRoll(
		&quaternion,
		rotY,
		0.0f,
		0.0f
	);

	// 回転を反映
	transform->Quaternion = quaternion;
	transform->UseQuaternion = true;
}
//=============================================================================
// クレイルグラブ状態の更新処理
//=============================================================================
void CCrailGrabState::OnUpdate(CPlayer* pPlayer)
{
	// 入力を取得
	CPlayer::InputData input = pPlayer->GatherInput();

	// 自動移動コンポーネントの取得
	auto autoMove = pPlayer->GetComponent<CAutoMoveComponent>();

	if (!autoMove->IsJump())
	{
		// 移動状態へ移行
		m_pMachine->ChangeState<CPlayerMoveState>();
	}
}

//=============================================================================
// テールグラブ状態の開始処理
//=============================================================================
void CTailGrabState::OnStart(CPlayer* pPlayer)
{
	// キャラクターコンポーネントの取得
	auto character = pPlayer->GetComponent<CCharacterComponent>();

	if (!character)
	{
		return;
	}

	// テールグラブモーション
	character->PlayMotion(4, 15);
}
//=============================================================================
// テールグラブ状態の更新処理
//=============================================================================
void CTailGrabState::OnUpdate(CPlayer* pPlayer)
{
	// 入力を取得
	CPlayer::InputData input = pPlayer->GatherInput();

	// 自動移動コンポーネントの取得
	auto autoMove = pPlayer->GetComponent<CAutoMoveComponent>();

	if (!autoMove->IsJump())
	{
		// 移動状態へ移行
		m_pMachine->ChangeState<CPlayerMoveState>();
	}
}

//=============================================================================
// トゥイーク状態の開始処理
//=============================================================================
void CTweakState::OnStart(CPlayer* pPlayer)
{
	// キャラクターコンポーネントの取得
	auto character = pPlayer->GetComponent<CCharacterComponent>();

	if (!character)
	{
		return;
	}

	// トゥイークモーション
	character->PlayMotion(5, 15);
}
//=============================================================================
// トゥイーク状態の更新処理
//=============================================================================
void CTweakState::OnUpdate(CPlayer* pPlayer)
{
	// 入力を取得
	CPlayer::InputData input = pPlayer->GatherInput();

	// 自動移動コンポーネントの取得
	auto autoMove = pPlayer->GetComponent<CAutoMoveComponent>();

	if (!autoMove->IsJump())
	{
		// 移動状態へ移行
		m_pMachine->ChangeState<CPlayerMoveState>();
	}
}
