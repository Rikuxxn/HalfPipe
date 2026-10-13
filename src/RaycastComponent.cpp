//=============================================================================
//
// レイキャストコンポーネント処理 [RaycastComponent.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "RaycastComponent.h"
#include "Manager.h"

//=============================================================================
// コンストラクタ
//=============================================================================
CRaycastComponent::CRaycastComponent() :
    m_pHitObject	(nullptr),      // ヒットオブジェクト
	m_HitPosition	(INIT_VEC3),    // 位置
	m_HitNormal		(INIT_VEC3),    // 法線
	m_HitDistance	(0.0f),			// 距離
	m_bHit			(false)         // 当たったかどうか
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CRaycastComponent::Init(void)
{
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void CRaycastComponent::Uninit(void)
{
    m_pHitObject    = nullptr;
    m_bHit          = false;
}

//=============================================================================
// 更新処理
//=============================================================================
void CRaycastComponent::Update(void)
{

}

//=============================================================================
// レイキャスト
//=============================================================================
bool CRaycastComponent::Raycast(const D3DXVECTOR3& origin, const D3DXVECTOR3& direction, float distance)
{
    m_pHitObject    = nullptr;
    m_HitDistance   = 0.0f;
    m_bHit          = false;

    D3DXVECTOR3 dir = direction;

    if (D3DXVec3LengthSq(&dir) <= 0.00001f)
    {
        return false;
    }

    // 正規化
    D3DXVec3Normalize(&dir, &dir);

    D3DXVECTOR3 end = origin + dir * distance;

    btVector3 from(
       origin.x,
       origin.y,
       origin.z);

    btVector3 to(
       end.x,
       end.y,
       end.z);

    // 物理世界の取得
    auto world = CManager::Instance()->GetPhysicsWorld();

    if (!world)
    {
        return false;
    }

    btCollisionWorld::ClosestRayResultCallback rayCallback(from, to);

    world->rayTest( from, to, rayCallback);

    if (!rayCallback.hasHit())
    {
        return false;
    }

    // ヒットしたBulletオブジェクト
    const btCollisionObject* collisionObject = rayCallback.m_collisionObject;

    if (!collisionObject)
    {
        return false;
    }

    // GameObject取得
    m_pHitObject = static_cast<CGameObject*>(collisionObject->getUserPointer());

    // ヒット位置
    const btVector3& hitPoint = rayCallback.m_hitPointWorld;

    m_HitPosition =
    {
        (float)hitPoint.x(),
        (float)hitPoint.y(),
        (float)hitPoint.z()
    };

    // ヒット法線
    const btVector3& hitNormal = rayCallback.m_hitNormalWorld;

    m_HitNormal =
    {
        (float)hitNormal.x(),
        (float)hitNormal.y(),
        (float)hitNormal.z()
    };

    // 距離
    btVector3 hitPointVec = rayCallback.m_hitPointWorld - from;

    m_HitDistance = (float)hitPointVec.length();

    m_bHit = true;

    return true;
}
