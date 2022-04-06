#pragma once

namespace common
{
	class Socket final
	{
	public:
		using Fnc = function<void(ENetEvent&)>;
		enum Type : u8
		{
			Client,
			Server
		};
		
		Socket(Type type, Fnc on_connect, Fnc on_packet, Fnc on_disconnect, szt peers = 0);
		~Socket();

		[[nodiscard]] auto IsClient() const { return _type == Type::Client; }
		
		bool Connect(const string& ip, u16 port);
		
		void Send(OutPktRef out, enet_uint8 channel = 0u) const;
		void Send(ENetPeer* peer, OutPktRef out, const enet_uint8 channel) const;

		void Tick() const;

		static string FetchIP(ENetPeer * peer);
		
	private:
		static u8 Count;

		Fnc _on_connect, _on_packet, _on_disconnect;

		// Client only
		ENetPeer* _peer;

		ENetHost* _host;
		Type _type;
	};
}