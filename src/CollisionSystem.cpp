//=============================================================================
//
// 衝突判定システム処理 [CollisionSystem.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "CollisionSystem.h"
#include "GameObject.h"
#include "Manager.h"
#include "GhostObjectManager.h"


//=============================================================================
// 更新処理
//=============================================================================
void CCollisionSystem::Update(void)
{
	// 現在当たっているオブジェクト
	HitMap collisionHits;	// 物理的衝突
	HitMap triggerHits;		// 接触

	// Bulletから現在の衝突取得
	CollectCollision(collisionHits);
	CollectTrigger(triggerHits);

	// イベント更新
	UpdateCollisionEvents(collisionHits);
	UpdateTriggerEvents(triggerHits);

	// 次フレーム用に保存
	m_PreviousHits = collisionHits;
	m_PreviousTriggerHits = triggerHits;
}

//=============================================================================
// 衝突情報のクリア
//=============================================================================
void CCollisionSystem::Clear(void)
{
	m_PreviousHits.clear();
	m_PreviousTriggerHits.clear();
}

//=============================================================================
// 衝突取得(物理的)
//=============================================================================
void CCollisionSystem::CollectCollision(HitMap& currentHits)
{
	auto world = CManager::Instance()->GetPhysicsWorld();

	int manifoldCount = world->getDispatcher()->getNumManifolds();

	for (int i = 0; i < manifoldCount; i++)
	{
		btPersistentManifold* manifold = world->getDispatcher()->getManifoldByIndexInternal(i);

		bool hit = false;

		for (int j = 0; j < manifold->getNumContacts(); j++)
		{
			btManifoldPoint& pt = manifold->getContactPoint(j);

			if (pt.getDistance() < 0.0f)
			{
				hit = true;
				break;
			}
		}

		if (!hit)
		{
			continue;
		}

		// オブジェクトA
		auto objA = manifold->getBody0();

		// オブジェクトB
		auto objB = manifold->getBody1();

		// ゴーストオブジェクトの場合はスキップ
		if (objA->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE)
		{
			continue;
		}
		if (objB->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE)
		{
			continue;
		}

		auto gameA = static_cast<CGameObject*>(objA->getUserPointer());

		auto gameB = static_cast<CGameObject*>(objB->getUserPointer());

		if (!gameA || !gameB)
		{
			continue;
		}

		if (gameA->IsDestroy() || gameB->IsDestroy())
		{
			continue;
		}

		currentHits[gameA].insert(gameB);
		currentHits[gameB].insert(gameA);
	}
}

//=============================================================================
// 衝突取得(接触)
//=============================================================================
void CCollisionSystem::CollectTrigger(HitMap& currentHits)
{
	// ゴーストオブジェクトの取得
	auto ghosts = CManager::Instance()->GetGhostObjectManager()->GetGhosts();

	for (auto ghost : ghosts)
	{
		if (!ghost)
		{
			continue;
		}

		auto trigger = static_cast<CGameObject*>(ghost->getUserPointer());

		if (!trigger)
		{
			continue;
		}

		if (trigger->IsDestroy())
		{
			continue;
		}

		int count = ghost->getNumOverlappingObjects();

		for (int i = 0; i < count; i++)
		{
			auto obj = ghost->getOverlappingObject(i);

			if (!obj)
			{
				continue;
			}

			auto other = static_cast<CGameObject*>(obj->getUserPointer());

			if (!other)
			{
				continue;
			}

			if (other->IsDestroy())
			{
				continue;
			}

			currentHits[trigger].insert(other);
			currentHits[other].insert(trigger);
		}
	}
}

//=============================================================================
// イベント衝突更新(物理的)
//=============================================================================
void CCollisionSystem::UpdateCollisionEvents(const HitMap& currentHits)
{
	//---------------------------------
	// Enter / Stay
	//---------------------------------

	for (auto& pair : currentHits)
	{
		CGameObject* owner = pair.first;

		if (!owner || owner->IsDestroy())
		{
			continue;
		}

		for (auto other : pair.second)
		{
			auto prevItr = m_PreviousHits.find(owner);

			bool isPrevHit = false;

			if (prevItr != m_PreviousHits.end())
			{
				isPrevHit = prevItr->second.find(other) != prevItr->second.end();
			}

			if (!isPrevHit)
			{
				// 前フレームにはいない
				owner->OnCollisionEnter(other);
			}
			else
			{
				// 前フレームにもいる
				owner->OnCollisionStay(other);
			}
		}
	}

	//---------------------------------
	// Exit
	//---------------------------------

	for (auto& pair : m_PreviousHits)
	{
		CGameObject* owner = pair.first;

		if (!owner || owner->IsDestroy())
		{
			continue;
		}

		for (auto other : pair.second)
		{
			if (!other || other->IsDestroy())
			{
				continue;
			}

			auto currentItr = currentHits.find(owner);

			if (currentItr == currentHits.end() ||
				currentItr->second.find(other)
				== currentItr->second.end())
			{
				owner->OnCollisionExit(other);
			}
		}
	}
}

//=============================================================================
// イベント衝突更新(接触)
//=============================================================================
void CCollisionSystem::UpdateTriggerEvents(const HitMap& currentHits)
{
	//---------------------------------
	// Enter / Stay
	//---------------------------------

	for (auto& pair : currentHits)
	{
		CGameObject* owner = pair.first;

		if (!owner || owner->IsDestroy())
		{
			continue;
		}

		for (auto other : pair.second)
		{
			bool isPrevHit = false;

			auto prevItr = m_PreviousTriggerHits.find(owner);

			if (prevItr != m_PreviousTriggerHits.end())
			{
				isPrevHit = prevItr->second.find(other) != prevItr->second.end();
			}

			if (!isPrevHit)
			{
				// Trigger侵入
				owner->OnTriggerEnter(other);
			}
			else
			{
				// Trigger内にいる
				owner->OnTriggerStay(other);
			}
		}
	}

	//---------------------------------
	// Exit
	//---------------------------------

	for (auto& pair : m_PreviousTriggerHits)
	{
		CGameObject* owner = pair.first;

		if (!owner || owner->IsDestroy())
		{
			continue;
		}

		for (auto other : pair.second)
		{
			if (!other || other->IsDestroy())
			{
				continue;
			}

			auto currentItr = currentHits.find(owner);

			if (currentItr == currentHits.end() ||
				currentItr->second.find(other) == currentItr->second.end())
			{
				owner->OnTriggerExit(other);
			}
		}
	}
}