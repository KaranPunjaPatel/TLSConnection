#pragma once
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include <iostream>
#include <memory>

#include "util.h"
#include "networkContext.h"

namespace Network {

	class NetworkContextImpl {
	public:
		std::unique_ptr<boost::asio::io_context>   io;
		std::unique_ptr<boost::asio::ssl::context> ctx;

		//std::thread threadContext;
		std::vector<std::thread> threadPool;


		NetworkContextImpl(ConnectionSystem system)
			: io(std::make_unique<boost::asio::io_context>())
		{
			if (system == ConnectionSystem::CLIENT)
			{
				ctx = std::make_unique<boost::asio::ssl::context>(boost::asio::ssl::context::tlsv13_client);

			}
			else if (system == ConnectionSystem::SERVER)
			{
				ctx = std::make_unique<boost::asio::ssl::context>(boost::asio::ssl::context::tlsv13_server);
			}
			else
			{
				std::cerr << "Error: System Connection not supported\n";
			}

			/* 
			Allow multiple threads to process I/O operations by using a thread pool.
			This allows multiple threads to handle incoming connections.
			*/
			for (size_t i = 0; i < std::thread::hardware_concurrency(); ++i) {
				threadPool.emplace_back([this]() { io->run(); });
			}

			
		}

		~NetworkContextImpl() {
			io->stop();
			for (auto& thread : threadPool) {
				if (thread.joinable()) thread.join();
			}
		}
	};

	class NetworkSocket {
	public:
		boost::asio::ssl::stream<boost::asio::ip::tcp::socket> socket;

		NetworkSocket(NetworkContext& context)
			: socket(*(context.impl->io), *(context.impl->ctx)) {
		}

		NetworkSocket(boost::asio::ip::tcp::socket&& t_socket, NetworkContext& context)
			: socket(std::move(t_socket), *(context.impl->ctx)) {
		}
	};

	class NetworkAcceptor {
	public:
		boost::asio::ip::tcp::acceptor acceptor;

		NetworkAcceptor(NetworkContext& context, const boost::asio::ip::port_type port)
			: acceptor(*(context.impl->io), boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
		{ }
	};

	class NetworkResolver {
	public: 
		boost::asio::ip::tcp::resolver resolver;

		NetworkResolver(NetworkContext& context)
			: resolver(*(context.impl->io))
		{ }
	};

	class NetworkEndpoint {
	public:
		boost::asio::ip::basic_resolver_results<boost::asio::ip::tcp> endpoint;

		NetworkEndpoint() {}

		NetworkEndpoint(boost::asio::ip::basic_resolver_results<boost::asio::ip::tcp> t_endpoint)
			: endpoint(t_endpoint)
		{ }
	};

	class Connection {
	public:
		uint64_t id;
		std::shared_ptr<NetworkSocket> socket;

		Connection(uint64_t id, std::shared_ptr<NetworkSocket> sock) : id(id), socket(std::move(sock)){}
	};
}