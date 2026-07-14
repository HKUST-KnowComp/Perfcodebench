#include "interface.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

constexpr std::size_t kPacketSize = 64;

struct Header {
  uint8_t version_ihl;
  uint8_t tos;
  uint16_t total_length;
  uint16_t id;
  uint16_t frag;
  uint8_t ttl;
  uint8_t protocol;
  uint16_t checksum;
  uint8_t src[4];
  uint8_t dst[4];
  uint16_t src_port;
  uint16_t dst_port;
};

bool is_private_ip(const uint8_t* ip) {
  return ip[0] == 10u ||
         (ip[0] == 172u && ip[1] >= 16u && ip[1] < 32u) ||
         (ip[0] == 192u && ip[1] == 168u);
}

}  // namespace

uint64_t classify_packets_checksum(
    const std::vector<uint8_t>& packets,
    std::size_t packet_count,
    int iters) {
  uint64_t hash = 0;
  const uint8_t* packets_data = packets.data();
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < packet_count; ++i) {
      const uint8_t* packet_data = packets_data + i * kPacketSize;
      uint8_t protocol = packet_data[9];
      uint8_t ttl = packet_data[8];
      uint8_t src_ip[4];
      std::memcpy(src_ip, packet_data + 12, 4);
      uint16_t dst_port = (packet_data[22] << 8) | packet_data[23];
      const uint32_t ttl_bucket = static_cast<uint32_t>(ttl >> 6);
      const uint32_t port_bucket =
          (protocol == 6u || protocol == 17u) ? ((dst_port < 1024u) ? 0u : ((dst_port < 8192u) ? 1u : 2u)) : 3u;
      const uint32_t key =
          static_cast<uint32_t>(protocol) * 131u + ttl_bucket * 17u +
          (is_private_ip(src_ip) ? 5u : 11u) + port_bucket * 23u;
      hash ^= static_cast<uint64_t>(key);
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}
