
#include "network.h" 
#include "networkClass.h"

#include <chrono>


namespace Network {

	NetworkBase::NetworkBase(NetworkContext& context) 
		: networkContext(context)
	{
		
	}

    //NetworkBase::~NetworkBase() = default;

    NetworkBase::~NetworkBase()
    {

    }

	void NetworkBase::Receive(std::shared_ptr<Connection> conn)
	{
        auto packet = std::make_shared<Packet>();
        auto sock =conn->socket;
        // Step 1: Read the header
        boost::asio::async_read(sock->socket,
            boost::asio::buffer(&packet->header, sizeof(PacketHeader)),
            [this, conn, sock, packet](boost::system::error_code ec, std::size_t bytes_transferred) {
                if (!ec) {
                    std::cout << "Header received.\n";

                    // Step 2: Read the payload if there is any
                    if (packet->header.payload_len > 0) {
                        packet->payload.resize(packet->header.payload_len);

                        boost::asio::async_read(sock->socket,
                            boost::asio::buffer(packet->payload.data(), packet->payload.size()),
                            [this, conn, sock, packet](boost::system::error_code ec, std::size_t bytes_transferred) {
                                if (!ec) {
                                    std::cout << "Payload received: " << bytes_transferred << " bytes.\n";
                                    // Now the packet is fully received and can be processed

                                    std::vector<uint8_t> rawData(sizeof(PacketHeader) + packet->payload.size());
                                    std::memcpy(rawData.data(), &packet->header, sizeof(PacketHeader));
                                    std::memcpy(rawData.data() + sizeof(PacketHeader), packet->payload.data(), packet->payload.size());

                                    // Create InternalPacket
                                    InternalPacket internalPacket{
                                        conn->id,
                                        static_cast<uint64_t>(std::chrono::system_clock::now().time_since_epoch().count()), // Timestamp
                                        std::move(rawData)
                                    };

                                    // Push to queue
                                    networkDeque.push_back(std::move(internalPacket));


                                }
                                else {
                                    std::cerr << "Payload receive failed: " << ec.message() << "\n";
                                }
                            }
                        );
                    }
                }
                else {
                    std::cerr << "Header receive failed: " << ec.message() << "\n";
                }

                Receive(conn);
            }
        );
	}

	void NetworkBase::Send(std::shared_ptr<NetworkSocket> sock, const Packet& packet)
	{
        std::vector<boost::asio::const_buffer> buffers;

        // Send the header first
        buffers.push_back(boost::asio::buffer(&packet.header, sizeof(PacketHeader)));

        // Send the payload (only if it's not empty)
        if (!packet.payload.empty()) {
            buffers.push_back(boost::asio::buffer(packet.payload.data(), packet.payload.size()));
        }

        boost::asio::async_write(sock->socket, buffers,
            [sock](boost::system::error_code ec, std::size_t bytes_transferred) {
                if (!ec) {
                    std::cout << "Packet sent successfully.\n";
                }
                else {
                    std::cerr << "Send failed: " << ec.message() << "\n";
                }
            }
        );
	}

}