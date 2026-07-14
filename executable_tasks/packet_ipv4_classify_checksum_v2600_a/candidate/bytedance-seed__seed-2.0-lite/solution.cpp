#include "interface.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace {
constexpr std::size_t kPacketSize = 64;
constexpr uint64_t kFnvOffset = 1469598103934665603ULL;
constexpr uint64_t kFnvPrime = 1099511628211ULL;

inline constexpr bool is_private_ip(const uint8_t* ip) {
  return ip[0] == 10u ||
         (ip[0] == 172u && ip[1] >= 16u && ip[1] < 32u) ||
         (ip[0] == 192u && ip[1] == 168u);
}
}  // namespace

uint64_t classify_packets_checksum(
    const std::vector<uint8_t>& packets,
    std::size_t packet_count,
    int iters) {
  const uint8_t* pkt_base = packets.data();
  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = kFnvOffset;
    const uint8_t* current_pkt = pkt_base;
    for (std::size_t i = 0; i < packet_count; ++i) {
      const uint8_t ttl = current_pkt[8];
      const uint8_t proto = current_pkt[9];
      const uint16_t dst_port = (static_cast<uint16_t>(current_pkt[22]) << 8) | current_pkt[23];
      const bool private_src = is_private_ip(current_pkt + 12);

      const uint32_t ttl_bucket = static_cast<uint32_t>(ttl >> 6);
      const uint32_t port_bucket = (proto == 6u || proto == 17u)
        ? (dst_port < 1024u ? 0u : (dst_port < 8192u ? 1u : 2u))
        : 3u;

      const uint32_t key = static_cast<uint32_t>(proto) * 131u +
                           ttl_bucket * 17u +
                           (private_src ? 5u : 11u) +
                           port_bucket * 23u;

      hash ^= static_cast<uint64_t>(key);
      hash *= kFnvPrime;
      current_pkt += kPacketSize;
    }
    final_hash = hash;
  }
  return final_hash;
}