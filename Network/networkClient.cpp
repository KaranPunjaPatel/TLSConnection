
#include "networkClass.h"
#include "networkClient.h"
#include "networkContext.h"

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

namespace Network {

	NetworkClient::NetworkClient(NetworkContext& context) 
		: NetworkBase(context),
		resolver(std::make_unique<NetworkResolver>(context)),
		endpoint(nullptr),
		typeOfConnection(true)
	{
		//Resolve("127.0.0.1", "1234");
	}

	void NetworkClient::TLS_Handshake(std::shared_ptr<NetworkSocket> sock)
	{
		sock->socket.async_handshake(boost::asio::ssl::stream_base::client,
			[this, sock](boost::system::error_code ec) {
				if (!ec) {
					std::cout << "TLS handshake successful.\n";

					// TODO: The conn needs to now call async_receive
					if (mainSocket == nullptr) {

						mainSocket = std::make_shared<Connection>(100, sock);
						Receive(mainSocket);
					}
					else {
						Receive(std::make_shared<Connection>(300, sock));
					}

				}
				else {
					std::cerr << "TLS handshake failed: " << ec.message() << "\n";
				}
			}
		);
	}

	void NetworkClient::Connect(std::shared_ptr<NetworkSocket> sock) // DONT NEED IT, I THINK
	{
		//connect(ssl_socket.next_layer(), endpoints);
	}

	void NetworkClient::Resolve(const std::string& host, const std::string& port)
	{
		if (resolver == nullptr) {
			std::cerr << "Error: Resolver not initialized\n";
			return;
		}

		resolver->resolver.async_resolve(host, port,
			[this, host](boost::system::error_code ec, boost::asio::ip::tcp::resolver::results_type results) {
				if (!ec) {
					endpoint = std::make_unique<NetworkEndpoint>(results);
					std::cout << "Resolved " << host << " to endpoint.\n";
				}
				else {
					std::cerr << "Resolve failed: " << ec.message() << "\n";
				}
			});
		
	}

	void NetworkClient::TCP_Connect(std::shared_ptr<NetworkSocket> sock = nullptr)
	{
		if (sock == nullptr)
			sock = mainSocket->socket;
		boost::asio::async_connect(sock->socket.next_layer(), endpoint->endpoint,
			[this, sock](boost::system::error_code ec, const boost::asio::ip::tcp::endpoint&) {
				if (!ec) {
					std::cout << "TCP Connection established.\n";

					TLS_Handshake(sock);
				}
				else {
					std::cerr << "Connect failed: " << ec.message() << "\n";
				}
			});
	}

	std::shared_ptr<NetworkSocket> NetworkClient::CreateSocket()
	{
		return std::make_shared<NetworkSocket>(networkContext);
	}

	void NetworkClient::SetMainSocket(std::shared_ptr<NetworkSocket> sock)
	{
		if (sock != nullptr) mainSocket->socket->socket.shutdown();
		mainSocket = std::make_shared<Connection>(100,sock);
	}



	// 0. Get Context
	// 1. Resolve Server Address and get endpoint
	// 2. Create a ssl socket with the context
	// 3. Perform tcp connect 
	// 4. Perform tls handshake TLS_Handshake()
	// 
	// WHERE. Verify the Server Certificate by getting it and then authenticate 
	//
	// TCP_Connect
	// TLS_Handshake
	// 
}
