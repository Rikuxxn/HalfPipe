//=============================================================================
//
// 行列ユーティリティ処理 [MatrixUtility.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _MATRIX_UTILITY_H_// このマクロ定義がされていなかったら
#define _MATRIX_UTILITY_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// 名前空間 MatrixUtil
//*****************************************************************************
namespace MatrixUtil
{
#define INIT_MTX (D3DXMATRIX(1.0f,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f,0.0f, 0.0f,0.0f,1.0f,0.0f, 0.0f,0.0f,0.0f,1.0f ))

	//========================================================================= 
	// 単位行列 
	//========================================================================= 
	
	// 単位行列を取得 
	D3DXMATRIX Identity(void); 

	//========================================================================= 
	// 平行移動 
	//========================================================================= 
	
	// 平行移動行列を作成 
	D3DXMATRIX Translation( float x, float y, float z ); 
	D3DXMATRIX Translation( const D3DXVECTOR3& position ); 

	//=========================================================================
	// 回転 
	//========================================================================= 
	
	// X軸回転 
	D3DXMATRIX RotationX(float angle); 

	// Y軸回転 
	D3DXMATRIX RotationY(float angle); 
	
	// Z軸回転 
	D3DXMATRIX RotationZ(float angle); 
	
	// XYZ回転 
	D3DXMATRIX Rotation(float x, float y, float z ); 
	D3DXMATRIX Rotation(const D3DXVECTOR3& rotation ); 
	
	// クォータニオンから回転行列を作成
	D3DXMATRIX QuaternionToMatrix(const D3DXQUATERNION& quaternion);

	// 回転行列からクォータニオンを作成
	D3DXQUATERNION MatrixToQuaternion(const D3DXMATRIX mtxRot);

	//=========================================================================
	// 拡縮 
	//========================================================================= 
	
	// 拡縮行列 
	D3DXMATRIX Scaling(float x, float y, float z ); 
	D3DXMATRIX Scaling(const D3DXVECTOR3& scale ); 
	
	//========================================================================= 
	// World行列 
	//========================================================================= 
	
	// Scale → Rotation → Translation 
	D3DXMATRIX World(const D3DXVECTOR3& position, const D3DXVECTOR3& rotation, const D3DXVECTOR3& scale ); 
	
	// Scale → Quaternion Rotation → Translation
	D3DXMATRIX QuatWorld(const D3DXVECTOR3& position, const D3DXQUATERNION& quatotation);

	//========================================================================= 
	// View行列 
	//=========================================================================
	
	// LookAt形式のView行列 
	D3DXMATRIX LookAtLH(const D3DXVECTOR3& eye, const D3DXVECTOR3& target, const D3DXVECTOR3& up ); 
	
	//========================================================================= 
	// Projection行列 
	//========================================================================= 
	
	// 透視投影行列 
	D3DXMATRIX PerspectiveFovLH( float fov, float aspect, float nearZ, float farZ ); 
	
	// 正射影行列 
	D3DXMATRIX OrthographicLH( float width, float height, float nearZ, float farZ );
	
	//========================================================================= 
	// 行列操作 
	//========================================================================= 
	
	// 転置 
	D3DXMATRIX Transpose( const D3DXMATRIX& matrix ); 
	
	// 逆行列 
	D3DXMATRIX Inverse( const D3DXMATRIX& matrix ); 
	
	// 行列の乗算 
	D3DXMATRIX Multiply( const D3DXMATRIX& a, const D3DXMATRIX& b ); 
	
	//========================================================================= 
	// 座標変換 
	//========================================================================= 
	
	// 座標を行列で変換 
	D3DXVECTOR3 TransformPoint( const D3DXVECTOR3& position, const D3DXMATRIX& matrix ); 
	
	// 法線・方向ベクトルを行列で変換 
	D3DXVECTOR3 TransformDirection( const D3DXVECTOR3& direction, const D3DXMATRIX& matrix ); 
	
	// ローカルからワールド変換
	D3DXVECTOR3 LocalToWorld(
		const D3DXVECTOR3& localPosition,
		const D3DXVECTOR3& position,
		const D3DXVECTOR3& rotation,
		const D3DXVECTOR3& scale);

	//=========================================================================
	// 行列から情報取得 
	//========================================================================= 
	
	// 行列から位置を取得 
	D3DXVECTOR3 GetPosition( const D3DXMATRIX& matrix ); 
	
	// 行列から回転部分だけを取得
	D3DXMATRIX GetRotationMatrix(const D3DXMATRIX& matrix);

	// 行列から回転をクォータニオンとして取得
	D3DXQUATERNION GetRotationQuaternion(const D3DXMATRIX& matrix);

	// 行列からScaleを取得
	D3DXVECTOR3 GetScale(const D3DXMATRIX& matrix);

	// 行列からRight方向を取得
	D3DXVECTOR3 GetRight( const D3DXMATRIX& matrix ); 
	
	// 行列からUp方向を取得 
	D3DXVECTOR3 GetUp( const D3DXMATRIX& matrix ); 
	
	// 行列からForward方向を取得 
	D3DXVECTOR3 GetForward( const D3DXMATRIX& matrix );


	D3DXQUATERNION QuatSlerp(D3DXQUATERNION quatNow, D3DXQUATERNION quatNext, float fRate);

	// ボーン追従・揺れ処理
	D3DXMATRIX MatrixFollowShaking(
		const D3DXMATRIX& MtxNowOld,
		const D3DXMATRIX& MtxParent,
		const D3DXMATRIX& MtxBoneInit,
		const D3DXMATRIX& MtxParentInit,
		float fShakeRate);
}

#endif