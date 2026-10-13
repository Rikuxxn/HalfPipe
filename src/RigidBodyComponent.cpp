//=============================================================================
//
// リジッドボディコンポーネント処理 [RigidBodyComponent.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "RigidBodyComponent.h"
#include "Manager.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "MathConstants.h"
#include "MathUtility.h"
#include "CollisionGroup.h"

// 名前空間の使用
using namespace MathUtil;


//=============================================================================
// コンストラクタ
//=============================================================================
CRigidBodyComponent::CRigidBodyComponent() :
	m_Shape				(nullptr),					// 形状
	m_TriangleMesh		(nullptr),					// メッシュ
	m_Body				(nullptr),					// 剛体
	m_ShapeType			(SHAPE_NONE),				// 形状の種類
	m_ColliderOffset	(INIT_VEC3),				// オフセット
	m_linearFactor		(INIT_BT_VEC3),				// 線形因子
	m_angularFactor		(INIT_BT_VEC3),				// 回転因子
	m_rollingFriction	(DEF_ROLLING_FRICTION),		// 回転摩擦
	m_friction			(DEF_FRICTION),				// 摩擦
	m_Mass				(1.0f),						// 質量
	m_CollisionGroup	(COLLISION_GROUP_PLAYER),	// コリジョングループ
	m_CollisionMask		(0xFFFF),					// コリジョンマスク
	m_IsDynamic			(false),					// 動的かどうか
	m_SyncRotation		(true)						// 回転同期をするか
{

}

//=============================================================================
// デストラクタ
//=============================================================================
CRigidBodyComponent::~CRigidBodyComponent()
{
	// 剛体・形状の削除
	ReleasePhysics();
}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CRigidBodyComponent::Init(void)
{

	return S_OK;
}

//=============================================================================
// 剛体・形状の削除
//=============================================================================
void CRigidBodyComponent::ReleasePhysics(void)
{
	// 物理世界の取得
	auto world = CManager::Instance()->GetPhysicsWorld();

	// リジッドボディの破棄
	if (m_Body)
	{
		if (world)
		{
			world->removeRigidBody(m_Body);
		}

		delete m_Body->getMotionState();
		delete m_Body;

		m_Body = nullptr;
	}

	// 形状の破棄
	if (m_Shape)
	{
		if (auto compound = dynamic_cast<btCompoundShape*>(m_Shape))
		{
			for (int i = compound->getNumChildShapes() - 1; i >= 0; --i)
			{
				delete compound->getChildShape(i);
			}
		}

		delete m_Shape;
		m_Shape = nullptr;
	}

	// メッシュの破棄
	if (m_TriangleMesh)
	{
		delete m_TriangleMesh;
		m_TriangleMesh = nullptr;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void CRigidBodyComponent::Update(void)
{
	// トランスフォームコンポーネントの取得
	auto transform = m_pOwner->GetComponent<CTransformComponent>();

	if (!transform || !m_Body)
	{
		return;
	}

	btTransform world;

	if (m_IsDynamic)
	{
		// Bullet → Transform（通常）
		m_Body->getMotionState()->getWorldTransform(world);

		btVector3 pos = world.getOrigin();

		transform->Position = { (float)pos.x(), (float)pos.y(), (float)pos.z() };
		
		// 回転同期をする場合
		if (m_SyncRotation)
		{
			btQuaternion rot = world.getRotation();

			// クオータニオンの使用
			transform->UseQuaternion = true;

			transform->Quaternion = { (float)rot.x(), (float)rot.y(), (float)rot.z(), (float)rot.w() };
		}
	}
	else
	{
		// Kinematic：Transform → Bullet
		btTransform t;
		t.setIdentity();

		t.setOrigin(btVector3(transform->Position.x, transform->Position.y, transform->Position.z));
		t.setRotation(btQuaternion(transform->Quaternion.x, transform->Quaternion.y, transform->Quaternion.z, transform->Quaternion.w));

		m_Body->setWorldTransform(t);

		if (m_Body->getMotionState())
		{
			m_Body->getMotionState()->setWorldTransform(t);
		}
	}
}

//=============================================================================
// スケールによる更新処理
//=============================================================================
void CRigidBodyComponent::UpdateFromScale(const D3DXVECTOR3& scale)
{
	m_Shape->setLocalScaling(btVector3(scale.x, scale.y, scale.z));
}

//=============================================================================
// 当たり判定生成
//=============================================================================
void CRigidBodyComponent::CreateShape(btCollisionShape* shape, float mass)
{
	// オフセットを使用しない場合
	if (m_ColliderOffset.x == 0.0f &&
		m_ColliderOffset.y == 0.0f &&
		m_ColliderOffset.z == 0.0f)
	{
		m_Shape = shape;
	}
	else// オフセットを使用する場合
	{
		auto compound = new btCompoundShape();

		btTransform offset;
		offset.setIdentity();
		offset.setOrigin(btVector3(
			m_ColliderOffset.x,
			m_ColliderOffset.y,
			m_ColliderOffset.z));

		compound->addChildShape(offset, shape);

		m_Shape = compound;
	}

	m_Mass = mass;

	// 動的でなければ質量0で静的扱いにする
	btScalar Mass = m_IsDynamic ? m_Mass : 0.0f;
	btVector3 inertia(0, 0, 0);

	if (Mass > 0.0f)
	{
		m_Shape->calculateLocalInertia(Mass, inertia);
	}

	// トランスフォームコンポーネントの取得
	auto transform = m_pOwner->GetComponent<CTransformComponent>();

	btTransform startTransform;
	startTransform.setIdentity();

	if (transform)
	{
		startTransform.setOrigin(btVector3(transform->Position.x, transform->Position.y, transform->Position.z));

		if (transform->UseQuaternion)
		{
			startTransform.setRotation(btQuaternion(transform->Quaternion.x, transform->Quaternion.y, transform->Quaternion.z, transform->Quaternion.w));
		}
	}

	auto motion = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo info(Mass, motion, m_Shape, inertia);

	// 剛体の生成
	m_Body = new btRigidBody(info);

	// ユーザーポインタの設定
	m_Body->setUserPointer(m_pOwner);

	// 設定の適用
	ApplySettings();

	// 物理世界に追加
	CManager::Instance()->GetPhysicsWorld()->addRigidBody(m_Body, m_CollisionGroup, m_CollisionMask);
}

//=============================================================================
// スフィアコライダー生成
//=============================================================================
void CRigidBodyComponent::CreateSphere(float radius, float mass)
{
	// 初回だけ
	if (m_ShapeType == SHAPE_NONE)
	{
		m_ShapeType = SHAPE_SPHERE;
	}

	// スフィアコライダーの生成
	CreateShape(new btSphereShape(radius), mass);
}

//=============================================================================
// ボックスコライダー生成
//=============================================================================
void CRigidBodyComponent::CreateBox(const D3DXVECTOR3& size, float mass)
{
	// 初回だけ
	if (m_ShapeType == SHAPE_NONE)
	{
		m_ShapeType = SHAPE_BOX;
	}

	// ボックスコライダーの生成
	CreateShape(new btBoxShape(btVector3(size.x * MathConst::HALF, size.y * MathConst::HALF, size.z * MathConst::HALF)),	mass);
}

//=============================================================================
// カプセルコライダー生成
//=============================================================================
void CRigidBodyComponent::CreateCapsule(float radius, float height, float mass)
{
	m_ShapeType = SHAPE_CAPSULE;

	// カプセルコライダーの生成
	CreateShape(new btCapsuleShape(radius, height), mass);
}

//=============================================================================
// メッシュコライダー生成
//=============================================================================
void CRigidBodyComponent::CreateTriangleMesh(btTriangleMesh* triangleMesh)
{
	if (!triangleMesh)
	{
		return;
	}

	m_ShapeType = SHAPE_TRIANGLE_MESH;

	m_TriangleMesh = triangleMesh;

	auto* shape = new btBvhTriangleMeshShape(triangleMesh, true);

	CreateShape(shape, 0.0f);
}

//=============================================================================
// 設定の適用
//=============================================================================
void CRigidBodyComponent::ApplySettings(void)
{
	if (!m_Body)
	{
		return;
	}

	int flags = m_Body->getCollisionFlags();

	m_Body->setActivationState(DISABLE_DEACTIVATION);		// スリープ状態にしない
	m_Body->setCollisionFlags(flags);						// コリジョンフラグ
	m_Body->setLinearFactor(m_linearFactor);				// 移動制限
	m_Body->setAngularFactor(m_angularFactor);				// 回転制限
	m_Body->setFriction(m_friction);						// 摩擦
	m_Body->setRollingFriction(m_rollingFriction);			// 転がり摩擦
	m_Body->setDamping(LINEAR_DAMPING, ANGLAR_DAMPING);		// 減衰(抵抗)linearDamping, angularDamping
}

//=============================================================================
// 動的かどうか
//=============================================================================
void CRigidBodyComponent::SetDynamic(bool dynamic)
{
	if (m_IsDynamic == dynamic)
	{
		return;
	}

	m_IsDynamic = dynamic;

	// 剛体再生成
	RecreateRigidBody();
}

//=============================================================================
// 位置更新
//=============================================================================
void CRigidBodyComponent::Transform(void)
{
	if (!m_Body)
	{
		return;
	}

	btTransform t;
	t.setIdentity();

	// トランスフォームコンポーネントの取得
	auto transform = m_pOwner->GetComponent<CTransformComponent>();

	t.setOrigin(btVector3(transform->Position.x, transform->Position.y, transform->Position.z));
	t.setRotation(btQuaternion(transform->Quaternion.x, transform->Quaternion.y, transform->Quaternion.z, transform->Quaternion.w));

	m_Body->setWorldTransform(t);
	m_Body->getMotionState()->setWorldTransform(t);
}

//=============================================================================
// 剛体再生成
//=============================================================================
void CRigidBodyComponent::RecreateRigidBody(void)
{
	// 物理世界の取得
	auto world = CManager::Instance()->GetPhysicsWorld();

	// 現在位置を保存
	btTransform transform;
	if (m_Body && m_Body->getMotionState())
	{
		m_Body->getMotionState()->getWorldTransform(transform);
	}

	// 破棄
	if (m_Body)
	{
		world->removeRigidBody(m_Body);
		delete m_Body->getMotionState();
		delete m_Body;
		m_Body = nullptr;
	}

	// コライダーがまだないなら作らない
	if (!m_Shape)
	{
		return;
	}

	// inertia再計算
	btScalar mass = m_IsDynamic ? m_Mass : 0.0f;
	btVector3 inertia(0, 0, 0);

	if (m_Shape && mass > 0.0f)
	{
		m_Shape->calculateLocalInertia(mass, inertia);
	}

	// 新規作成
	btDefaultMotionState* motion = new btDefaultMotionState(transform);

	btRigidBody::btRigidBodyConstructionInfo info(mass, motion, m_Shape, inertia);

	// リジッドボディの生成
	m_Body = new btRigidBody(info);

	// ユーザーポインタの設定
	m_Body->setUserPointer(m_pOwner);

	// 設定適用
	ApplySettings();

	// ワールドに追加
	world->addRigidBody(m_Body, m_CollisionGroup, m_CollisionMask);
}

//=============================================================================
// 物理速度を設定
//=============================================================================
void CRigidBodyComponent::SetPhysicsVelocity(const D3DXVECTOR3& velocity)
{
	if (!m_Body)
	{
		return;
	}

	m_Body->setLinearVelocity(btVector3(velocity.x, velocity.y, velocity.z));
}

//=============================================================================
// 物理速度を取得
//=============================================================================
D3DXVECTOR3 CRigidBodyComponent::GetPhysicsVelocity(void) const
{
	if (!m_Body)
	{
		return INIT_VEC3;
	}

	const btVector3 velocity = m_Body->getLinearVelocity();

	return D3DXVECTOR3(velocity.x(), velocity.y(), velocity.z());
}

//=============================================================================
// 物理移動
//=============================================================================
void CRigidBodyComponent::MovePhysics(
	const D3DXVECTOR3& moveDir, 
	float speed, 
	float acceleration,
	float deceleration,
	float deltaTime)
{
	if (!m_Body)
	{
		return;
	}

	// 現在速度を取得
	btVector3 velocity = m_Body->getLinearVelocity();

	// 移動入力があるか
	bool isMoving = moveDir.x != 0.0f || moveDir.z != 0.0f;

	//=========================================================
	// 目標速度
	//=========================================================

	btVector3 targetVelocity(0.0f, velocity.getY(), 0.0f);

	if (isMoving)
	{
		targetVelocity.setX(moveDir.x * speed);
		targetVelocity.setZ(moveDir.z * speed);
	}

	//=========================================================
	// 現在速度 → 目標速度へ補間
	//=========================================================

	float rate = isMoving ? acceleration : deceleration;

	float t = rate * deltaTime;

	// 1.0を超えないようにする
	t = Min(t, 1.0f);

	float currentX = velocity.getX();
	float currentZ = velocity.getZ();

	float newX = currentX + (targetVelocity.getX() - currentX) * t;
	float newZ = currentZ + (targetVelocity.getZ() - currentZ) * t;

	//=========================================================
	// 速度設定
	//=========================================================

	m_Body->setLinearVelocity(btVector3(newX, velocity.getY(), newZ));
}

//=============================================================================
// 回転因子(回転できる方向)の設定
//=============================================================================
void CRigidBodyComponent::SetAngularFactor(const btVector3& factor)
{
	m_angularFactor = factor;

	if (m_Body)
	{
		m_Body->setAngularFactor(m_angularFactor);
	}
}