#pragma once

#include <memory>
#include <vector>

#include "networkDeque.h"
#include "util.h"
#include "networkContext.h"

namespace Network {

	class NetworkContextImpl;
	

	class NetworkSocket;

	class Connection;

	class NetworkBase {
	public:

		NetworkBase(NetworkContext& context);
		~NetworkBase();

		virtual void TLS_Handshake(std::shared_ptr<NetworkSocket> sock) = 0;
		void Receive(std::shared_ptr<Connection> conn);
		void Send(std::shared_ptr<NetworkSocket> sock, const Packet& packet);

	protected:
		NetworkContext& networkContext;

		PacketDeque networkDeque; // From "networkDeque.h"

	private:
		
	};
}