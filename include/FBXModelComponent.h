//=============================================================================
// 
// FBXモデルコンポーネント [FBXModelComponent.h]
// Author : RIKU TANEKAWA
// 
//=============================================================================
#ifndef _FBX_MODEL_COMPONENT_H_
#define _FBX_MODEL_COMPONENT_H_

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "RenderComponent.h"
#include "FBXManager.h"

#define MOTIONBLEND_TIME (10)	// モーションブレンドする時間

// 前方宣言
class CShaderResource;

//*****************************************************************************
// FBXモデルコンポーネントクラス
//*****************************************************************************
class CFBXModelComponent : public CRenderComponent
{
public:
	// モーションタイプ列挙
	enum
	{
		MOTION_TYPE_NONE = -1,	// モーション無し
		MOTION_TYPE_00,			// モーションタイプ00
		MOTION_TYPE_01,			// モーションタイプ01
		MOTION_TYPE_02,			// モーションタイプ02
		MOTION_TYPE_03,			// モーションタイプ03
		MOTION_TYPE_04,			// モーションタイプ04
		MOTION_TYPE_05,			// モーションタイプ05
		MOTION_TYPE_06,			// モーションタイプ06
		MOTION_TYPE_07,			// モーションタイプ07
		MOTION_TYPE_08,			// モーションタイプ08
		MOTION_TYPE_09,			// モーションタイプ09
		MOTION_TYPE_11,			// モーションタイプ11
		MOTION_TYPE_12,			// モーションタイプ12
		MOTION_TYPE_13,			// モーションタイプ13
		MOTION_TYPE_14,			// モーションタイプ14
		MOTION_TYPE_15,			// モーションタイプ15
		MOTION_TYPE_16,			// モーションタイプ16
		MOTION_TYPE_17,			// モーションタイプ17
		MOTION_TYPE_18,			// モーションタイプ18
		MOTION_TYPE_19,			// モーションタイプ19
		MOTION_TYPE_MAX
	}MOTION_TYPE;

	// モーション制御情報
	typedef struct MotionControlData
	{
	public:

		//=====================================================================
		// setter
		//=====================================================================

		// モーションタイプ設定処理
		void SetTypeMotion(int nSetTypeMotion) { nTypeMotion = nSetTypeMotion; }

		// 現在フレーム設定処理
		void SetFrameNow(int nSetFrameNow) { nFrameNow = nSetFrameNow; }

		// 最大フレーム設定処理
		void SetFrameMax(int nSetFrameMax) { nFrameMax = nSetFrameMax; }

		// ブレンド時間カウンター設定処理
		void SetCntBlendTime(int nSetCntBlendTime) { nCntBlendTime = nSetCntBlendTime; }

		// ブレンド時間設定処理
		void SetBlendTime(int nSetBlendTime) { nBlendTime = nSetBlendTime; }

		// ループするかどうか設定
		void SetIsLoop(bool bSetLoop) { bLoop = bSetLoop; }

		// モーションが終了したかどうか設定
		void SetIsEndMotion(bool bSetEndMotion) { bEndMotion = bSetEndMotion; }

		// ブレンドが終了したかどうか設定
		void SetIsEndBlend(bool bSetEndBlend) { bEndBlend = bSetEndBlend; }

		//=====================================================================
		// getter
		//=====================================================================

		// ループ真偽取得処理
		bool GetIsLoop(void) { return bLoop; }

		// モーション終了真偽取得処理
		bool GetIsEndMotion(void) { return bEndMotion; }

		// ブレンド終了真偽取得処理
		bool GetIsEndBlend(void) { return bEndBlend; }

		// ブレンド時間の率(レート)取得処理
		float GetRateBlendTime(void);								

		// モーションタイプ取得処理
		int GetTypeMotion(void) { return nTypeMotion; }

		// 現在のフレーム数取得処理
		int GetFrameNow(void) { return nFrameNow; }

		// 最大のフレーム数取得処理
		int GetFrameMax(void) { return nFrameMax; }

		// ブレンド時間カウンター取得処理
		int GetCntBlendTime(void) { return nCntBlendTime; }

		/* 加算 */

		// 現在フレーム加算処理
		void AddFrameNow(void);	

		// ブレンド時間加算処理
		void AddCntBlendTime(void);	

		/* 減算 */

		// ブレンド時間減算処理
		void SubCntBlendTime(void);

		//=====================================================================
		// flagment
		//=====================================================================

		// 現在のフレーム数が最大に達したかの真偽
		bool IsFrameNowReachedMax(void);	

		// 現在のブレンド時間が最大に達したかの真偽
		bool IsCntBlendTimeMax(void);	

	private:

		int		nTypeMotion		= MOTION_TYPE_NONE;	// モーションタイプ
		int		nFrameNow		= 0;				// 今のフレーム
		int		nFrameMax		= 0;				// 最大フレーム数
		int		nBlendTime		= 0;				// ブレンド時間
		int		nCntBlendTime	= 0;				// ブレンド時間カウンター
		bool	bLoop			= true;				// ループの真偽
		bool	bEndMotion		= false;			// モーションが終ったかの真偽
		bool	bEndBlend		= false;			// ブレンドが終ったかの真偽
	}MotionControlData;

	// コンストラクタ
	CFBXModelComponent(CFBXManager::ArgumentLoad& rArg);

	// デストラクタ
	virtual ~CFBXModelComponent();

	// 初期化
	HRESULT Init(void) override;

	// 終了
	void Uninit(void) override;

	// 更新
	void Update(void) override;

	// 描画
	void Draw(void) override;

	// 描画ボーン無し
	void DrawBoneNone(void);	

	// 描画ボーンあり
	void DrawBoneOwn(void);	

	// モーションの更新
	void UpdateMotion(void);	

	// ブレンドの更新
	void UpdateMotionBlend(void);	

	//=====================================================================
	// setter
	//=====================================================================

	// モーションの設定
	void SetMotion(int nTypeMotion, int nTimeBlend = MOTIONBLEND_TIME);

	//=====================================================================
	// getter
	//=====================================================================

	// モデル名取得処理
	std::string GetModelName(void) { return m_ModelName; }

	// モーション操作情報取得処理
	MotionControlData GetMotionControlData(void) { return m_MotionCntrolData; }

	// ボーンの行列を取得する処理
	D3DXMATRIX GetMtxBone(std::string BoneName);

	// ボーンのワールドマトリックスを取得する処理
	D3DXMATRIX GetMtxWorldBone(std::string BoneName);
	
	// 子ボーンの行列を取得する処理
	D3DXMATRIX GetMtxChildBone(std::string ParentBoneName);
	
	// ボーン情報があるか確認する処理
	bool IsCheckBoneData(std::string BoneName);	

private:
	// ボーンのブレンド処理
	D3DXMATRIX CreateBlendMotionMatrix(std::vector <CFBXManager::MotionData>& rMotionList, std::string& rBoneName);

	std::map<std::string,std::pair<D3DXMATRIX,std::string>>	m_MtxBoneDataOld;			// 古い送るボーンデータ変数
	std::string												m_ModelName;				// モデル名
	MotionControlData										m_MotionCntrolData;			// モーション制御情報
	MotionControlData										m_MotionCntrolDataBlend;	// ブレンドするモーション制御情報
	bool													m_bLighting;				// ライティングの有無

	std::shared_ptr<CShaderResource> m_pShaderFBX;
};
#endif