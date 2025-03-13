
#pragma once
#include "util.h"

#include <memory>

namespace Network {

	class NetworkContextImpl;

	class NetworkContext {
	public:
		explicit NetworkContext(ConnectionSystem system);
		~NetworkContext();

		NetworkContext(const NetworkContext&) = delete;
		NetworkContext& operator=(const NetworkContext&) = delete;

		std::unique_ptr<NetworkContextImpl> impl;  // Hide implementation details
	};

	NetworkContext::NetworkContext(ConnectionSystem system)
		: impl(std::make_unique<NetworkContextImpl>(system)) {
	}

	NetworkContext::~NetworkContext() = default;  // Destructor needed for unique_ptr

}