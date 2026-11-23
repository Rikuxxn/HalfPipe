//=============================================================================
//
// リザルト処理 [Result.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _RESULT_H_// このマクロ定義がされていなかったら
#define _RESULT_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Scene.h"

//*****************************************************************************
// 前方宣言
//*****************************************************************************
class CBlockManager;
class CSky;
class CPlayer;


//*****************************************************************************
// リザルトクラス
//*****************************************************************************
class CResult : public CScene
{
public:
	CResult();
	~CResult();

	// プレイヤー生成
	void SpawnPlayer(uint8_t playerId, uint8_t rank);

	// 指定IDのプレイヤーを検索
	CPlayer* FindPlayerById(uint8_t playerId);

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

	void CalculateRanking(void);

	void OnDeviceReset(void) override;

private:

	struct PlayerRanking
	{
		uint8_t playerId;
		int score;
		uint8_t rank;
	};

private:
	static int			m_clearRankIndex;	// クリア時のランクインデックス

	std::unique_ptr<CSky>	m_pSky;

	std::vector<PlayerRanking> m_Rankings;

	// 順位ごとの配置位置
	D3DXVECTOR3 m_RankPositions[4] =
	{
		{ -300.0f, 598.0f, -1998.0f }, // 1位
		{ -100.0f, 598.0f, -1998.0f }, // 2位
		{  100.0f, 598.0f, -1998.0f }, // 3位
		{  300.0f, 598.0f, -1998.0f }  // 4位
	};
};

#endif
