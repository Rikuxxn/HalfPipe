//=============================================================================
// 
// FBXモデルコンポーネント [FBXModelComponent.cpp]
// Author : RIKU TANEKAWA
// 
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "FBXModelComponent.h"
#include "Manager.h"
#include "Renderer.h"
#include "TextureResource.h"
#include "TransformComponent.h"
#include "ShaderManager.h"
#include "ShaderResource.h"
#include "TextureManager.h"
#include "MathUtility.h"
#include "MatrixUtility.h"
#include "InputManager.h"
#include "Input.h"
#include "GameObject.h"
#include "Manager.h"
#include "Camera.h"
#include "EnvironmentUtility.h"
#include "GameTime.h"

// 名前空間の使用
using namespace MathUtil;
using namespace MatrixUtil;
using namespace EnvironmentUtil;


//=============================================================================
// コンストラクタ
//=============================================================================
CFBXModelComponent::CFBXModelComponent(CFBXManager::ArgumentLoad& rArg)
{
	// 読み込み
	m_ModelName = CFBXManager::GetInstance().Load(rArg);

	// シェーダーマネージャーの取得
	auto shaderMgr = CManager::Instance()->GetShaderManager();

	// シェーダーの追加
	m_pShaderFBX = shaderMgr->Load("data/Shader/Fbx/ShaderFBX.hlsl");

	// ライティングの有無設定
	m_bLighting = rArg.bLighting;
}

//=============================================================================
// デストラクタ
//=============================================================================
CFBXModelComponent::~CFBXModelComponent()
{

}

//=============================================================================
// 初期化
//=============================================================================
HRESULT CFBXModelComponent::Init(void)
{
	// モデルデータ取得
	CFBXManager::ModelData* pModelData = CFBXManager::GetInstance().GetModelData(m_ModelName);

	// 取得できなければ処理しない
	if (pModelData == nullptr)
	{
		return E_FAIL;
	}

	// メッシュの数分回す
	for (auto IterMeshData : pModelData->MeshData)
	{
		// ボーンの数分回す
		for (auto IterBoneData : IterMeshData.second.BoneData)
		{
			// ボーンの初期マトリックス取得
			D3DXMATRIX MtxInit = IterMeshData.second.BoneData[IterBoneData.first].MtxInit;

			// ボーンのマトリックス確保
			m_MtxBoneDataOld[IterBoneData.first].first = MtxInit;

			// 子ボーンの名前設定
			m_MtxBoneDataOld[IterBoneData.first].second = IterBoneData.second.ChildName;
		}
	}

	return S_OK;
}

//=============================================================================
// 終了
//=============================================================================
void CFBXModelComponent::Uninit(void)
{
	// 自身の破棄
	delete this;
}

//=============================================================================
// 更新
//=============================================================================
void CFBXModelComponent::Update(void)
{
#ifdef _DEBUG
	if(CManager::Instance()->GetInput()->GetKeyboard()->GetTrigger(DIK_1) == true)
	{
		SetMotion(MOTION_TYPE_00, 30);
	}

	if (CManager::Instance()->GetInput()->GetKeyboard()->GetTrigger(DIK_2) == true)
	{
		SetMotion(MOTION_TYPE_01, 30);
	}
#endif

	// モーションの更新
	UpdateMotion();

	// ブレンド用モーションの更新
	UpdateMotionBlend();
}

//=============================================================================
// 描画
//=============================================================================
void CFBXModelComponent::Draw(void)
{
	// モーションがあれば
	if (m_MotionCntrolData.GetTypeMotion() > MOTION_TYPE_NONE)
	{
		// ボーン有描画
		DrawBoneOwn();
	}
	// モーションが無ければ
	else
	{
		// ボーン無し描画
		DrawBoneNone();
	}

}

//=============================================================================
// 描画モーション無し
//=============================================================================
void CFBXModelComponent::DrawBoneNone(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::Instance()->GetRenderer()->GetDevice();

	// モデルデータ取得
	CFBXManager::ModelData* pModelData = CFBXManager::GetInstance().GetModelData(m_ModelName);

	// 取得できなければ処理しない
	if (pModelData == nullptr)
	{
		return;
	}

	// コンポーネント取得
	auto transform = m_pOwner->GetComponent<CTransformComponent>();

	// シェーダー取得
	LPD3DXEFFECT pShader = m_pShaderFBX->GetEffect();

	// 取得できなければ処理しない
	if (pShader == nullptr)
	{
		return;
	}

	// ワールドマトリックス用意
	D3DXMATRIX MtxWorld = INIT_MTX;

	// 取得できていれば
	if (transform != nullptr)
	{
		// トランスフォームのマトリックス取得
		MtxWorld = transform->GetWorldMatrix();
	}

	D3DXMATRIX MtxView = CManager::Instance()->GetCamera()->GetViewMatrix();
	D3DXMATRIX MtxProj = CManager::Instance()->GetCamera()->GetProjMatrix();

	pShader->SetMatrix("MtxWorld", &MtxWorld);
	pShader->SetMatrix("MtxView", &MtxView);
	pShader->SetMatrix("MtxProj", &MtxProj);

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D_FBX);

	// シェーダー起動
	pShader->Begin(0, 0);

	// メッシュの数分回す
	for (auto& IterMeshData : pModelData->MeshData)
	{
		// 頂点バッファをデバイスのデータストリームに設定
		pDevice->SetStreamSource(0, IterMeshData.second.VtxBuff, 0, sizeof(VERTEX_3D_FBX));

		// メッシュの幾何オフセットを取得
		D3DXMATRIX MtxMeshInit = IterMeshData.second.MtxMeshInit;

		// 定数レジスタにメッシュの初期位置マトリックスを設定
		pShader->SetMatrix("MtxMeshInit", &MtxMeshInit);

		// インデックスの数分回す
		for (auto& IterIdxBuff : IterMeshData.second.IndexBuffers)
		{
			// マテリアル情報設定
			CFBXManager::MaterialLambertData& rMaterial = pModelData->Materials[IterIdxBuff.first];
			pShader->SetVector("MaterialAmbient", &rMaterial.Ambient);
			pShader->SetVector("MaterialDiffuse", &rMaterial.Diffuse);
			pShader->SetVector("MaterialEmissive", &rMaterial.Emissive);
			pShader->SetVector("MaterialTransParentColor", &rMaterial.TransParentColor);

			if (rMaterial.Texture != nullptr)
			{
				pShader->SetTexture("Texture0", rMaterial.Texture->Texture.Get());
			}
			else
			{
				pShader->SetTexture("Texture0", nullptr);
			}

			// インデックスバッファをデータストリームに設定
			pDevice->SetIndices(IterIdxBuff.second);

			// ライティングあり
			if (m_bLighting == true)
			{
				// パスの起動
				pShader->BeginPass(1);
			}
			else
			{
				// パスの起動
				pShader->BeginPass(3);
			}

			// 変更をGPUに反映
			pShader->CommitChanges();

			// 描画
			pDevice->DrawIndexedPrimitive(
				D3DPT_TRIANGLELIST,
				0,
				0,
				(UINT)IterMeshData.second.Vertices.size(),
				0,
				(UINT)IterMeshData.second.Indices[IterIdxBuff.first].size() / 3
			);

			// パスの終了
			pShader->EndPass();
		}
	}

	// シェーダー終了
	pShader->End();

	// ライティング設定
	pDevice->LightEnable(0, TRUE);
}

//=============================================================================
// 描画モーションあり
//=============================================================================
void CFBXModelComponent::DrawBoneOwn(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::Instance()->GetRenderer()->GetDevice();

	// モデルデータ取得
	CFBXManager::ModelData* pModelData = CFBXManager::GetInstance().GetModelData(m_ModelName);

	// 取得できなければ処理しない
	if (pModelData == nullptr)
	{
		return;
	}

	// コンポーネント取得
	auto transform = m_pOwner->GetComponent<CTransformComponent>();

	// シェーダー取得
	LPD3DXEFFECT pShader = m_pShaderFBX->GetEffect();

	// 取得できなければ処理しない
	if (pShader == nullptr)
	{
		return;
	}

	// ライティング設定
	pDevice->LightEnable(0,(BOOL)m_bLighting);

	// ワールドマトリックス用意
	D3DXMATRIX MtxWorld = INIT_MTX;

	// 取得できていれば
	if (transform != nullptr)
	{
		// トランスフォームのマトリックス取得
		MtxWorld = transform->GetWorldMatrix();
	}

	// 送るボーンデータ変数
	D3DXMATRIX MtxBoneData[MAX_BONE_NUM];
	D3DXMATRIX MtxBoneInitData[MAX_BONE_NUM];

	for (int i = 0; i < MAX_BONE_NUM; i++)
	{
		MtxBoneData[i] = Identity();
		MtxBoneInitData[i] = Identity();
	}

	//// スキン番号
	//int nNumberSkin = 0;

	//// 現在のモーション番号
	//int nTypeMotion = m_MotionCntrolData.GetTypeMotion();

	//// 現在のモーションのフレーム数
	//int nFrameNow = m_MotionCntrolData.GetFrameNow();

	// テクスチャを設定
	pDevice->SetTexture(0, nullptr);

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D_FBX);

	D3DXMATRIX MtxView = CManager::Instance()->GetCamera()->GetViewMatrix();
	D3DXMATRIX MtxProj = CManager::Instance()->GetCamera()->GetProjMatrix();

	pShader->SetMatrix("MtxWorld", &MtxWorld);
	pShader->SetMatrix("MtxView", &MtxView);
	pShader->SetMatrix("MtxProj", &MtxProj);

	//=========================================================================
	// 時間(ゲーム時間)
	//=========================================================================
	float time = CManager::Instance()->GetGameTime()->GetTimeOfDay();

	//=========================================================================
	// 太陽 ・ 月
	//=========================================================================
	D3DXVECTOR3 sunDir = GetSunDirection(time);
	D3DXVECTOR3 moonDir = GetMoonDirection(time);

	float sunHeight = sunDir.y;
	float moonHeight = moonDir.y;

	//=========================================================================
	// 現在の光源
	//=========================================================================
	D3DXVECTOR3 lightDir;
	D3DXVECTOR3 lightColor;
	float lightIntensity;

	//-------------------------------------------------------------------------
	// 太陽が地平線より上
	//-------------------------------------------------------------------------
	if (sunHeight > 0.05f)
	{
		// 太陽から来る光の方向
		lightDir = sunDir;

		// 太陽光
		lightColor = D3DXVECTOR3(1.0f, 0.95f, 0.85f);
		lightIntensity = 1.0f;
	}
	//-------------------------------------------------------------------------
	// 月が地平線より上
	//-------------------------------------------------------------------------
	else if (moonHeight > 0.05f)
	{
		// 月から来る光の方向
		lightDir = moonDir;

		// 月光
		lightColor = D3DXVECTOR3(0.7f, 0.8f, 1.0f);
		lightIntensity = 0.8f;
	}
	//-------------------------------------------------------------------------
	// 太陽も月も沈んでいる
	//-------------------------------------------------------------------------
	else
	{
		// 夜
		lightDir = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
		lightColor = D3DXVECTOR3(0.1f, 0.1f, 0.15f);
		lightIntensity = 0.2f;
	}

	D3DXVECTOR4 LightVector(lightDir.x, lightDir.y, lightDir.z, 0.0);

	D3DXVECTOR4 LightColor(
		lightColor.x * lightIntensity,
		lightColor.y * lightIntensity,
		lightColor.z * lightIntensity,
		1.0f
	);

	pShader->SetVector("LightVector", &LightVector);
	pShader->SetVector("LightColor", &LightColor);

	pShader->Begin(0, 0);

	for (auto& IterMeshData : pModelData->MeshData)
	{
		// ボーンの最大数又はボーンの要素数分回す
		int nCntBone = 0;

		for (auto& IterBoneData : IterMeshData.second.BoneData)
		{
			if (nCntBone >= MAX_BONE_NUM)
			{
				break;
			}

			// ボーン名取得
			std::string BoneName = IterBoneData.first;

			// 初期ボーン行列
			MtxBoneInitData[nCntBone] = IterBoneData.second.MtxInit;

			// まず必ず初期行列
			MtxBoneData[nCntBone] = IterBoneData.second.MtxInit;

			// 通常ボーン
			if (IterBoneData.second.Type == CFBXManager::BONE_TYPE_NOMAL)
			{
				MtxBoneData[nCntBone] = CreateBlendMotionMatrix(pModelData->MotionList, BoneName);
			}
			// 追従ボーン
			else if (IterBoneData.second.Type == CFBXManager::BONE_TYPE_FOLLOW)
			{
				// 親の名前を取得
				std::string BoneNameParent = IterBoneData.second.ParentName;

				// 親を探す
				auto IterBoneParent = IterMeshData.second.BoneData.find(BoneNameParent);

				// 親が見つかれば
				if (IterBoneParent != IterMeshData.second.BoneData.end())
				{
					D3DXMATRIX MtxNowOld = m_MtxBoneDataOld[BoneName].first;
					D3DXMATRIX MtxParent = m_MtxBoneDataOld[BoneNameParent].first;
					D3DXMATRIX MtxParentInit = IterMeshData.second.BoneData[BoneNameParent].MtxInit;

					MtxBoneData[nCntBone] = MatrixFollowShaking(
							MtxNowOld,
							MtxParent,
							MtxBoneInitData[nCntBone],
							MtxParentInit,
							IterBoneData.second.fShakeRate
						);
				}
			}

			// 前フレーム保存
			m_MtxBoneDataOld[BoneName].first = MtxBoneData[nCntBone];

			// カウンター追加
			nCntBone++;
		}

		// メッシュの初期行列を取得
		D3DXMATRIX MtxMeshInit = IterMeshData.second.MtxMeshInit;

		// 定数レジスタにメッシュの初期位置マトリックスを設定
		pShader->SetMatrix("MtxMeshInit", &MtxMeshInit);

		// 定数レジスタにボーンマトリックスを設定
		pShader->SetMatrixArray("MtxBones", MtxBoneData, MAX_BONE_NUM);

		// 定数レジスタにボーン初期マトリックスを設定
		pShader->SetMatrixArray("MtxBonesInit", MtxBoneInitData, MAX_BONE_NUM);

		// 頂点バッファをデバイスのデータストリームに設定
		pDevice->SetStreamSource(0, IterMeshData.second.VtxBuff, 0, sizeof(VERTEX_3D_FBX));

		// インデックスの数分回す
		for (auto& IterIdxBuff : IterMeshData.second.IndexBuffers)
		{
			// マテリアル情報設定
			CFBXManager::MaterialLambertData& rMaterial = pModelData->Materials[IterIdxBuff.first];
			pShader->SetVector("MaterialAmbient", &rMaterial.Ambient);
			pShader->SetVector("MaterialDiffuse", &rMaterial.Diffuse);
			pShader->SetVector("MaterialEmissive", &rMaterial.Emissive);
			pShader->SetVector("MaterialTransParentColor", &rMaterial.TransParentColor);

			BOOL bHasTexture = (rMaterial.Texture != nullptr);

			pShader->SetBool("HasTexture", bHasTexture);

			if (bHasTexture)
			{
				pShader->SetTexture("Texture0", rMaterial.Texture->Texture.Get());
			}
			else
			{
				pShader->SetTexture("Texture0", nullptr);
			}

			pDevice->SetIndices(IterIdxBuff.second);

			// ライティングあり
			if (m_bLighting == true)
			{
				// パスの起動
				pShader->BeginPass(0);
			}
			else
			{
				// パスの起動
				pShader->BeginPass(2);
			}

			pShader->CommitChanges();

			pDevice->DrawIndexedPrimitive(
				D3DPT_TRIANGLELIST,
				0,
				0,
				(UINT)IterMeshData.second.Vertices.size(),
				0,
				(UINT)IterMeshData.second.Indices[IterIdxBuff.first].size() / 3
			);

			pShader->EndPass();
		}
	}

	pShader->End();

	// ライティング設定
	pDevice->LightEnable(0, TRUE);
}

//=============================================================================
// モーション設定
//=============================================================================
void CFBXModelComponent::SetMotion(int nTypeMotion,int nTimeBlend)
{
	// モーションタイプが範囲外であればこの先処理しない
	if(nTypeMotion <= MOTION_TYPE_NONE)
	{
		// モーションタイプ設定
		m_MotionCntrolData.SetTypeMotion(nTypeMotion);
		return;
	}

	if(nTypeMotion >= MOTION_TYPE_MAX)
	{
		// モーションタイプ設定
		m_MotionCntrolData.SetTypeMotion(nTypeMotion);
		return;
	}

	// モデルデータ取得
	CFBXManager::ModelData* pModelData = CFBXManager::GetInstance().GetModelData(m_ModelName);

	// 取得できなければ処理しない
	if (pModelData == nullptr)
	{
		return;
	}

	// 要素が無ければ処理しない
	if(pModelData->MotionList.empty() == true)					 return;
	if(pModelData->MotionList.size() <= nTypeMotion)			 return;
	if(pModelData->MotionList[0].MotionBoneList.empty() == true) return;
	
	// モーションデータ参照
	CFBXManager::MotionData& rMotionData = pModelData->MotionList[nTypeMotion];

	// モーションフレーム数取得
	int nFrameMax = (int)rMotionData.nFrame;

	// 最大フレーム数を1つ下げる
	nFrameMax--;
	if (nFrameMax <= 1)
	{
		nFrameMax = 1;
	}

	// ループの有無取得
	bool bLoop = rMotionData.bLoop;

	// ブレンドするデータに現在のデータをコピーする
	m_MotionCntrolDataBlend = m_MotionCntrolData;

	// モーションタイプ設定
	m_MotionCntrolData.SetTypeMotion(nTypeMotion);

	// 現在のフレーム数リセット
	m_MotionCntrolData.SetFrameNow(0);

	// 最大フレーム数設定
	m_MotionCntrolData.SetFrameMax(nFrameMax);

	// モーションが終ってない設定にする
	m_MotionCntrolData.SetIsEndMotion(false);

	// ループの有無設定
	m_MotionCntrolData.SetIsLoop(bLoop);

	// ブレンドカウンターをゼロ
	m_MotionCntrolData.SetCntBlendTime(0);

	// ブレンドが終っていない状態にする
	m_MotionCntrolData.SetIsEndBlend(false);

	// ブレンドする時間設定
	m_MotionCntrolData.SetBlendTime(nTimeBlend);

	// ブレンドカウンタ0
	m_MotionCntrolData.SetCntBlendTime(0);
}

//=============================================================================
// モーション更新
//=============================================================================
void CFBXModelComponent::UpdateMotion(void)
{
	// モーションが終っていれば処理しない
	if (m_MotionCntrolData.GetIsEndMotion() == true)
	{
		return;
	}

	// 現在フレーム数追加
	m_MotionCntrolData.AddFrameNow();

	// ブレンドカウンター加算
	m_MotionCntrolData.AddCntBlendTime();

	// 最大であればブレンドが終ったことにする
	if (m_MotionCntrolData.IsCntBlendTimeMax() == true)
	{
		m_MotionCntrolData.SetIsEndBlend(true);
	}

	// 現在のフレーム数が最大に達したら
	if (m_MotionCntrolData.IsFrameNowReachedMax() == true)
	{
		// モーションが終ったと設定する
		m_MotionCntrolData.SetIsEndMotion(true);

		// ループするのであれば
		if (m_MotionCntrolData.GetIsLoop() == true)
		{
			// モーションが終っていないことにする
			m_MotionCntrolData.SetIsEndMotion(false);

			// 現在のフレーム数ゼロ
			m_MotionCntrolData.SetFrameNow(0);
		}
	}
}

//=============================================================================
// ブレンドの更新
//=============================================================================
void CFBXModelComponent::UpdateMotionBlend(void)
{
	// モーションが終っていれば処理しない
	if (m_MotionCntrolDataBlend.GetIsEndMotion() == true)
	{
		return;
	}

	// 現在フレーム数追加
	m_MotionCntrolDataBlend.AddFrameNow();

	// 現在のフレーム数が最大に達したら
	if (m_MotionCntrolDataBlend.IsFrameNowReachedMax() == true)
	{
		// モーションが終ったと設定する
		m_MotionCntrolDataBlend.SetIsEndMotion(true);

		// ループするのであれば
		if (m_MotionCntrolDataBlend.GetIsLoop() == true)
		{
			// モーションが終っていないことにする
			m_MotionCntrolDataBlend.SetIsEndMotion(false);

			// 現在のフレーム数ゼロ
			m_MotionCntrolDataBlend.SetFrameNow(0);
		}
	}
}

//=============================================================================
// ブレンド時間の率(レート)取得処理
//=============================================================================
float CFBXModelComponent::MotionControlData::GetRateBlendTime(void)
{
	// 割る数がゼロの場合処理をしない
	if (nBlendTime == 0)
	{
		return 0.0f;
	}

	// 計算
	return (float)nCntBlendTime / (float)nBlendTime;
}

//=============================================================================
// 現在フレーム加算処理
//=============================================================================
void CFBXModelComponent::MotionControlData::AddFrameNow(void)
{
	// フレーム数追加
	nFrameNow++;

	// 値が範囲外にならないようにする
	nFrameNow = Clamp(nFrameNow, 0, nFrameMax);
}

//=============================================================================
// ブレンド時間加算処理
//=============================================================================
void CFBXModelComponent::MotionControlData::AddCntBlendTime(void)
{
	// カウンター数加算
	nCntBlendTime++;

	// 値が範囲外にならないようにする
	nCntBlendTime = Clamp(nCntBlendTime, 0, nBlendTime);
}

//=============================================================================
// ブレンド時間減算処理
//=============================================================================
void CFBXModelComponent::MotionControlData::SubCntBlendTime(void)
{
	// カウンター数減算
	nCntBlendTime--;

	// 値が範囲外にならないようにする
	nCntBlendTime = Clamp(nCntBlendTime, 0, nCntBlendTime);
}

//=============================================================================
// 現在のフレーム数が最大に達したかの真偽
//=============================================================================
bool CFBXModelComponent::MotionControlData::IsFrameNowReachedMax(void)
{ 
	return nFrameNow >= nFrameMax;
}

//=============================================================================
// 現在のブレンド時間が最大に達したかの真偽
//=============================================================================
bool CFBXModelComponent::MotionControlData::IsCntBlendTimeMax(void)
{
	return nCntBlendTime >= nBlendTime;
}

//=============================================================================
// ボーンのブレンド処理
//=============================================================================
D3DXMATRIX CFBXModelComponent::CreateBlendMotionMatrix(std::vector <CFBXManager::MotionData>& rMotionList, std::string& rBoneName)
{
	// 現在のモーションタイプ取得
	int nTypeMotion = m_MotionCntrolData.GetTypeMotion();

	// ブレンドするモーション取得
	int nTypeMotionBlend = m_MotionCntrolDataBlend.GetTypeMotion();

	// 現在のモーションのフレーム数
	int nFrameNow = m_MotionCntrolData.GetFrameNow();

	// 現在のマトリックス取得
	D3DXMATRIX MtxNow = rMotionList[nTypeMotion].MotionBoneList[rBoneName].FlameMtxList[nFrameNow];

	// ブレンドが終っていれば現在の行列を返す
	if (m_MotionCntrolData.GetIsEndBlend() == true)
	{
		return MtxNow;
	}

	// モーションが無ければ現在の行列を返す
	if (nTypeMotionBlend == MOTION_TYPE_NONE)
	{
		return MtxNow;
	}

	// 現在のブレンドモーションのフレーム数
	int nFrameNowBlend = m_MotionCntrolDataBlend.GetFrameNow();

	// ブレンドするマトリックスを取得
	D3DXMATRIX MtxBlend = rMotionList[nTypeMotionBlend].MotionBoneList[rBoneName].FlameMtxList[nFrameNowBlend];

	// ブレンド時間のレート取得
	float fRateBlendTime = m_MotionCntrolData.GetRateBlendTime();
	float fRateBlendTimeRevers = 1.0f - fRateBlendTime;

	// 各マトリックスの回転行列を作成
	D3DXMATRIX MtxNowRot = GetRotationMatrix(MtxNow);
	D3DXMATRIX MtxBlendRot = GetRotationMatrix(MtxBlend);
	D3DXVECTOR3 PosNow = GetPosition(MtxNow);
	D3DXVECTOR3 PosBlend = GetPosition(MtxBlend);

	// 回転行列をクォータニオンに変換
	D3DXQUATERNION QuatNow = MatrixToQuaternion(MtxNowRot);
	D3DXQUATERNION QuatBlend = MatrixToQuaternion(MtxBlendRot);

	// 希望の回転を算出
	D3DXQUATERNION QuatHope/* = QuatSlerp(QuatNow, QuatBlend, fRateBlendTimeRevers)*/;
	D3DXQuaternionSlerp(&QuatHope, &QuatNow, &QuatBlend, fRateBlendTimeRevers);

	// 希望の位置を算出
	D3DXVECTOR3 PosHope = PosNow * fRateBlendTime;
	PosHope += PosBlend * fRateBlendTimeRevers;

	// 希望の行列を作成
	D3DXMATRIX MtxHope = QuatWorld(PosHope, QuatHope);

	// 行列を返す
	return MtxHope;
}

//=============================================================================
// ボーンの行列を取得する処理
//=============================================================================
D3DXMATRIX CFBXModelComponent::GetMtxBone(std::string BoneName)
{
	// 探す
	auto IterMtxBone = m_MtxBoneDataOld.find(BoneName);

	// 無ければ初期行列を返す
	if (IterMtxBone == m_MtxBoneDataOld.end())
	{
		return INIT_MTX;
	}

	// 探したボーンの行列を返す
	return IterMtxBone->second.first;
}

//=============================================================================
// ボーンのワールドマトリックスを取得する処理
//=============================================================================
D3DXMATRIX CFBXModelComponent::GetMtxWorldBone(std::string BoneName)
{
	// 探す
	auto IterMtxBone = m_MtxBoneDataOld.find(BoneName);

	// 無ければ初期行列を返す
	if (IterMtxBone == m_MtxBoneDataOld.end())
	{
		return INIT_MTX;
	}

	// 探したボーンの行列を返す
	return IterMtxBone->second.first;
}

//=============================================================================
// 子ボーンの行列を取得する処理
//=============================================================================
D3DXMATRIX CFBXModelComponent::GetMtxChildBone(std::string ParentBoneName)
{
	// 親を探す
	auto IterParentBoneData = m_MtxBoneDataOld.find(ParentBoneName);

	// 無ければ初期行列を返す
	if (IterParentBoneData == m_MtxBoneDataOld.end())
	{
		return INIT_MTX;
	}

	// 子を探す
	auto IterChildBoneData = m_MtxBoneDataOld.find(IterParentBoneData->second.second);

	// 無ければ親行列を返す
	if (IterChildBoneData == m_MtxBoneDataOld.end())
	{
		return IterParentBoneData->second.first;
	}

	// 探した子ボーンの行列を返す
	return IterChildBoneData->second.first;
}

//=============================================================================
// ボーン情報があるか確認する処理
//=============================================================================
bool CFBXModelComponent::IsCheckBoneData(std::string BoneName)
{
	// 探す
	auto IterMtxBone = m_MtxBoneDataOld.find(BoneName);

	// 無かった
	if (IterMtxBone == m_MtxBoneDataOld.end())
	{
		return false;
	}

	// あった
	return true;
}