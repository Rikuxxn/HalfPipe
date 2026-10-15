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

	// 自動移動中だったら
	if (pPlayer->IsAutoMoving())
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

	// 自動移動中の場合、自動移動更新
	if (pPlayer->IsAutoMoving())
	{
		pPlayer->UpdateAutoMove();
	}

	//// デルタタイム
	//float deltaTime = CManager::Instance()->GetTimeSystem()->GetDeltaTime();
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
