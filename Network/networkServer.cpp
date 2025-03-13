
#include "networkClass.h"
#include "networkServer.h"
#include "networkContext.h"

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

namespace Network {


	NetworkServer::NetworkServer(NetworkContext& context, uint16_t port) 
		: NetworkBase(context), 
		acceptor(std::make_unique<NetworkAcceptor>(context, port))
	{

	}

	void NetworkServer::TLS_Handshake(std::shared_ptr<NetworkSocket> sock)
	{
		// TODO: First need to check if connected or not theen perform handshake

		sock->socket.async_handshake(boost::asio::ssl::stream_base::server, 
			[this,sock](boost::system::error_code ec) {
				if (!ec) {
					std::cout << "SSL handshake successful (Server)\n";
					std::shared_ptr<Connection> conn = std::make_shared<Connection>(1000, sock);

					clients.push_back(conn);

					Receive(conn);
				}
				else {
					std::cerr << "Handshake failed: " << ec.message() << "\n";
				}
			}
		);
	}

	void NetworkServer::Accept()
	{

		if (acceptor == nullptr) {

			std::cout << "NO ACCEPTOR\n";
			return;
		}

		acceptor->acceptor.async_accept(
			[this](std::error_code ec, boost::asio::ip::tcp::socket socket)
			{
				if (!ec)
				{
					std::cout << "[Server] New Connection: " << socket.remote_endpoint() << "\n";

					TLS_Handshake(std::make_shared<NetworkSocket>(std::move(socket), networkContext));

				}
				else
				{
					std::cout << "[Server] New Connection Error: " << ec.message() << "\n";
				}

				Accept();
			}
		);
	}

	// 0. Get Context
	// 1. Context use certificate and key
	// 2. Acceptor needs to listen on port
	// 3. Create a socket and pass to acceptor the new connection will become that socket and connection established
	// 4. Pass the tcp socket to create a ssl socket
	// 5. Perform tls handshake TLS_Handshake()
	
}
