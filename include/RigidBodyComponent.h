//=============================================================================
//
// リジッドボディコンポーネント処理 [RigidBodyComponent.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _RIGIDBODY_COMPONENT_H_// このマクロ定義がされていなかったら
#define _RIGIDBODY_COMPONENT_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Component.h"


//*****************************************************************************
// リジッドボディコンポーネントクラス
//*****************************************************************************
class CRigidBodyComponent : public CComponent
{
public:
    CRigidBodyComponent();
    virtual ~CRigidBodyComponent();

    //=========================================================================
    // 形状の種類
    //=========================================================================
    enum ShapeType
    {
        SHAPE_NONE,
        SHAPE_BOX,
        SHAPE_SPHERE,
        SHAPE_CAPSULE,
        SHAPE_CYLINDER,
        SHAPE_TRIANGLE_MESH,
    };

    HRESULT Init            (void) override;
    void    ReleasePhysics  (void);
    void    Update          (void) override;
    void    Transform       (void);
    void    UpdateFromScale (const D3DXVECTOR3& scale);

    btRigidBody* GetBody(void) const { return m_Body; }

    //=====================================================================
    // 当たり判定生成
    //=====================================================================
    void CreateShape        (btCollisionShape* shape, float mass);      // 当たり判定生成
    void CreateSphere       (float radius, float mass);                 // 球の当たり判定生成
    void CreateBox          (const D3DXVECTOR3& size, float mass);      // ボックスの当たり判定生成
    void CreateCapsule      (float radius, float height, float mass);   // カプセルの当たり判定生成
    void CreateCylinder     (const D3DXVECTOR3& size, float mass);      // シリンダーの当たり判定生成
    void CreateTriangleMesh (btTriangleMesh* triangleMesh);             // メッシュの当たり判定生成

    // 設定の適用
    void ApplySettings(void);

    // 剛体再生成
    void RecreateRigidBody(void);

    // コライダーのオフセット設定
    void SetColliderOffset(const D3DXVECTOR3& offset) { m_ColliderOffset = offset; }

    // 質量の設定・取得
    void SetMass(float mass) { m_Mass = mass; }
    float GetMass(void) const { return m_Mass; }

    // フィルターの設定
    void SetCollisionFilter(short group, short mask)
    {
        m_CollisionGroup = group;
        m_CollisionMask = mask;
    }

    // 移動
    void SetPhysicsVelocity(const D3DXVECTOR3& velocity);
    void MovePhysics(const D3DXVECTOR3& moveDir, float speed, float acceleration, float deceleration, float deltaTime);

    // 動的の設定・取得(デフォルトは静的)
    void SetDynamic(bool dynamic);
    bool IsDynamic(void) const { return m_IsDynamic; }

    // 回転同期の設定・取得
    void SetSyncRotation(bool enable)   { m_SyncRotation = enable; }
    bool GetSyncRotation(void) const    { return m_SyncRotation; }

    // 形状の種類の取得
    ShapeType GetShapeType(void) { return m_ShapeType; }

    D3DXVECTOR3 GetPhysicsVelocity(void) const;

    virtual int       GetCollisionFlags (void) const { return 0; }									// デフォルトはフラグなし

    void SetAngularFactor(const btVector3& factor);

private:

    btCollisionShape*   m_Shape;            // 形状
    btTriangleMesh*     m_TriangleMesh;     // メッシュ    
    btRigidBody*        m_Body;             // 剛体
    ShapeType           m_ShapeType;        // 形状の種類
    D3DXVECTOR3         m_ColliderOffset;   // オフセット
    btVector3           m_linearFactor;     // 線形因子
    btVector3           m_angularFactor;    // 回転因子
    btScalar            m_rollingFriction;  // 回転摩擦
    btScalar            m_friction;         // 摩擦
    short               m_CollisionGroup;   // コリジョングループ
    short               m_CollisionMask;    // コリジョンマスク
    float               m_Mass;             // 質量
    bool                m_IsDynamic;        // 動的かどうか
    bool                m_SyncRotation;     // 回転同期をするか 

private:
    static constexpr float	DEF_MASS             = 2.0f;	// デフォルト質量
    static constexpr float	DEF_ROLLING_FRICTION = 0.2f;	// デフォルト回転摩擦
    static constexpr float	DEF_FRICTION         = 0.5f;	// デフォルト摩擦
    static constexpr float	LINEAR_DAMPING       = 0.1f;	// 線形減衰
    static constexpr float	ANGLAR_DAMPING       = 0.5f;	// 角減衰

};

#endif