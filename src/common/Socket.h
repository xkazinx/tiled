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
		
		static u8 Count;
		
		ENetHost* _host;
		Type _type;

		[[nodiscard]] auto IsClient() const { return _type == Type::Client; }

		// Must call enet_packet_destroy(ev.packet); after processing packet
		bool Service(ENetEvent& ev) const;
		void Tick() const;

		// Client only
		ENetPeer* _peer;

		bool Connect(const string& ip, u16 port);
		
		void Send(OutPktRef out, enet_uint8 channel = 0u) const;
		void Send(ENetPeer* peer, OutPktRef out, const enet_uint8 channel) const;

		static string FetchIP(ENetPeer * peer);

		Fnc _on_connect, _on_packet, _on_disconnect;
	};
}