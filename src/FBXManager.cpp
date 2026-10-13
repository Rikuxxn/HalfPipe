//=============================================================================
// 
// FBXマネージャー [FBXManager.cpp]
// Author : RIKU TANEKAWA
// 
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "FBXManager.h"
#include "ShaderManager.h"
#include "TextureManager.h"
#include "Manager.h"
#include "Renderer.h"
#include <algorithm>

// 名前空間を使う
using namespace std;

//=============================================================================
// コンストラクタ
//=============================================================================
CFBXManager::CFBXManager():
	m_pFbxManager	(nullptr),	// FBXマネージャー
	m_pFbxIos		(nullptr),	// Ios
	m_pFbxImporter	(nullptr),	// インポーター
	m_pFbxScene		(nullptr)	// Fbxシーン1
{
	// モデル無しを読み込み
	ArgumentLoad Arg;
	Arg.FBXFilePath = "data/MODELS/Fbx/test2.fbx";
	Load(Arg);
}

//=============================================================================
// デストラクタ
//=============================================================================
CFBXManager::~CFBXManager()
{
	// Fbxの破棄
	Uninit();
}

//=============================================================================
// 読み込み処理
//=============================================================================
std::string CFBXManager::Load(ArgumentLoad& rArg)
{
	// モデル名取得
	string ModelName = GetModelName(rArg.FBXFilePath);
	
	// モデルリストからモデルを探す
	auto Iter = m_ModelData.find(ModelName);

	// モデルがすでにあればこれを返す
	if (Iter != m_ModelData.end())
	{
		return ModelName;
	}

	// 途中までのパス取得
	m_PreTexturePath = GetPreTexturePath(rArg.FBXFilePath);

	// FBXFile読み込み
	if (LoadFbxFile(m_ModelData[ModelName], rArg, ModelName) == false)
	{
		// 消す
		m_ModelData.erase(std::prev(m_ModelData.end()));
		return "";
	}

	// VertexBuffer作成
	if (CreateVertexBuffer(m_ModelData[ModelName]) == false)
	{
		// 消す
		m_ModelData.erase(std::prev(m_ModelData.end()));
		return "";
	}

	// IndexBuffer作成
	if (CreateIndexBuffer(m_ModelData[ModelName]) == false)
	{
		// 消す
		m_ModelData.erase(std::prev(m_ModelData.end()));
		return "";
	}

	return ModelName;
}

//=============================================================================
// 前のテクスチャパスを抜き取る処理
//=============================================================================
std::string CFBXManager::GetPreTexturePath(std::string FbxFilePath)
{
	// パスの取得
	std::string PrePath = FbxFilePath;

	// パスの長さ分回す(後ろから)
	while (PrePath.size() > 0)
	{
		// /がであれば
		if (PrePath.back() == '/')
		{
			// 処理を抜け出す
			break;
		}

		// 後ろから消して行く
		PrePath.pop_back();
	}

	// パスを返す
	return PrePath;
}

//=============================================================================
// モデル名を抜き取る処理
//=============================================================================
std::string CFBXManager::GetModelName(std::string FbxFilePath)
{
	// 名前の取得
	std::string Name;

	// ファイルパスの長さ分回す
	for (int nCntPath = 0; nCntPath < (int)FbxFilePath.size(); nCntPath++)
	{
		// 一文字ずつ入れる
		Name += FbxFilePath[nCntPath];

		// /が見つかった
		if (Name.back() == '/')
		{
			// 文字列を空にする
			Name.clear();
		}
	}

	// 名前を返す
	return Name;
}

//=============================================================================
// FBXファイルの読み込み
//=============================================================================
bool CFBXManager::LoadFbxFile(ModelData& rModelData, ArgumentLoad& rArg, std::string& /*rModelName*/)
{
	// FbxManager作成
	FbxManager* fbx_manager = fbxsdk::FbxManager::Create();
	if (fbx_manager == nullptr)
	{
		return false;
	}

	// FbxImporter作成
	FbxImporter* fbx_importer = FbxImporter::Create(fbx_manager, "");
	if (fbx_importer == nullptr)
	{
		fbx_manager->Destroy();
		return false;
	}

	// FbxScene作成
	m_pFbxScene = FbxScene::Create(fbx_manager, "");
	if (m_pFbxScene == nullptr)
	{
		fbx_importer->Destroy();
		fbx_manager->Destroy();
		return false;
	}

	// Fileを初期化
	if (fbx_importer->Initialize(rArg.FBXFilePath.c_str()) == false)
	{
		return false;
	}

	// sceneにインポート
	fbx_importer->Import(m_pFbxScene);

	// ポリゴンを三角形にする
	FbxGeometryConverter converter(fbx_manager);
	converter.Triangulate(m_pFbxScene, true);

	// メッシュNodeを探す
	std::map<std::string, FbxNode*> mesh_node_list;
	CollectMeshNode(m_pFbxScene->GetRootNode(), mesh_node_list);

	// メッシュの数分回す
	for (auto data : mesh_node_list)
	{
		// mesh作成
		LoadMesh(rModelData,data.first.c_str(), data.second);
	}

	// モーションファイルの数分回す
	for (auto IterMotinFile : rArg.FBXMotionFilePath)
	{
		// モーションの読み込み
		LoadMotion(rModelData, IterMotinFile.first);

		// ループの有無を設定
		rModelData.MotionList.back().bLoop = IterMotinFile.second;
	}

	fbx_importer->Destroy();
	m_pFbxScene->Destroy();
	fbx_manager->Destroy();

	return true;
}

//=============================================================================
// 頂点バッファの生成
//=============================================================================
bool CFBXManager::CreateVertexBuffer(ModelData& rModelData)
{
	// メッシュの数分回す
	for (auto &IterMeshData : rModelData.MeshData)
	{
		// 頂点バッファの生成
		HRESULT Hr = CManager::Instance()->GetRenderer()->GetDevice()->CreateVertexBuffer(
			(UINT)(sizeof(VERTEX_3D_FBX) * IterMeshData.second.Vertices.size()),
			0,
			FVF_VERTEX_3D_FBX,
			D3DPOOL_MANAGED,
			&IterMeshData.second.VtxBuff,
			nullptr);

		// 生成出来なかった
		if (FAILED(Hr))
		{
			MessageBox(NULL, "CFBXManager::CreateIndexBuffer > 頂点バッファをクリエイトできませんでした", "Error", MB_OK);
			continue;
		}

		// 頂点バッファをロック
		VERTEX_3D_FBX* pVtx;
		IterMeshData.second.VtxBuff->Lock(0,0,(void**)&pVtx,0);
		
		// 頂点の数分回す
		for (int nCntVtx = 0; nCntVtx < (int)IterMeshData.second.Vertices.size(); nCntVtx++)
		{
			// 頂点データをコピー
			pVtx[nCntVtx] = IterMeshData.second.Vertices[nCntVtx];
		}

		// 頂点バッファをアンロック
		IterMeshData.second.VtxBuff->Unlock();
	}

	return true;
}

//=============================================================================
// インデックスバッファの生成
//=============================================================================
bool CFBXManager::CreateIndexBuffer(ModelData& rModelData)
{
	// メッシュの数分回す
	for (auto &IterMeshData : rModelData.MeshData)
	{
		// インデックスバッファの数分回す
		for (auto& IterIndices : IterMeshData.second.Indices)
		{
			_D3DFORMAT format = _D3DFORMAT::D3DFMT_INDEX32;

			// 頂点バッファの生成
			HRESULT Hr = CManager::Instance()->GetRenderer()->GetDevice()->CreateIndexBuffer(
				(UINT)(sizeof(UINT) * IterIndices.second.size()),
				D3DUSAGE_WRITEONLY,
				format,
				D3DPOOL_MANAGED,
				&IterMeshData.second.IndexBuffers[IterIndices.first],
				nullptr);

			// 生成出来なかった
			if (FAILED(Hr))
			{
				MessageBox(NULL, "CFBXManager::CreateIndexBuffer > インデックスバッファをクリエイトできませんでした", "Error", MB_OK);
				continue;
			}

			// インデックスバッファをロック
			UINT* pIdx;
			IterMeshData.second.IndexBuffers[IterIndices.first]->Lock(0,0,(void**)&pIdx,0);

			// インデックスデータをコピー
			for (int nCntIdx = 0; nCntIdx < (int)IterIndices.second.size(); nCntIdx++)
			{
				pIdx[nCntIdx] = IterIndices.second[nCntIdx];
			}

			// インデックスバッファをアンロック
			IterMeshData.second.IndexBuffers[IterIndices.first]->Unlock();
		}
	}

	return true;
}

//=============================================================================
// メッシュノード探索
//=============================================================================
void CFBXManager::CollectMeshNode(FbxNode* node, std::map<std::string, FbxNode*>& list)
{
	for (int i = 0; i < node->GetNodeAttributeCount(); i++)
	{
		FbxNodeAttribute* attribute = node->GetNodeAttributeByIndex(i);

		// Attributeがメッシュなら追加
		if (attribute->GetAttributeType() == FbxNodeAttribute::EType::eMesh)
		{
			list[node->GetName()] = node;
			list[node->GetName()]->GetMaterial(0)->GetName();
			break;
		}
	}

	for (int i = 0; i < node->GetChildCount(); i++)
	{
		CollectMeshNode(node->GetChild(i), list);
	}
}

//**************************************************
// メッシュの作成
//**************************************************
bool CFBXManager::LoadMesh(ModelData& rModelData, const char* node_name, FbxNode* mesh)
{
	// マテリアルの読み込み
	LoadMaterial(rModelData, node_name, mesh->GetMesh());

	// ボーンの読み込み
	LoadBone(rModelData, node_name, mesh->GetMesh());

	// 頂点の読み込み
	LoadVertex(rModelData, node_name, mesh->GetMesh());

	return true;
}

//=============================================================================
// マテリアルの作成
//=============================================================================
void CFBXManager::LoadMaterial(ModelData& rModelData, const char* /*node_name*/, FbxMesh* mesh)
{
	// ノード取得
	FbxNode* pNode = mesh->GetNode();

	// 無ければ処理しない
	if (pNode == nullptr)
	{
		return;
	}

	// メッシュのマテリアル数の取得
	int nNumMaterial = pNode->GetSrcObjectCount<FbxSurfaceMaterial>();

	// エレメントチェックリスト
	const char* ElementCheckList[] =
	{
		FbxSurfaceMaterial::sAmbient,
		FbxSurfaceMaterial::sDiffuse,
		FbxSurfaceMaterial::sEmissive,
		FbxSurfaceMaterial::sTransparentColor,
	};

	// ファクターチェックリスト
	const char* FactorCheckList[] =
	{
		FbxSurfaceMaterial::sAmbientFactor,
		FbxSurfaceMaterial::sDiffuseFactor,
		FbxSurfaceMaterial::sEmissiveFactor,
		FbxSurfaceMaterial::sTransparencyFactor,
	};

	// マテリアル数分回す
	for (int nCntMat = 0; nCntMat < nNumMaterial; nCntMat++)
	{
		// マテリアルの取得
		FbxSurfaceMaterial* pSurfMaterial = pNode->GetSrcObject<FbxSurfaceMaterial>(nCntMat);

		// FbxSurfaceLambertクラスかどうか調べる
		if (pSurfMaterial->GetClassId().Is(FbxSurfaceLambert::ClassId))
		{
			// マテリアル
			MaterialLambertData Material;

			// 取得用変数
			FbxDouble3 GetColors[MATERIAL_ORDER_LAMBERT::MAX] = {};
			FbxDouble GetFactors[MATERIAL_ORDER_LAMBERT::MAX] = { 1.0,1.0 ,1.0,1.0 };

			// オーダー分回す
			for (int nCntOrder = 0; nCntOrder < (int)MATERIAL_ORDER_LAMBERT::MAX; nCntOrder++)
			{
				// エレメントのプロパティを見つける
				FbxProperty Prop = pSurfMaterial->FindProperty(ElementCheckList[nCntOrder]);

				// 有効チェック
				if (Prop.IsValid() == true)
				{
					GetColors[nCntOrder] = Prop.Get<FbxDouble3>();
					
					// LayeredTexture数の取得
					int nNumLayeredTexture = Prop.GetSrcObjectCount<FbxLayeredTexture>();

					// LayeredTextureがある
					if (0 < nNumLayeredTexture)
					{
						// FileTexture数分回す
						for (int nCntLayTex = 0; nCntLayTex < nNumLayeredTexture; nCntLayTex++)
						{
							// テクスチャを取得する
							FbxLayeredTexture* pFbxLayeredTexture = Prop.GetSrcObject<FbxLayeredTexture>(nCntLayTex);

							// 無ければ処理しない
							if (pFbxLayeredTexture == nullptr)
							{
								continue;
							}

							// FileTexture数の取得
							int nNumFileTexture = pFbxLayeredTexture->GetSrcObjectCount<FbxFileTexture>();

							// FileTexture数分回す
							for (int nCntFileTex = 0; nCntFileTex < nNumFileTexture; nCntFileTex++)
							{
								// FbxFileTextureの取得
								FbxFileTexture* pFbxFileTexture = pFbxLayeredTexture->GetSrcObject<FbxFileTexture>(nCntFileTex);

								if (pFbxFileTexture == nullptr)
								{
									continue;
								}

								// テクスチャパス
								std::string TexturePath = pFbxFileTexture->GetRelativeFileName();
								
								if (TexturePath.empty())
								{
									TexturePath = pFbxFileTexture->GetFileName();
								}

								std::replace(TexturePath.begin(), TexturePath.end(), '\\', '/');

								std::string FullTexturePath = m_PreTexturePath + TexturePath;

								// テクスチャの読み込み
								Material.Texture = CManager::Instance()->GetTextureManager()->Load(FullTexturePath);

								// UVSet名を取得
								std::string UVSetName = pFbxFileTexture->UVSet.Get().Buffer();
							}
						}
					}
					// LayeredTextureがない
					else
					{
						// テクスチャを取得する
						FbxFileTexture* pFbxFileTexture = Prop.GetSrcObject<FbxFileTexture>();

						// 無ければ処理しない
						if (pFbxFileTexture == nullptr)
						{
							continue;
						}

						// テクスチャパス
						std::string TexturePath = pFbxFileTexture->GetRelativeFileName();

						if (TexturePath.empty())
						{
							TexturePath = pFbxFileTexture->GetFileName();
						}

						std::replace(TexturePath.begin(), TexturePath.end(), '\\', '/');

						// フルパス作成
						std::string FullTexturePath = m_PreTexturePath + TexturePath;

						// テクスチャ読み込み
						Material.Texture = CManager::Instance()->GetTextureManager()->Load(FullTexturePath);

						// UVSet名
						std::string UVSetName = pFbxFileTexture->UVSet.Get().Buffer();
					}
				}
				else
				{
					GetColors[nCntOrder] = FbxDouble3(1.0, 1.0, 1.0);
				}

				// ファクターのプロパティを見つける
				Prop = pSurfMaterial->FindProperty(FactorCheckList[nCntOrder]);

				// 有効チェック
				if (Prop.IsValid() == true)
				{
					GetFactors[nCntOrder] = Prop.Get<FbxDouble>();
				}
				else
				{
					GetFactors[nCntOrder] = 1.0;
				}
			}

			// マテリアルの追加
			Material.Ambient.x = (float)GetColors[MATERIAL_ORDER_LAMBERT::AMBIENT][0];
			Material.Ambient.y = (float)GetColors[MATERIAL_ORDER_LAMBERT::AMBIENT][1];
			Material.Ambient.z = (float)GetColors[MATERIAL_ORDER_LAMBERT::AMBIENT][2];
			Material.Ambient.w = (float)GetFactors[MATERIAL_ORDER_LAMBERT::AMBIENT];

			Material.Diffuse.x = (float)GetColors[MATERIAL_ORDER_LAMBERT::DIFFUSE][0];
			Material.Diffuse.y = (float)GetColors[MATERIAL_ORDER_LAMBERT::DIFFUSE][1];
			Material.Diffuse.z = (float)GetColors[MATERIAL_ORDER_LAMBERT::DIFFUSE][2];
			Material.Diffuse.w = (float)GetFactors[MATERIAL_ORDER_LAMBERT::DIFFUSE];

			Material.Emissive.x = (float)GetColors[MATERIAL_ORDER_LAMBERT::EMISSIVE][0];
			Material.Emissive.y = (float)GetColors[MATERIAL_ORDER_LAMBERT::EMISSIVE][1];
			Material.Emissive.z = (float)GetColors[MATERIAL_ORDER_LAMBERT::EMISSIVE][2];
			Material.Emissive.w = (float)GetFactors[MATERIAL_ORDER_LAMBERT::EMISSIVE];

			Material.TransParentColor.x = (float)GetColors[MATERIAL_ORDER_LAMBERT::TRANSPARENTCOLOR][0];
			Material.TransParentColor.y = (float)GetColors[MATERIAL_ORDER_LAMBERT::TRANSPARENTCOLOR][1];
			Material.TransParentColor.z = (float)GetColors[MATERIAL_ORDER_LAMBERT::TRANSPARENTCOLOR][2];
			Material.TransParentColor.w = (float)GetFactors[MATERIAL_ORDER_LAMBERT::TRANSPARENTCOLOR];

			// マテリアル名取得
			char* pMatNameAnsi;
			std::string MatNameUTF8 = pSurfMaterial->GetName();
			FbxUTF8ToAnsi(MatNameUTF8.c_str(), pMatNameAnsi);

			// マテリアルの追加
			rModelData.Materials[pMatNameAnsi] = Material;

			// 解放
			FbxFree(pMatNameAnsi);
		}
		else
		{
			//int a = 0;
		}
	}
}

//=============================================================================
// 頂点の作成
//=============================================================================
void CFBXManager::LoadVertex(ModelData& rModelData, const char* node_name, FbxMesh* mesh)
{
	// ポリゴン数取得
	int nNumPolygon = mesh->GetPolygonCount();
	
	// ポリゴン数分回す
	for (int nCntPoly = 0; nCntPoly < nNumPolygon; nCntPoly++)
	{	
		// ポリゴンの頂点情報取得
		for (int nCntVtx = 0; nCntVtx < 3; nCntVtx++)
		{
			// インデックス取得
			int nIdxVtx = mesh->GetPolygonVertex(nCntPoly,nCntVtx);
		
			// 位置取得
			FbxVector4 FbxV4Pos = mesh->GetControlPointAt(nIdxVtx);

			// 法線取得
			FbxVector4 FbxV4Nor = {0.0 ,0.0 , 0.0 , 0.0};
			mesh->GetPolygonVertexNormal(nCntPoly, nCntVtx, FbxV4Nor);

			// UV座標の取得
			FbxVector2 FbxV2UV;
			bool bUnMapped;
			FbxGeometryElementUV* pGeoEleUV = mesh->GetElementUV();
			if (pGeoEleUV != nullptr)
			{
				mesh->GetPolygonVertexUV(nCntPoly, nCntVtx, pGeoEleUV->GetName(), FbxV2UV, bUnMapped);
			}

			// 頂点カラーデータの取得
			FbxColor FbxCol;
			D3DXCOLOR Col = {1.0f,1.0f, 1.0f, 1.0f};
			FbxGeometryElementVertexColor* pColorBuff = mesh->GetElementVertexColor();

			// データがあれば
			if(pColorBuff != nullptr)
			{
				// EMappingModeを取得する
				FbxLayerElement::EMappingMode MappingMode = pColorBuff->GetMappingMode();

				// EReferenceModeを取得する
				FbxLayerElement::EReferenceMode ReferenceMode = pColorBuff->GetReferenceMode();

				// モードチェック
				if (MappingMode == FbxLayerElement::eByPolygonVertex)
				{
					// 参照方法チェック
					if (ReferenceMode == FbxLayerElement::eIndexToDirect)
					{
						int nIdxCol = pColorBuff->GetIndexArray().GetAt(nCntPoly * 3 + nCntVtx);
						FbxCol = pColorBuff->GetDirectArray().GetAt(nIdxCol);
						Col.r = (float)FbxCol[0];
						Col.g = (float)FbxCol[1];
						Col.b = (float)FbxCol[2];
						Col.a = (float)FbxCol[3];
					}
				}
			}

			// ボーンインデックスと重さ取得
			std::vector<int> IdxBone;
			std::vector<float> Weight;
			GetBoneIdxAndWeightFromIdxCP(IdxBone, Weight,rModelData.MeshData[node_name],nIdxVtx);

			// 移し替える
			VERTEX_3D_FBX Vtx3D;
			Vtx3D.Pos.x = (float)-FbxV4Pos[0];
			Vtx3D.Pos.y = (float)FbxV4Pos[1];
			Vtx3D.Pos.z = (float)FbxV4Pos[2];
			Vtx3D.Nor.x = (float)-FbxV4Nor[0];
			Vtx3D.Nor.y = (float)FbxV4Nor[1];
			Vtx3D.Nor.z = (float)FbxV4Nor[2];
			Vtx3D.Tex.x = (float)FbxV2UV[0];
			Vtx3D.Tex.y = 1.0f - (float)FbxV2UV[1];
			Vtx3D.Col = Col;
			Vtx3D.IdxBone.x = (float)IdxBone[0];
			Vtx3D.IdxBone.y = (float)IdxBone[1];
			Vtx3D.Weight.x = Weight[0];
			Vtx3D.Weight.y = Weight[1];

			// 読み込んだ頂点情報追加
			rModelData.MeshData[node_name].Vertices.push_back(Vtx3D);
		}
	}

	// マテリアル情報取得
	FbxLayerElementMaterial* pMaterialElement = mesh->GetElementMaterial();

	// ノード取得
	FbxNode* pNode = mesh->GetNode();

	//// メッシュのマテリアル数の取得
	//int nNumMaterial = pNode->GetSrcObjectCount<FbxSurfaceMaterial>();

	// ポリゴンごとのマテリアルインデックス配列を取得
	const FbxLayerElementArrayTemplate<int>& indexArray = pMaterialElement->GetIndexArray();

	// ポリゴン数分回す
	for (int nCntPoly = 0; nCntPoly < nNumPolygon; nCntPoly++)
	{
		// ポリゴン i が使用しているマテリアルのインデックス
		int materialIndex = indexArray.GetAt(nCntPoly);

		// マテリアルの取得
		FbxSurfaceMaterial* pSurfMaterial = pNode->GetSrcObject<FbxSurfaceMaterial>(materialIndex);

		// マテリアル名取得
		char* pMatNameAnsi;
		std::string MatNameUTF8 = pSurfMaterial->GetName();
		FbxUTF8ToAnsi(MatNameUTF8.c_str(), pMatNameAnsi);

		// ポリゴンの数だけ連番として保存する
		rModelData.MeshData[node_name].Indices[pMatNameAnsi].push_back(nCntPoly * 3 + 2);
		rModelData.MeshData[node_name].Indices[pMatNameAnsi].push_back(nCntPoly * 3 + 1);
		rModelData.MeshData[node_name].Indices[pMatNameAnsi].push_back(nCntPoly * 3 + 0);
	}

	// メッシュの初期マトリックス取得
	FbxNode* pMeshNode = mesh->GetNode();
	FbxAMatrix FbxMtxmeshInit = pMeshNode->EvaluateLocalTransform();
	ConvartFbxMatrixToD3DXMAT(FbxMtxmeshInit, rModelData.MeshData[node_name].MtxMeshInit);
}

//=============================================================================
// ボーンの読み込み
//=============================================================================
void CFBXManager::LoadBone(ModelData& rModelData, const char* node_name, FbxMesh* mesh)
{
	// スキン取得
	FbxDeformer*pDeformer = mesh->GetDeformer(0, FbxDeformer::eSkin);
	FbxSkin* pSkin = static_cast<FbxSkin*>(pDeformer);

	//// ノード取得
	//FbxNode* pNode = mesh->GetNode();

	// スキンが無ければ次のループカウントへ
	if (pSkin == nullptr)
	{
		return;
	}

	// ボーン情報参照
	std::map<std::string, BoneData>& rBoneData = rModelData.MeshData[node_name].BoneData;

	// クラスター(ボーン)の数を取得
	int nNumCluster = pSkin->GetClusterCount();

	// クラスター(ボーン)の数分回す
	for (int nCntCluster = 0; nCntCluster < nNumCluster; nCntCluster++)
	{
		// クラスター(ボーン)の取得
		FbxCluster* pCluster = pSkin->GetCluster(nCntCluster);

		// クラスター(ボーン)が無ければ次のループカウントへ
		if(pCluster == nullptr)continue;
		
		// ノード取得
		FbxNode* pNode = pCluster->GetLink();

		// 名前取得
		string BoneName = pNode->GetName();

		// n番目のクラスター(ボーン)データ参照
		BoneData& rBone = rBoneData[BoneName];

		// 名前からキーワードを見つけたら
		if (BoneName.find(KEYWORD_BONE_FOLLOW) != string::npos)
		{
			// 追従タイプに変更
			rBone.Type = BONE_TYPE_FOLLOW;
		}

		// 名前の数分回す
		string StrWeight;
		bool bSetWeight = false;
		for (auto IterName : BoneName)
		{
			// キーワード文字列
			string cData;
			cData += IterName;

			// セット可能であれば
			if (bSetWeight == true)
			{
				// ウェイト保存
				StrWeight += cData;
			}

			// 揺れ具合設定キーワードを探す
			if (cData == KEYWORD_SHAKRATE)
			{
				// セット可能状態にする
				bSetWeight = true;
			}
		}

		// セット可能であれば
		if (bSetWeight == true)
		{
			// float型に変換
			float fShakeRate = stof(StrWeight);

			// 少数に変換
			fShakeRate *= CONVERT_FACTOR_SHAKRATE;

			// 重さ設定
			rBone.fShakeRate = fShakeRate;
		}

		// クラスター(ボーン)の初期姿勢取得
		FbxAMatrix MtxInit;
		pCluster->GetTransformLinkMatrix(MtxInit);
		ConvartFbxMatrixToD3DXMAT(MtxInit, rBone.MtxInit);
		rBone.MtxInit._41 = -rBone.MtxInit._41;

		// ノードがあれば
		if (pNode != nullptr)
		{
			// 親ノード取得
			FbxNode* pNodeParent = pNode->GetParent();

			// 親ノードがあれば
			if (pNodeParent != nullptr)
			{
				// 親ボーンの名前取得
				rBone.ParentName = pNodeParent->GetName();
			}

			// 子ノード取得
			FbxNode* pNodeChild = pNode->GetChild(0);

			// 子ノードがあれば
			if (pNodeChild != nullptr)
			{
				// 子ボーンの名前取得
				rBone.ChildName = pNodeChild->GetName();
			}
		}

		// クラスター(ボーン)内頂点数取得
		int nNumPoint = pCluster->GetControlPointIndicesCount();

		// 先頭頂コントロールポイント取得
		int *pAryPoint = pCluster->GetControlPointIndices();

		// 先頭重さコントロールポイント取得
		double* pAryWeight = pCluster->GetControlPointWeights();

		// 先頭コントロールポイントが無ければ次のループカウントへ
		if(pAryPoint == nullptr)continue;
		if(pAryWeight == nullptr)continue;

		// 頂点の数分確保
		rBone.IdxCP.assign(nNumPoint,0);
		rBone.Weight.assign(nNumPoint,0.0f);

		// クラスター(ボーン)内頂点数分回す
		for (int nCntPoint = 0; nCntPoint < nNumPoint; nCntPoint++)
		{
			// 頂点インデックス取得
			int nIndex = pAryPoint[nCntPoint];
			rBone.IdxCP[nCntPoint] = nIndex;

			// 重さ取得
			float fWeight = (float)pAryWeight[nCntPoint];
			rBone.Weight[nCntPoint] = fWeight;

			continue;
		}
	}
}

//=============================================================================
// モーション読み込み
//=============================================================================
void CFBXManager::LoadMotion(ModelData& rModelData, std::string& rFBXMotionFilePath)
{
	// ファイル名取得
	string FBXMotionFileName = GetModelName(rFBXMotionFilePath);

	// FbxManager作成
	FbxManager* fbx_manager = fbxsdk::FbxManager::Create();
	if (fbx_manager == nullptr)
	{
		return;
	}

	// FbxImporter作成
	FbxImporter* fbx_importer = FbxImporter::Create(fbx_manager, "");
	if (fbx_importer == nullptr)
	{
		fbx_manager->Destroy();
		return;
	}

	// FbxScene作成
	FbxScene* pFbxScene = FbxScene::Create(fbx_manager, "");
	if (pFbxScene == nullptr)
	{
		fbx_importer->Destroy();
		fbx_manager->Destroy();
		return;
	}

	// Fileを初期化
	if (fbx_importer->Initialize(rFBXMotionFilePath.c_str()) == false)
	{
		return;
	}

	// sceneにインポート
	fbx_importer->Import(pFbxScene);

	// ルートノード取得
	FbxNode* pRootNode = pFbxScene->GetRootNode();

	// 取得できなければ処理しない
	if (pRootNode == nullptr)return;

	// ボーンのノードを探す;
	std::map<std::string, FbxNode*> BoneList;
	RecursionSkeltonNode(pRootNode, BoneList);

	// 新しいモーションデータ
	MotionData MotionDataNew;

	// クラスター(ボーン)の数分回す
	for (auto IterBoneNode : BoneList)
	{
		// 新しいモーションのボーンデータ
		MotionBoneData MotionBoneDataNew;

		// アニメーションスタック取得
		FbxAnimStack* pAnimStack = pFbxScene->GetCurrentAnimationStack();

		// テイク情報取得
		FbxTakeInfo* pTakeInfo = pFbxScene->GetTakeInfo(pAnimStack->GetName());

		// スタート時間取得
		FbxTime StartFbx = pTakeInfo->mLocalTimeSpan.GetStart();
		StartFbx = StartFbx.GetFrameCount(FbxTime::eFrames60);

		// 終了時間取得
		FbxTime EndFbx = pTakeInfo->mLocalTimeSpan.GetStop();
		EndFbx = EndFbx.GetFrameCount(FbxTime::eFrames60);

		// 型変換
		int StartInt;
		int EndInt;
		ConvartFbxTimeToInt(StartFbx, StartInt);
		ConvartFbxTimeToInt(EndFbx, EndInt);

		// フレーム数
		int nNumFrame = EndInt - StartInt;

		// フレーム数分回す
		for (int nCntFrame = 0; nCntFrame < nNumFrame; nCntFrame++)
		{
			// フレーム数時のボーンマトリックス取得
			FbxAMatrix MtxFbx;
			FbxTime Time;
			Time.SetFrame(nCntFrame, FbxTime::eFrames60);
			MtxFbx = IterBoneNode.second->EvaluateGlobalTransform(Time);

			// 型を変換
			D3DXMATRIX MtxD3DX;
			ConvartFbxMatrixToD3DXMAT(MtxFbx, MtxD3DX);

			// X軸要素反転
			MtxD3DX._12 = -MtxD3DX._12;
			MtxD3DX._13 = -MtxD3DX._13;
			MtxD3DX._21 = -MtxD3DX._21;
			MtxD3DX._31 = -MtxD3DX._31;
			MtxD3DX._41 = -MtxD3DX._41;

			// マトリックス設定
			MotionBoneDataNew.FlameMtxList.push_back(MtxD3DX);
		}

		// 新しいモーションボーンデータ設定
		MotionDataNew.MotionBoneList[IterBoneNode.first] = MotionBoneDataNew;

		// フレーム数設定
		MotionDataNew.nFrame = nNumFrame;
	}

	// 名前設定
	MotionDataNew.Name = FBXMotionFileName;
	
	// モーション情報追加
	rModelData.MotionList.push_back(MotionDataNew);

	fbx_importer->Destroy();
	pFbxScene->Destroy();
	fbx_manager->Destroy();
}
//=============================================================================
// 破棄処理
//=============================================================================
void CFBXManager::Uninit(void)
{
	// インポーターの破棄
	if (m_pFbxImporter != nullptr)
	{
		m_pFbxImporter->Destroy();
		m_pFbxImporter = nullptr;
	}

	// マネージャーの破棄
	if (m_pFbxManager != nullptr)
	{
		m_pFbxManager->Destroy();
		m_pFbxManager = nullptr;
	}

	// モデルの削除
	for (auto& IterModel : m_ModelData)
	{
		IterModel.second.Uninit();
	}
	m_ModelData.clear();
}

//=============================================================================
// インスタンス取得処理
//=============================================================================
CFBXManager& CFBXManager::GetInstance(void)
{
	// インスタンス生成
	static CFBXManager ManagerFBX;

	return ManagerFBX;
}

//=============================================================================
// モデル取得処理
//=============================================================================
CFBXManager::ModelData* CFBXManager::GetModelData(std::string ModelName)
{
	// 探す
	auto IterModelData = m_ModelData.find(ModelName);
	
	// 取得できなかったら仮モデルを渡す
	if(IterModelData == m_ModelData.end())return &m_ModelData["test2.fbx"];

	// 返す
	return &IterModelData->second;
}
//=============================================================================
// モデルデータ破棄処理
//=============================================================================
void CFBXManager::ModelData::Uninit(void)
{
	// メッシュのデータ分回す
	for (auto IterMesh : MeshData)
	{
		// インデックスバッファ破棄
		for (auto& Iter : IterMesh.second.IndexBuffers)
		{
			// 無ければ次へ回す
			if (Iter.second == nullptr)continue;

			Iter.second->Release();
		}

		// 頂点バッファ破棄
		if (IterMesh.second.VtxBuff != nullptr)
		{
			IterMesh.second.VtxBuff->Release();
		}

		// インデックス破棄
		IterMesh.second.Indices.clear();

		// 頂点破棄
		IterMesh.second.Vertices.clear();
	}
}

//=============================================================================
// FbxAMatrixからD3DXMATRIXに変換する処理
//=============================================================================
void CFBXManager::ConvartFbxMatrixToD3DXMAT(FbxAMatrix& rFbxMatrix, D3DXMATRIX& rD3DXMat)
{
	// 行分回す
	for (int nCntRow = 0; nCntRow < 4; nCntRow++)
	{
		// 列分回す
		for (int nCntCol = 0; nCntCol < 4; nCntCol++)
		{
			rD3DXMat(nCntRow,nCntCol) = static_cast<float>(rFbxMatrix.Get(nCntRow,nCntCol));
		}
	}
}

//=============================================================================
// FbxTimeからintに変換する処理
//=============================================================================
void CFBXManager::ConvartFbxTimeToInt(FbxTime& rFbxTime,int& rInt)
{
	rInt = static_cast<int>(rFbxTime.Get());
}

//=============================================================================
// コントロールポイントの番号からボーンインデックスと重さを調べる関数
//=============================================================================
void CFBXManager::GetBoneIdxAndWeightFromIdxCP(std::vector<int>& rOutIdxBone, std::vector<float>& rOutWeight, MeshData& rMeshData, int nIdxCP)
{
	// 出力配列をクリーン
	rOutIdxBone.clear();
	rOutWeight.clear();

	// ボーンカウンター
	int nCntBone = 0;
	for (auto IterBone : rMeshData.BoneData)
	{
		// ボーンデータ参照
		BoneData& rBoneData = IterBone.second;

		// ボーン内の頂点インデックス分又回す
		for (int nCntIdxVtx = 0; nCntIdxVtx < (int)rBoneData.IdxCP.size(); nCntIdxVtx++)
		{
			// 探しているCPが無かったら次を検査
			if (rBoneData.IdxCP[nCntIdxVtx] != nIdxCP)continue;

			// 出力配列に追加
			rOutIdxBone.push_back(nCntBone);
			rOutWeight.push_back(rBoneData.Weight[nCntIdxVtx]);
		}

		// カウンター追加
		nCntBone++;
	}

	// 要素量調整
	rOutIdxBone.resize(MAX_BONE_IDX_NUM,0);
	rOutWeight.resize(MAX_BONE_IDX_NUM,0.0f);
}

//=============================================================================
// ボーンノードを探す再帰関数
//=============================================================================
void CFBXManager::RecursionSkeltonNode(FbxNode* pRootNode, std::map<std::string, FbxNode*>& rBoneList)
{
	// 取得できなければ処理しない
	if (pRootNode == nullptr)
	{
		return;
	}

	// ノードアトリビュート取得
	FbxNodeAttribute* pNodeAtt = pRootNode->GetNodeAttribute();

	// ノードアトリビュートがスケルトンタイプだったら
	if (
	pNodeAtt != nullptr && 
	pNodeAtt->GetAttributeType() == FbxNodeAttribute::eSkeleton)
	{
		rBoneList[pRootNode->GetName()] = pRootNode;
	}

	// ノードの子の数分回す
	for (int nCntNodeChild = 0; nCntNodeChild < pRootNode->GetChildCount(); nCntNodeChild++) 
	{
		// 子を探す
		RecursionSkeltonNode(pRootNode->GetChild(nCntNodeChild),rBoneList);
	}
}