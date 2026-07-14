#include "interface.h"

namespace {
constexpr std::size_t kPacketSize = 64;
}

uint64_t classify_packets_checksum(
    const std::vector<uint8_t>& packets,
    std::size_t packet_count,
    int iters) {
  const uint8_t* data = packets.data();
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL; // FNV offset basis
    const uint8_t* p = data;
    for (std::size_t i = 0; i < packet_count; ++i, p += kPacketSize) {
      const uint8_t ttl = p[8];
      const uint8_t protocol = p[9];
      const uint32_t ttl_bucket = static_cast<uint32_t>(ttl >> 6);

      const uint16_t dst_port = static_cast<uint16_t>((static_cast<uint16_t>(p[22]) << 8) | p[23]);

      const uint8_t s0 = p[12];
      const uint8_t s1 = p[13];
      const bool private_ip = (s0 == 10u) ||
                              (s0 == 172u && s1 >= 16u && s1 < 32u) ||
                              (s0 == 192u && s1 == 168u);

      uint32_t port_bucket;
      if (protocol == 6u || protocol == 17u) {
        port_bucket = (dst_port < 1024u) ? 0u : ((dst_port < 8192u) ? 1u : 2u);
      } else {
        port_bucket = 3u;
      }

      const uint32_t key = static_cast<uint32_t>(protocol) * 131u +
                           ttl_bucket * 17u +
                           (private_ip ? 5u : 11u) +
                           port_bucket * 23u;

      hash ^= static_cast<uint64_t>(key);
      hash *= 1099511628211ULL; // FNV prime
    }
  }
  return hash;
}
