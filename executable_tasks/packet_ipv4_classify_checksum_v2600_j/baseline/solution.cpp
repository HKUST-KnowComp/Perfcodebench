#include "interface.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
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
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < packet_count; ++i) {
      Header h{};
      std::memcpy(&h.version_ihl, packets.data() + i * kPacketSize, 20u);
      std::memcpy(&h.src_port, packets.data() + i * kPacketSize + 20u, 2u);
      std::memcpy(&h.dst_port, packets.data() + i * kPacketSize + 22u, 2u);
      const uint16_t dst_port =
          static_cast<uint16_t>((static_cast<uint16_t>(packets[i * kPacketSize + 22u]) << 8) |
                                packets[i * kPacketSize + 23u]);
      const uint32_t ttl_bucket = static_cast<uint32_t>(h.ttl >> 6);
      const uint32_t port_bucket =
          (h.protocol == 6u || h.protocol == 17u) ? ((dst_port < 1024u) ? 0u : ((dst_port < 8192u) ? 1u : 2u)) : 3u;
      const uint32_t key =
          static_cast<uint32_t>(h.protocol) * 131u + ttl_bucket * 17u +
          (is_private_ip(h.src) ? 5u : 11u) + port_bucket * 23u;
      hash ^= static_cast<uint64_t>(key);
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}
