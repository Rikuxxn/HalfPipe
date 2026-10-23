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

// 名前空間の使用
using namespace MatrixUtil;


//=============================================================================
// コンストラクタ
//=============================================================================
CThrowComponent::CThrowComponent() :
	m_BallThrowOffset	({ 0.0f, 1.0f, 0.5f }),	// ボールのオフセット
	m_BallThrowSpeed	(1000.0f)				// ボールの投擲スピード
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

	// ボール生成
	auto ball = scene->CreateObject<CBall>();

	if (!ball)
	{
		return;
	}

	// ボールの初期化処理
	ball->Init();

	// 前方方向の取得
	D3DXVECTOR3 forward = GetForward(transform->GetWorldMatrix());

	// 投擲方向
	D3DXVECTOR3 direction = forward;

	// 正規化
	if (D3DXVec3LengthSq(&direction) < 0.0001f)
	{
		return;
	}

	// 正規化
	D3DXVec3Normalize(&direction, &direction);

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

	rigid->Transform();

	// 初速
	D3DXVECTOR3 velocity = direction * m_BallThrowSpeed;

	// 投擲
	ball->Throw(velocity);
}
