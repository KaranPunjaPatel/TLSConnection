#pragma once

#include "network.h"
#include <string>

namespace Network {

	class NetworkResolver;

	class NetworkEndpoint;

	class NetworkClient : public NetworkBase
	{
	public:
		NetworkClient(NetworkContext& context);
		virtual void TLS_Handshake(std::shared_ptr<NetworkSocket> sock);

		void Connect(std::shared_ptr<NetworkSocket> sock);
		void Resolve(const std::string& host, const std::string& port);
		void TCP_Connect(std::shared_ptr<NetworkSocket> sock);
		std::shared_ptr<NetworkSocket> CreateSocket();
		void SetMainSocket(std::shared_ptr<NetworkSocket> sock);

	private:
		std::unique_ptr<NetworkResolver> resolver;
		std::unique_ptr<NetworkEndpoint> endpoint;

		std::vector<std::shared_ptr<Connection>> socks;

		std::shared_ptr<Connection> mainSocket;

		bool typeOfConnection; // 0 - Public, 1 - Private

	};

}
