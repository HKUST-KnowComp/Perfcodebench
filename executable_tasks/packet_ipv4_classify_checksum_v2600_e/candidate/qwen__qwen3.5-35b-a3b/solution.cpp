#include "interface.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

constexpr std::size_t kPacketSize = 64;

inline bool is_private_ip(const uint8_t* ip) {
  uint8_t ip0 = ip[0];
  uint8_t ip1 = ip[1];
  return (ip0 == 10) ||
         (ip0 == 172 && ip1 >= 16 && ip1 < 32) ||
         (ip0 == 192 && ip1 == 168);
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
      const uint8_t* pkt = packets.data() + i * kPacketSize;
      uint8_t ttl = pkt[8];
      uint8_t protocol = pkt[9];
      const uint8_t* src = pkt + 12;
      const uint16_t dst_port = (pkt[22] << 8) | pkt[23];
      const uint32_t ttl_bucket = ttl >> 6;
      const uint32_t port_bucket =
          (protocol == 6 || protocol == 17) ? ((dst_port < 1024) ? 0 : ((dst_port < 8192) ? 1 : 2)) : 3;
      const uint32_t key =
          static_cast<uint32_t>(protocol) * 131u + ttl_bucket * 17u +
          (is_private_ip(src) ? 5u : 11u) + port_bucket * 23u;
      hash ^= static_cast<uint64_t>(key);
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}