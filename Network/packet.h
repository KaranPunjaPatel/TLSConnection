#pragma once

#include <cstdint>
#include <vector>

struct InternalPacket {
	uint64_t connectionId;  // Unique identifier for the connection
	uint64_t timestamp;     // Time received
	std::vector<uint8_t> data; // The received packet (original format)
};

struct PacketHeader {
	uint16_t magic_bytes;
	uint16_t version;
	uint16_t type;
	uint16_t payload_len;
	uint8_t  reserved[8];
};

struct Packet {
	PacketHeader header;
	std::vector<uint8_t> payload;
};

