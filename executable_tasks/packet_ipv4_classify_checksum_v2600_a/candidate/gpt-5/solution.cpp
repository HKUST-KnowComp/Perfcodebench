#include "interface.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace {
constexpr std::size_t kPacketSize = 64;
constexpr uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME = 1099511628211ULL;
}

uint64_t classify_packets_checksum(
    const std::vector<uint8_t>& packets,
    std::size_t packet_count,
    int iters) {
  uint64_t hash = 0;
  const uint8_t* base = packets.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = FNV_OFFSET_BASIS;
    const uint8_t* p = base;

    for (std::size_t i = 0; i < packet_count; ++i) {
      const uint8_t ttl = p[8];
      const uint8_t proto = p[9];

      // Source IP first two octets
      const uint8_t s0 = p[12];
      const uint8_t s1 = p[13];

      // Destination port (network byte order)
      const uint16_t dst_port = static_cast<uint16_t>((static_cast<uint16_t>(p[22]) << 8) | p[23]);

      const uint32_t ttl_bucket = static_cast<uint32_t>(ttl >> 6);

      uint32_t port_bucket;
      if (proto == 6u || proto == 17u) {
        port_bucket = (dst_port < 1024u) ? 0u : ((dst_port < 8192u) ? 1u : 2u);
      } else {
        port_bucket = 3u;
      }

      const bool is_priv = (s0 == 10u) || ((s0 == 172u) && (s1 >= 16u) && (s1 < 32u)) || ((s0 == 192u) && (s1 == 168u));
      const uint32_t priv_weight = is_priv ? 5u : 11u;

      const uint32_t key = static_cast<uint32_t>(proto) * 131u + ttl_bucket * 17u + priv_weight + port_bucket * 23u;

      h ^= static_cast<uint64_t>(key);
      h *= FNV_PRIME;

      p += kPacketSize;
    }

    hash = h; // last iteration defines the result
  }

  return hash;
}
