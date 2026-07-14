#include "interface.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace {
constexpr std::size_t kPacketSize = 64;
}

uint64_t classify_packets_checksum(
    const std::vector<uint8_t>& packets,
    std::size_t packet_count,
    int iters) {
  const uint8_t* base = packets.data();
  const std::size_t stride = kPacketSize;
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    const uint8_t* p = base;
    for (std::size_t i = 0; i < packet_count; ++i, p += stride) {
      const uint8_t proto = p[9];
      const uint32_t ttl_bucket = static_cast<uint32_t>(p[8] >> 6);
      const uint16_t dst_port = static_cast<uint16_t>((static_cast<uint16_t>(p[22]) << 8) | p[23]);

      // Inline private IP detection for source address
      const uint8_t src0 = p[12];
      uint32_t src_term;
      if (src0 == 10u) {
        src_term = 5u;
      } else if (src0 == 172u) {
        const uint8_t src1 = p[13];
        src_term = (static_cast<uint8_t>(src1 - 16u) < 16u) ? 5u : 11u;
      } else if (src0 == 192u && p[13] == 168u) {
        src_term = 5u;
      } else {
        src_term = 11u;
      }

      uint32_t port_bucket;
      if (proto == 6u || proto == 17u) {
        if (dst_port < 1024u) {
          port_bucket = 0u;
        } else if (dst_port < 8192u) {
          port_bucket = 1u;
        } else {
          port_bucket = 2u;
        }
      } else {
        port_bucket = 3u;
      }

      const uint32_t key = static_cast<uint32_t>(proto) * 131u + ttl_bucket * 17u + src_term + port_bucket * 23u;
      hash ^= static_cast<uint64_t>(key);
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}
