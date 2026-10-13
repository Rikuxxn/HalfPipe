//=============================================================================
//
// 接続画面処理 [Connect.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _CONNECT_H_// このマクロ定義がされていなかったら
#define _CONNECT_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Scene.h"


// 前方宣言
class CSky;
class CPlayer;


//*****************************************************************************
// 接続画面クラス
//*****************************************************************************
class CConnect : public CScene
{
public:
	CConnect();
	~CConnect();

	HRESULT Init	(void) override;
	void	Uninit	(void) override;
	void	Update	(void) override;
	void	Draw	(void) override;

	// プレイヤー生成
	void SpawnPlayer(uint8_t playerId);

	// 指定IDのプレイヤーを検索
	CPlayer* FindPlayerById(uint8_t playerId);

private:

	std::unique_ptr<CSky>	m_pSky;

	bool m_JoinRequestSent; // 参加要求を送ったか
	bool m_PlayerSpawned;	// プレイヤーを生成したか
};

#endif
