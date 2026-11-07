//=============================================================================
//
// 通信マネージャー [NetWorkManager.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _NETWORK_MANAGER_H_// このマクロ定義がされていなかったら
#define _NETWORK_MANAGER_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "NetworkProtocol.h"
#include <enet.h>

//*****************************************************************************
// 通信マネージャークラス (クライアント)
//*****************************************************************************
class CNetworkManager
{
public: 
	CNetworkManager();
	~CNetworkManager(); 
	
	bool Init	(void);
	void Uninit	(void);
		
	//============================================================ 
	// クライアントからサーバーに接続
	//============================================================ 
	bool Connect(const char* address, uint16_t port = Network::DEFAULT_PORT);
	
	//============================================================ 
	// サーバーから切断
	//============================================================ 
	void Disconnect(void);

	//============================================================ 
	// LAN内のServerを探して接続
	//============================================================ 
	bool FindLANServer(uint16_t port);

	//============================================================ 
	// 更新 
	//============================================================ 
	void Update(void); 
	
	//============================================================ 
	// 送信 (入力と直接)
	//============================================================ 
	void SendPlayerInput(float moveX, float moveY, float moveZ); 
	void SendPlayerTransform(const D3DXVECTOR3& position, const D3DXVECTOR3& rotation, const D3DXQUATERNION& quaternion);

	//============================================================ 
	// 情報 
	//============================================================ 
	bool	IsConnected				(void) const { return m_IsConnected; }
	bool	IsGameReady				(void) const { return m_IsGameReady; }
	bool	IsGameFinish			(void) const { return m_IsGameFinish; }
	bool	ConsumePlayerLeft		(uint8_t& playerId);
	uint8_t GetLocalPlayerId		(void) const { return m_LocalPlayerId; } 

	const std::vector<uint8_t>& GetActivePlayerIds(void) const { return m_ActivePlayerIds;}

	//============================================================ 
	// クライアントスナップショット
	//============================================================ 
	bool GetPlayerState(uint8_t playerId, Network::PlayerState& outState) const; 

	void SendJoinRequest(D3DXVECTOR3 pos, D3DXVECTOR3 rot);
	void SendPlayerGoal(void);

	bool IsJoined(void) const {	return m_IsJoined; }

	void ResetJoinState(void) {	m_IsJoined = false;	}

private:

	void		OnConnect			(ENetPeer* peer); 
	void		OnReceive			(ENetPeer* peer, ENetPacket* packet); 
	void		OnDisconnect		(ENetPeer* peer); 

private:
	// 参加中のプレイヤーID
	std::vector<uint8_t> m_ActivePlayerIds;

	ENetPeer*					m_ServerPeer;
	ENetHost*					m_Host;
	uint8_t						m_LocalPlayerId;
	uint8_t						m_LeftPlayerId;
	Network::PlayerState		m_PlayerStates[Network::MAX_PLAYERS];
	bool						m_HasPlayerState[Network::MAX_PLAYERS];
	bool						m_IsConnected;
	bool						m_IsJoined;
	bool						m_IsGameReady;
	bool						m_IsGameFinish;
	bool						m_HasPlayerLeft;
};

#endif