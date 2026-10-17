//=============================================================================
//
// 通信マネージャー [NetWorkManager.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************

#define ENET_IMPLEMENTATION 
#include "enet.h" 

#include "NetworkManager.h"


//=============================================================================
// コンストラクタ
//=============================================================================
CNetworkManager::CNetworkManager() : 
	m_Host					(nullptr), 
	m_ServerPeer			(nullptr), 
	m_IsConnected			(false), 
	m_IsJoined				(false),
	m_IsGameReady			(false),
	m_HasPlayerLeft			(false),
	m_LocalPlayerId			(255),
	m_LeftPlayerId			(255)

{ 
	std::memset(m_PlayerStates, 0, sizeof(m_PlayerStates));
	std::memset(m_HasPlayerState, 0, sizeof(m_HasPlayerState)); 
} 

//=============================================================================
// デストラクタ
//=============================================================================
CNetworkManager::~CNetworkManager()
{ 
	Uninit();
} 

//=============================================================================
// 初期化処理
//=============================================================================
bool CNetworkManager::Init(void)
{ 
	if (enet_initialize() != 0) 
	{ 
		MessageBox(nullptr, "[Network] enet 初期化に失敗", "警告！", MB_ICONWARNING);
		return false; 
	}
	
	OutputDebugStringA("[Network] ENet 初期化済み\n");

	return true; 
} 

//=============================================================================
// 終了処理
//=============================================================================
void CNetworkManager::Uninit(void)
{ 
	// サーバーから切断する
	Disconnect();

	if (m_Host)
	{ 
		enet_host_destroy(m_Host); 
		m_Host = nullptr; 
	} 
	
	m_ServerPeer = nullptr;
	m_IsConnected = false;
	m_IsJoined = false;
	m_IsGameReady = false;
	m_LocalPlayerId = 255;

	if (enet_linked_version() != 0)
	{
		enet_deinitialize(); 
	} 
}

//=============================================================================
// 接続
//=============================================================================
bool CNetworkManager::Connect(const char* addressString, uint16_t port)
{ 
	if (m_Host)
	{
		return false;
	} 
	
	m_Host = enet_host_create( nullptr, 1, 2, 0, 0);
	
	if (!m_Host)
	{
		MessageBox(nullptr, "[Network] クライアントの生成に失敗\n", "警告！", MB_ICONWARNING);

		return false;
	} 
	
	ENetAddress address{};
	address.port = port;
	
	if (enet_address_set_host(&address, addressString) != 0) 
	{ 
		char debugMsg[256];

		sprintf_s(
			debugMsg,
			"[Network] 無効なサーバーアドレス : %s\n",
			addressString);

		OutputDebugStringA(debugMsg);

		//std::printf( "[Network] Invalid server address: %s\n", addressString);
		enet_host_destroy(m_Host);
		m_Host = nullptr; 
		
		return false;
	}
	
	m_ServerPeer = enet_host_connect(m_Host, &address, 2, 0); 
	
	if (!m_ServerPeer)
	{ 
		OutputDebugStringA("[Network] 接続失敗\n");
		enet_host_destroy(m_Host);
		m_Host = nullptr; 
		
		return false; 
	} 
	
	char debugMsg[256];

	sprintf_s(
		debugMsg,
		"[Network] 接続中 %s:%u...\n",
		addressString,
		port);

	OutputDebugStringA(debugMsg);

	//std::printf( "[Network] Connecting to %s:%u...\n", addressString, port); 
	
	return true;
} 

//=============================================================================
// LAN内のServerを探して接続
//=============================================================================
bool CNetworkManager::FindLANServer(uint16_t port)
{
	// すでに接続済みなら検索しない
	if (m_Host)
	{
		return false;
	}

	SOCKET socketHandle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (socketHandle == INVALID_SOCKET)
	{
		OutputDebugStringA("[Network] Discovery Socketの生成に失敗\n");

		return false;
	}

	//=========================================================
	// Broadcastを許可
	//=========================================================

	BOOL broadcastEnable = TRUE;

	setsockopt(
		socketHandle,
		SOL_SOCKET,
		SO_BROADCAST,
		reinterpret_cast<const char*>(&broadcastEnable),
		sizeof(broadcastEnable)
	);

	//=========================================================
	// 受信タイムアウト
	//=========================================================

	DWORD timeout = 1000; // 1秒

	setsockopt(
		socketHandle,
		SOL_SOCKET,
		SO_RCVTIMEO,
		reinterpret_cast<const char*>(&timeout),
		sizeof(timeout)
	);

	//=========================================================
	// Broadcast先
	//=========================================================

	sockaddr_in broadcastAddress{};

	broadcastAddress.sin_family = AF_INET;
	broadcastAddress.sin_port = htons(22334);
	broadcastAddress.sin_addr.s_addr = htonl(INADDR_BROADCAST);

	const char* message = "RIKU_GAME_DISCOVER";

	//=========================================================
	// Discovery送信
	//=========================================================

	int sendResult = sendto(
		socketHandle,
		message,
		static_cast<int>(strlen(message)),
		0,
		reinterpret_cast<sockaddr*>(&broadcastAddress),
		sizeof(broadcastAddress)
	);

	if (sendResult == SOCKET_ERROR)
	{
		OutputDebugStringA("[Network] LANサーバー検索の送信に失敗\n");

		closesocket(socketHandle);

		return false;
	}

	OutputDebugStringA("[Network] LANサーバーを検索中...\n");

	//=========================================================
	// Serverからの返答を待つ
	//=========================================================

	char buffer[256]{};

	sockaddr_in serverAddress{};
	int serverAddressSize = sizeof(serverAddress);

	int receiveResult = recvfrom(
		socketHandle,
		buffer,
		sizeof(buffer) - 1,
		0,
		reinterpret_cast<sockaddr*>(&serverAddress),
		&serverAddressSize
	);

	closesocket(socketHandle);

	if (receiveResult == SOCKET_ERROR)
	{
		OutputDebugStringA("[Network] LANサーバーが見つかりませんでした\n");

		return false;
	}

	buffer[receiveResult] = '\0';

	//=========================================================
	// Serverからの応答確認
	//=========================================================

	if (strcmp(buffer, "RIKU_GAME_SERVER") != 0)
	{
		OutputDebugStringA("[Network] 不正なDiscovery応答\n");

		return false;
	}

	//=========================================================
	// 送信元IPを取得
	//=========================================================

	char serverIP[INET_ADDRSTRLEN]{};

	inet_ntop(
		AF_INET,
		&serverAddress.sin_addr,
		serverIP,
		sizeof(serverIP)
	);

	char debugMsg[256];

	sprintf_s(
		debugMsg,
		"[Network] LANサーバー発見 : %s:%u\n",
		serverIP,
		port
	);

	OutputDebugStringA(debugMsg);

	//=========================================================
	// 既存のENet接続を利用
	//=========================================================

	return Connect(serverIP, port);
}

//=============================================================================
// サーバーから切断
//=============================================================================
void CNetworkManager::Disconnect(void)
{
	if (!m_Host)
	{
		return;
	}

	if (m_ServerPeer)
	{
		// サーバーに切断を通知する
		enet_peer_disconnect(m_ServerPeer, 0);

		ENetEvent event{};

		// 切断イベントを待つ
		while (enet_host_service(m_Host, &event, 100) > 0)
		{
			if (event.type == ENET_EVENT_TYPE_RECEIVE)
			{
				enet_packet_destroy(event.packet);
			}
			else if (event.type == ENET_EVENT_TYPE_DISCONNECT)
			{
				break;
			}
		}

		// 相手から応答がない場合も接続を終了する
		if (m_ServerPeer)
		{
			enet_peer_reset(m_ServerPeer);
			m_ServerPeer = nullptr;
		}
	}
}

//=============================================================================
// 更新
//=============================================================================
 void CNetworkManager::Update(void)
 { 
	if (!m_Host)
	{
		 return; 
	} 

	ENetEvent event{};

	while (enet_host_service(m_Host, &event, 0) > 0) 
	{ 
	switch (event.type)
	{ 
	case ENET_EVENT_TYPE_CONNECT: 
		OnConnect(event.peer); 
		break; 

	case ENET_EVENT_TYPE_RECEIVE: 
		OnReceive( event.peer, event.packet); 
		enet_packet_destroy(event.packet); 
		break; 

	case ENET_EVENT_TYPE_DISCONNECT:
		OnDisconnect(event.peer); 
		break; 

	default: 
		break; 
	} 
	}
 }

//=============================================================================
// 接続時
//=============================================================================
 void CNetworkManager::OnConnect(ENetPeer* peer)
 { 
	 if (peer == m_ServerPeer)
	 {
		 OutputDebugStringA("[Network] サーバーに接続しました\n");
	 }
 }

//=============================================================================
// 受信
//=============================================================================
void CNetworkManager::OnReceive( ENetPeer* peer, ENetPacket* packet)
{ 
	if (!packet)
	{ 
		return; 
	} 
	
	if (packet->dataLength < sizeof(Network::PacketHeader))
	{
		return; 
	}
	
	Network::PacketHeader header{};
	std::memcpy( &header, packet->data, sizeof(header)); 
	
	if (header.type == Network::PacketType::Welcome)
	{
		if (packet->dataLength < sizeof(Network::WelcomePacket))
		{
			return;
		}

		Network::WelcomePacket welcome{};
		std::memcpy(&welcome, packet->data, sizeof(welcome));
		m_LocalPlayerId = welcome.playerId;
		m_IsConnected = true;

		char debugMsg[256];

		sprintf_s(
			debugMsg,
			"[Network] 割り当てられたプレイヤーID = %u\n",
			m_LocalPlayerId);

		OutputDebugStringA(debugMsg);

		//std::printf( "[Network] Assigned PlayerID = %u\n", m_LocalPlayerId); 
	}
	else if (header.type == Network::PacketType::Snapshot)
	{
		if (packet->dataLength < sizeof(Network::SnapshotPacket))
		{
			return;
		}

		Network::SnapshotPacket snapshot{};
		std::memcpy(&snapshot, packet->data, sizeof(snapshot));

		if (snapshot.playerCount > Network::MAX_PLAYERS)
		{
			return;
		}

		// 今回のスナップショットに含まれるプレイヤーだけ有効にする
		std::memset(m_HasPlayerState, 0, sizeof(m_HasPlayerState));

		for (uint8_t i = 0; i < snapshot.playerCount; ++i)
		{
			const Network::PlayerState& state = snapshot.players[i];

			if (state.playerId >= Network::MAX_PLAYERS)
			{
				continue;
			}

			m_PlayerStates[state.playerId] = state;
			m_HasPlayerState[state.playerId] = true;
		}
	}
	else if (header.type == Network::PacketType::JoinAccepted)
	{
		if (packet->dataLength <sizeof(Network::JoinAcceptedPacket))
		{
			return;
		}

		Network::JoinAcceptedPacket response{};

		std::memcpy(
			&response,
			packet->data,
			sizeof(response)
		);

		if (response.playerId >= Network::MAX_PLAYERS)
		{
			return;
		}

		m_LocalPlayerId = response.playerId;
		m_IsJoined = true;

		OutputDebugStringA("[Network] ゲームへの参加が確定しました\n");
	}
	else if (header.type == Network::PacketType::JoinRejected)
	{
		if (packet->dataLength < sizeof(Network::JoinRejectedPacket))
		{
			return;
		}

		Network::JoinRejectedPacket response{};

		std::memcpy(
			&response,
			packet->data,
			sizeof(response)
		);

		m_IsJoined = false;

		if (response.reason == static_cast<uint8_t>(Network::JoinRejectReason::ServerFull))
		{
			OutputDebugStringA("[Network] サーバーが満員です\n");
		}
	}
	else if (header.type == Network::PacketType::GameStart)
	{
		if (packet->dataLength < sizeof(Network::GameStartPacket))
		{
			return;
		}

		// ゲーム開始通知を受信
		m_IsGameReady = true;

		std::printf("[Network] ゲーム開始\n");
	}
	else if (header.type == Network::PacketType::PlayerLeft)
	{
		if (packet->dataLength < sizeof(Network::PlayerLeftPacket))
		{
			return;
		}
	
		Network::PlayerLeftPacket leftPacket{};
	
		std::memcpy(&leftPacket, packet->data, sizeof(leftPacket));
	
		const uint8_t playerId = leftPacket.playerId;
	
		if (playerId >= Network::MAX_PLAYERS)
		{
			return;
		}
	
		// 状態キャッシュを無効化
		m_HasPlayerState[playerId] = false;
	
		char debugMsg[256];

		sprintf_s(
			debugMsg,
			"[Network] Player %u がゲームを離れました\n",
			playerId);

		OutputDebugStringA(debugMsg);

		// ゲーム側でプレイヤーを削除できるように記録する
		m_LeftPlayerId = playerId;
		m_HasPlayerLeft = true;
	}
}

//=============================================================================
// 切断
//=============================================================================
void CNetworkManager::OnDisconnect(ENetPeer* peer)
{
	if (peer == m_ServerPeer)
	{
		m_ServerPeer = nullptr;
		m_IsConnected = false;
		m_IsJoined = false;
		m_IsGameReady = false;

		m_LocalPlayerId = 255;
		OutputDebugStringA("[Network] サーバーから切断されました\n");
	}
}

//=============================================================================
// プレイヤー入力を送る
//=============================================================================
void CNetworkManager::SendPlayerInput(float moveX, float moveY, float moveZ)
{ 	
	if (!m_ServerPeer) 
	{ 
		return; 
	} 
	
	if (!m_IsConnected) 
	{ 
		return; 
	} 
	
	static uint32_t sequence = 0; 
	Network::PlayerInputPacket packet{}; 
	packet.header.type = Network::PacketType::PlayerInput; 
	packet.header.size = sizeof(packet);
	packet.moveX = moveX; 
	packet.moveY = moveY; 
	packet.moveZ = moveZ; 
	packet.sequence = sequence++; 
	ENetPacket* enetPacket = enet_packet_create(&packet, sizeof(packet), 0);
	
	if (!enetPacket) 
	{ 
		return;
	} 
	
	enet_peer_send( m_ServerPeer, 0, enetPacket); enet_host_flush(m_Host); 
}

//=============================================================================
// 参加要求送信
//=============================================================================
void CNetworkManager::SendJoinRequest(void)
{
	if (!m_Host || !m_ServerPeer)
	{
		return;
	}

	if (!m_IsConnected || m_IsJoined)
	{
		return;
	}

	if (m_LocalPlayerId >= Network::MAX_PLAYERS)
	{
		return;
	}

	Network::JoinRequestPacket packet{};

	packet.header.type = Network::PacketType::JoinRequest;
	packet.header.size = sizeof(packet);

	ENetPacket* enetPacket =
		enet_packet_create(
			&packet,
			sizeof(packet),
			ENET_PACKET_FLAG_RELIABLE
		);

	if (!enetPacket)
	{
		return;
	}

	if (enet_peer_send(
		m_ServerPeer,
		0,
		enetPacket) != 0)
	{
		enet_packet_destroy(enetPacket);
		return;
	}

	enet_host_flush(m_Host);

	OutputDebugStringA("[Network] 参加要求を送信しました\n");
}

//=============================================================================
// プレイヤー状態の入力
//=============================================================================
bool CNetworkManager::GetPlayerState(uint8_t playerId, Network::PlayerState& outState) const
{ 
	if (playerId >= Network::MAX_PLAYERS) 
	{ 
		return false; 
	} 
	
	if (!m_HasPlayerState[playerId])
	{ 
		return false; 
	} 
	
	outState = m_PlayerStates[playerId]; 
	
	return true; 
}

//=============================================================================
// プレイヤーがゲームを離れたか
//=============================================================================
bool CNetworkManager::ConsumePlayerLeft(uint8_t& playerId)
{
	if (!m_HasPlayerLeft)
	{
		return false;
	}

	playerId = m_LeftPlayerId;

	m_LeftPlayerId = 255;
	m_HasPlayerLeft = false;

	return true;
}