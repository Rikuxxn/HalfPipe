//=============================================================================
//
// 投擲コンポーネント処理 [ThrowComponent.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "ThrowComponent.h"
#include "Manager.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "RigidBodyComponent.h"
#include "Ball.h"
#include "MatrixUtility.h"
#include "AimComponent.h"

// 名前空間の使用
using namespace MatrixUtil;


//=============================================================================
// コンストラクタ
//=============================================================================
CThrowComponent::CThrowComponent() :
	m_BallThrowOffset	({ 0.0f, 50.0f, 0.0f }),// ボールのオフセット
	m_ThrowCount		(MAX_DEF_THROW_NUM),	// 投擲数
	m_BallThrowSpeed	(3500.0f)				// ボールの投擲スピード
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CThrowComponent::Init(void)
{
	return S_OK;
}

//=============================================================================
// 更新処理
//=============================================================================
void CThrowComponent::Update(void)
{

}

//=============================================================================
// 投擲処理
//=============================================================================
void CThrowComponent::Throw(void)
{
	// 投擲数が無くなったら
	if (m_ThrowCount <= 0)
	{
		return;
	}

	// 投擲数を減らす
	m_ThrowCount--;

	// シーン取得
	auto scene = CManager::Instance()->GetSceneManager()->GetScene();

	if (!scene)
	{
		return;
	}

	// プレイヤーのトランスフォームコンポーネント
	auto transform = m_pOwner->GetComponent<CTransformComponent>();

	if (!transform)
	{
		return;
	}

	// エイムコンポーネント取得
	auto aim = m_pOwner->GetComponent<CAimComponent>();

	if (!aim)
	{
		return;
	}

	// ボール生成
	auto ball = scene->CreateObject<CBall>();

	if (!ball)
	{
		return;
	}

	// ボールの初期化処理
	ball->Init();

	// 生成位置
	D3DXVECTOR3 spawnPosition = transform->Position + m_BallThrowOffset;

	auto ballTransform = ball->GetComponent<CTransformComponent>();

	if (!ballTransform)
	{
		return;
	}

	ballTransform->Position = spawnPosition;

	auto rigid = ball->GetComponent<CRigidBodyComponent>();

	if (!rigid)
	{
		return;
	}

	// 物理位置の更新
	rigid->Transform();

	// エイム位置を取得
	D3DXVECTOR3 targetPosition = aim->GetAimPosition();

	// エイム位置に向かう方向を計算
	D3DXVECTOR3 direction = targetPosition - spawnPosition;

	// 距離がほぼゼロなら投げない
	if (D3DXVec3LengthSq(&direction) < 0.0001f)
	{
		return;
	}

	// 正規化
	D3DXVec3Normalize(&direction, &direction);

	// 初速を計算
	D3DXVECTOR3 velocity = direction * m_BallThrowSpeed;

	// 投擲
	ball->Throw(targetPosition, spawnPosition);
}
