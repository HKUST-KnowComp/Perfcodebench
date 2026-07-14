#include "interface.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

constexpr std::size_t kPacketSize = 64;

inline bool is_private_ip(const uint8_t* ip) {
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
    const uint8_t* pkt = packets.data();
    for (std::size_t i = 0; i < packet_count; ++i, pkt += kPacketSize) {
      const uint8_t proto = pkt[9];
      const uint8_t ttl = pkt[8];
      const uint16_t dst_port =
          static_cast<uint16_t>((static_cast<uint16_t>(pkt[22]) << 8) | pkt[23]);
      const uint32_t ttl_bucket = static_cast<uint32_t>(ttl >> 6);
      const uint32_t port_bucket =
          (proto == 6u || proto == 17u) ? ((dst_port < 1024u) ? 0u : ((dst_port < 8192u) ? 1u : 2u)) : 3u;
      const uint32_t key =
          static_cast<uint32_t>(proto) * 131u + ttl_bucket * 17u +
          (is_private_ip(pkt + 12) ? 5u : 11u) + port_bucket * 23u;
      hash ^= static_cast<uint64_t>(key);
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}
