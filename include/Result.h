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
class CGameObject;


//*****************************************************************************
// リザルトクラス
//*****************************************************************************
class CResult : public CScene
{
public:
	CResult();
	~CResult();

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

	void OnDeviceReset(void) override;

private:

	struct PlayerRanking
	{
		uint8_t playerId;
		int score;
		uint8_t rank;
	};

	// 指定IDのプレイヤーを検索
	CPlayer* FindPlayerById(uint8_t playerId);

	// プレイヤー生成
	void SpawnPlayer(uint8_t playerId, uint8_t rank);

	// スコア生成
	void SpawnScore(const PlayerRanking& ranking);

	void CalculateRanking(void);

private:
	static int			m_clearRankIndex;	// クリア時のランクインデックス

	std::unique_ptr<CSky>	m_pSky;

	std::vector<PlayerRanking> m_Rankings;

	// 順位ごとのプレイヤー配置位置
	D3DXVECTOR3 m_RankPositions[4] =
	{
		{	0.0f, -1211.0f, 4181.0f }, // 1位
		{ 55.0f, -1228.0f, 4181.0f }, // 2位
		{  -55.0f, -1240.0f, 4181.0f }, // 3位
		{  -130.0f, -1260.0f, 4181.0f }  // 4位
	};

	// 順位ごとのスコア配置位置
	D3DXVECTOR2 m_ScorePositions[4] =
	{
		{	0.56f,	0.33f }, // 1位
		{	0.44f,	0.43f }, // 2位
		{	0.64f,	0.48f }, // 3位
		{	0.74f,	0.55f }  // 4位
	};
};

#endif
