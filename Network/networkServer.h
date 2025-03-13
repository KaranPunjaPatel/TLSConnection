#pragma once

#include "network.h"

#include <unordered_map>


namespace Network {

	class NetworkAcceptor;

	class Connection;

	class NetworkServer : public NetworkBase
	{
	public:
		NetworkServer(NetworkContext& context, uint16_t port);

		virtual void TLS_Handshake(std::shared_ptr<NetworkSocket> sock);

		void Accept();

	private:
		std::unique_ptr<NetworkAcceptor> acceptor;

		std::vector<std::shared_ptr<Connection>> clients;

		std::unordered_map<uint64_t, std::shared_ptr<Connection>> activeConnections;


	};

}

