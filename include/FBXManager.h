//=============================================================================
// 
// FBXマネージャー [FBXManager.h]
// Author : RIKU TANEKAWA
// 
//=============================================================================
#ifndef _FBX_MANAGER_H_
#define _FBX_MANAGER_H_

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "fbxsdk.h"
#include <vector>
#include <map>
#include "MatrixUtility.h"

// マクロ定義
#define FVF_VERTEX_3D_FBX		(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX3)	// 座標,法線,カラー,テクスチャ座標,ボーンインデックス,ウェイト
#define MAX_BONE_NUM			(30)														// 最大ボーン数
#define MAX_BONE_IDX_NUM		(2)															// 最大ボーンインデックス数
#define KEYWORD_BONE_FOLLOW		("follow")													// ボーンの追従キーワード
#define KEYWORD_SHAKRATE		("_")														// ボーンの追従具合キーワード
#define CONVERT_FACTOR_SHAKRATE	(0.01f)														// 揺れ具合変換係数

// 前方宣言
class CTextureResource;

// 頂点構造体(FBX用)
typedef struct VERTEX_3D_FBX
{
	D3DXVECTOR3	Pos;		// 頂点座標
	D3DXVECTOR3	Nor;		// 法線ベクトル
	D3DCOLOR	Col;		// 頂点カラー
	D3DXVECTOR2	Tex;		// テクスチャ座標
	D3DXVECTOR2	IdxBone;	// ボーンインデックス
	D3DXVECTOR2	Weight;		// ウェイト
}VERTEX_3D_FBX;

//*****************************************************************************
// FBXマネージャクラス
//*****************************************************************************
class CFBXManager
{
public:

	// マテリアルオーダー
	enum MATERIAL_ORDER_LAMBERT
	{
		AMBIENT,
		DIFFUSE,
		EMISSIVE,
		TRANSPARENTCOLOR,
		
		/*REFLECTION,
		SPECULAR,
		SHINIESS,*/
		MAX
	};

	// ボーンタイプ
	typedef enum BONE_TYPE
	{
		BONE_TYPE_NOMAL = 0,	// 通常
		BONE_TYPE_FOLLOW,		// 追従
		BONE_TYPE_MAX
	}BONE_TYPE;

	// マテリアル構造体
	typedef struct MaterialLambertData
	{
		D3DXVECTOR4 Ambient				= {};	// Ambient
		D3DXVECTOR4 Diffuse				= {};	// Diffuse
		D3DXVECTOR4 Emissive			= {};	// Emissive
		D3DXVECTOR4 TransParentColor	= {};	// TransParentColor
		std::shared_ptr<CTextureResource> Texture;
	}MaterialLambertData;

	// ボーン情報
	typedef struct BoneData
	{
		D3DXMATRIX				MtxInit		= {};				// 初期位置マトリック
		std::vector	<int>		IdxCP;							// CPインデックス
		std::vector	<float>		Weight;							// 重さ
		std::string				ParentName;						// 親名
		std::string				ChildName;						// 子名
		BONE_TYPE				Type		= BONE_TYPE_NOMAL;	// タイプ
		float					fShakeRate	= 1.0f;				// 揺れ具合
	}BoneData;

	// メッシュ構造体
	typedef struct MeshData
	{
		LPDIRECT3DVERTEXBUFFER9											VtxBuff;				// 頂点バッファ
		std::vector				<VERTEX_3D_FBX>							Vertices;				// 頂点読み込み情報
		std::map				<std::string, LPDIRECT3DINDEXBUFFER9>	IndexBuffers;			// マテリアル名,インデックスバッファ
		std::map				<std::string, std::vector<UINT>>		Indices;				// マテリアル名,インデックス読込情報
		std::map				<std::string, BoneData>					BoneData;				// ボーン情報
		D3DXMATRIX														MtxMeshInit = INIT_MTX;	// メッシュ初期マトリック
	}MeshData;

	// モーション用ボーン構造体
	typedef struct MotionBoneData
	{
		std::vector<D3DXMATRIX>	FlameMtxList;	// フレーム毎モーション行列リスト
	}MotionBoneData;

	// モーション構造体
	typedef struct MotionData
	{
		std::string									Name;			// 名前
		std::map	<std::string, MotionBoneData>	MotionBoneList;	// モーション行列情報
		int											nFrame;			// フレーム数
		bool										bLoop;			// ループの有無
	}MotionData;

	// モデルデータ
	typedef struct ModelData
	{
		void Uninit(void);	// 破棄処理

		std::map<std::string, MeshData>				MeshData;	// メッシュ名,メッシュデータ
		std::map<std::string,MaterialLambertData>	Materials;	// マテリアル名,マテリアル情報
		std::vector <MotionData>					MotionList;	// モーション行列情報
	}ModelData;

	// 読み込み時引数
	typedef struct ArgumentLoad
	{
		std::string									FBXFilePath;		// FBXのファイルパス
		std::vector<std::pair<std::string,bool>>	FBXMotionFilePath;	// FBXのモーションファイルパス
		std::vector<bool>							IsLoop;				// モーションのループの有無
		bool										bLighting = true;	// ライティングの有無
	}ArgumentLoad;

	std::string	Load (ArgumentLoad& rArg);

	// ゲッター
	static CFBXManager&	GetInstance		(void);						// インスタンス取得処理
	ModelData*			GetModelData	(std::string ModelName);	// モデルデータ取得処理

	// 計算
	void ConvartFbxMatrixToD3DXMAT(FbxAMatrix& rFbxMatrix,D3DXMATRIX& rD3DXMat);	// FbxAMatrixからD3DXMATRIXに変換する処理
	void ConvartFbxTimeToInt(FbxTime& rFbxTime,int& rInt);							// FbxTimeからintに変換する処理

private:
	CFBXManager();
	~CFBXManager();

	void					Uninit								(void);
	std::string				GetPreTexturePath					(std::string FbxFilePath);
	std::string				GetModelName						(std::string FbxFilePath);
	void					CollectMeshNode						(FbxNode* node, std::map<std::string, FbxNode*>& list);
	bool					LoadFbxFile							(ModelData& rModelData, ArgumentLoad& rArg, std::string& rModelName);
	bool					LoadMesh							(ModelData& rModelData,const char* node_name, FbxNode* mesh);
	void					LoadMaterial						(ModelData& rModelData,const char* node_name, FbxMesh* mesh);
	void					LoadVertex							(ModelData& rModelData,const char* node_name, FbxMesh* mesh);
	void					LoadBone							(ModelData& rModelData,const char* node_name, FbxMesh* mesh);// モーション読み込み
	void					LoadMotion							(ModelData& rModelData, std::string& rFBXMotionFilePath);
	bool					CreateVertexBuffer					(ModelData& rModelData);
	bool					CreateIndexBuffer					(ModelData& rModelData);
	static void				GetBoneIdxAndWeightFromIdxCP		(std::vector<int>& rOutIdxBone, std::vector<float>& rOutWeight, MeshData& rMeshData, int nIdxCP);	// コントロールポイントの番号からボーンインデックスと重さを調べる関数

	// 再帰関数
	void RecursionSkeltonNode(FbxNode* pNode,std::map<std::string,FbxNode*>& rBoneList);

	FbxManager*							m_pFbxManager;		// FBXマネージャー
	FbxIOSettings*						m_pFbxIos;			// Ios
	FbxImporter*						m_pFbxImporter;		// インポーター
	FbxScene*							m_pFbxScene;		// Fbxシーン
	std::string							m_PreTexturePath;	// 前のテクスチャパス
	std::map<std::string, ModelData>	m_ModelData;		// モデルデータ
};

#endif