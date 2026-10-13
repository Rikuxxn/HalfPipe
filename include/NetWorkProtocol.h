//=============================================================================
//
// 通信プロトコル [NetWorkProtocol.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _NETWORK_PROTOCOL_H_// このマクロ定義がされていなかったら
#define _NETWORK_PROTOCOL_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************


//*****************************************************************************
// 名前空間 Network
//*****************************************************************************
namespace Network 
{
	constexpr uint8_t MAX_PLAYERS	= 2; 
	constexpr uint16_t DEFAULT_PORT = 22333;
	
	//============================================================ 
	// パケットタイプ
	//============================================================ 
	enum class PacketType : uint8_t
	{ 
		Welcome = 1,
		PlayerInput, 
		Snapshot, 
		Disconnect,
		JoinRequest,
		JoinAccepted,
		JoinRejected,
		GameStart,
		PlayerLeft
	}; 
	
	enum class JoinRejectReason : uint8_t
	{
		ServerFull = 0,
		AlreadyJoined
	};

	//============================================================
	// パケットヘッダー
	//============================================================ 
#pragma pack(push, 1) 
	struct PacketHeader
	{ 
		PacketType type; 
		uint16_t size; 
	};
	
	//============================================================
	// Welcome 
	// サーバー → クライアント
	//============================================================ 
	struct WelcomePacket
	{ 
		PacketHeader header; 
		uint8_t playerId; 
	}; 
	
   //============================================================
   // プレイヤー参加要求
   // クライアント → サーバー
   //============================================================
	struct JoinRequestPacket
	{
		PacketHeader header;
	};

	//============================================================
	// 参加確定
	// サーバー → クライアント
	//============================================================
	struct JoinAcceptedPacket
	{
		PacketHeader header;
		uint8_t playerId;
	};

	//============================================================
	// 参加拒否
	// サーバー → クライアント
	//============================================================
	struct JoinRejectedPacket
	{
		PacketHeader header;
		uint8_t reason;
	};

	//============================================================
	// ゲーム開始
	// サーバー → 全クライアント
	//============================================================
	struct GameStartPacket
	{
		PacketHeader header;
	};

	//============================================================
	// プレイヤー削除
	// サーバー → 全クライアント
	//============================================================
	struct PlayerLeftPacket
	{
		PacketHeader header;
		uint8_t playerId;
	};

	//============================================================
	// プレイヤー入力
	// クライアント → サーバー
	//============================================================ 
	struct PlayerInputPacket 
	{ 
		PacketHeader header; 
		float moveX; 
		float moveY; 
		float moveZ; 
		uint32_t sequence; 
	}; 
	
	//============================================================ 
	// プレイヤー状態
	//============================================================ 
	struct PlayerState 
	{
		uint8_t playerId; 
		float posX; 
		float posY;
		float posZ; 
		float rotY; 
		float velocityX;
		float velocityY; 
		float velocityZ; 
		uint8_t isMoving;
	}; 
	
	//============================================================
	// スナップショット
	// Server -> Clients 
	//============================================================ 
	struct SnapshotPacket 
	{ 
		PacketHeader header; 
		uint8_t playerCount;
		PlayerState players[MAX_PLAYERS]; 
	}; 
#pragma pack(pop) 
	static_assert(sizeof(PacketHeader) == 3, "PacketHeader size error");
}

#endif