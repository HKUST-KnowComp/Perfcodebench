#include "interface.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

constexpr std::size_t kPacketSize = 64;

uint32_t is_private_ip(const uint8_t* ip) {
  return (ip[0] == 10u) | ((ip[0] == 172u && (ip[1] - 16u) < 16u) << 1) | ((ip[0] == 192u && ip[1] == 168u) << 2);
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
      const uint8_t* packet = packets.data() + i * kPacketSize;
      uint32_t protocol = packet[9];
      uint32_t ttl_bucket = packet[8] >> 6;
      uint16_t dst_port = (packet[22u] << 8) | packet[23u];
      uint32_t port_bucket =
          (protocol == 6u || protocol == 17u) ? ((dst_port < 1024u) ? 0u : ((dst_port < 8192u) ? 1u : 2u)) : 3u;
      uint32_t key =
          protocol * 131u + ttl_bucket * 17u +
          is_private_ip(packet + 12) * 5u + port_bucket * 23u;
      hash ^= static_cast<uint64_t>(key);
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}
