//=============================================================================
//
// ゴーストオブジェクトコンポーネント処理 [GhostObjectComponent.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "GhostObjectComponent.h"
#include "Manager.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "GhostObjectManager.h"
#include "MathConstants.h"
#include "CollisionGroup.h"


//=============================================================================
// コンストラクタ
//=============================================================================
CGhostObjectComponent::CGhostObjectComponent() :
    m_Ghost             (nullptr),                  // ゴーストオブジェクト
    m_Shape             (nullptr),                  // コリジョンシェイプ
    m_CollisionGroup    (COLLISION_GROUP_PLAYER),   // コリジョングループ
    m_CollisionMask     (0xFFFF)                    // コリジョンマスク
{

}

//=============================================================================
// デストラクタ
//=============================================================================
CGhostObjectComponent::~CGhostObjectComponent()
{
    ReleaseGhostObject();
}

//=============================================================================
// 破棄
//=============================================================================
void CGhostObjectComponent::ReleaseGhostObject(void)
{
    // 物理世界の取得
    auto world = CManager::Instance()->GetPhysicsWorld();

    if (m_Ghost)
    {
        CManager::Instance()->GetGhostObjectManager()->RemoveGhost(m_Ghost);

        // 物理世界から削除
        world->removeCollisionObject(m_Ghost);

        delete m_Ghost;
        m_Ghost = nullptr;
    }

    delete m_Shape;
    m_Shape = nullptr;
}

//=============================================================================
// 更新
//=============================================================================
void CGhostObjectComponent::Update(void)
{
    if (!m_Ghost)
    {
        return;
    }

    // トランスフォームコンポーネントの取得
    auto transform = m_pOwner->GetComponent<CTransformComponent>();

    btTransform t;
    t.setIdentity();

    t.setOrigin(btVector3(
        transform->Position.x,
        transform->Position.y,
        transform->Position.z));

    t.setRotation(btQuaternion(
        transform->Quaternion.x,
        transform->Quaternion.y,
        transform->Quaternion.z,
        transform->Quaternion.w));

    m_Ghost->setWorldTransform(t);
}

//=============================================================================
// スケールによる更新処理
//=============================================================================
void CGhostObjectComponent::UpdateFromScale(const D3DXVECTOR3& scale)
{
    m_Shape->setLocalScaling(btVector3(scale.x, scale.y, scale.z));
}

//=============================================================================
// 位置更新
//=============================================================================
void CGhostObjectComponent::Transform(void)
{
    if (!m_Ghost)
    {
        return;
    }

    btTransform t;
    t.setIdentity();

    // トランスフォームコンポーネントの取得
    auto transform = m_pOwner->GetComponent<CTransformComponent>();

    t.setOrigin(btVector3(transform->Position.x, transform->Position.y, transform->Position.z));
    t.setRotation(btQuaternion(transform->Quaternion.x, transform->Quaternion.y, transform->Quaternion.z, transform->Quaternion.w));

    m_Ghost->setWorldTransform(t);
}

//=============================================================================
// ボックス
//=============================================================================
void CGhostObjectComponent::CreateBox(const D3DXVECTOR3& size)
{
    // 名前空間の使用
    using namespace MathConst;

    // シェイプの生成
    m_Shape = new btBoxShape(btVector3(size.x * HALF, size.y * HALF, size.z * HALF));

    // ゴーストオブジェクトの生成
    m_Ghost = new btPairCachingGhostObject();

    btTransform t;
    t.setIdentity();

    // トランスフォームコンポーネントの取得
    auto transform = m_pOwner->GetComponent<CTransformComponent>();

    t.setOrigin(btVector3(transform->Position.x, transform->Position.y, transform->Position.z));

    m_Ghost->setWorldTransform(t);

    m_Ghost->setCollisionShape(m_Shape);

    // ユーザーポインタの設定
    m_Ghost->setUserPointer(m_pOwner);

    // コリジョンフラグ
    m_Ghost->setCollisionFlags(m_Ghost->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);

    // 物理世界の取得
    auto world = CManager::Instance()->GetPhysicsWorld();

    // 追加
    world->addCollisionObject(m_Ghost, m_CollisionGroup, m_CollisionMask);

    // 登録
    CManager::Instance()->GetGhostObjectManager()->AddGhost(m_Ghost);
}