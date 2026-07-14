#include "interface.h"
#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

constexpr std::size_t kPacketSize = 64;
constexpr uint64_t kFnvOffset = 1469598103934665603ULL;
constexpr uint64_t kFnvPrime  = 1099511628211ULL;

inline bool is_private_ip(const uint8_t* ip) {
  const uint32_t ip32 = *reinterpret_cast<const uint32_t*>(ip);
  const uint8_t b0 = ip32 & 0xFFu;
  const uint8_t b1 = (ip32 >> 8) & 0xFFu;
  return (b0 == 10u) ||
         (b0 == 172u && b1 >= 16u && b1 < 32u) ||
         (b0 == 192u && b1 == 168u);
}

}  // namespace

uint64_t classify_packets_checksum(
    const std::vector<uint8_t>& packets,
    std::size_t packet_count,
    int iters) {
  const uint8_t* __restrict base = packets.data();
  const std::size_t n = packet_count;

  uint64_t hash = kFnvOffset;
  for (int iter = 0; iter < iters; ++iter) {
    hash = kFnvOffset;
    std::size_t i = 0;

    // Process four packets per iteration
    for (; i + 3 < n; i += 4) {
      const uint8_t* p0 = base + (i + 0) * kPacketSize;
      const uint8_t* p1 = base + (i + 1) * kPacketSize;
      const uint8_t* p2 = base + (i + 2) * kPacketSize;
      const uint8_t* p3 = base + (i + 3) * kPacketSize;

      // Load protocol, TTL, src IP, dst port for each packet
      const uint8_t proto0 = p0[9];
      const uint8_t proto1 = p1[9];
      const uint8_t proto2 = p2[9];
      const uint8_t proto3 = p3[9];

      const uint8_t ttl0 = p0[8];
      const uint8_t ttl1 = p1[8];
      const uint8_t ttl2 = p2[8];
      const uint8_t ttl3 = p3[8];

      const uint32_t src0 = *reinterpret_cast<const uint32_t*>(p0 + 12);
      const uint32_t src1 = *reinterpret_cast<const uint32_t*>(p1 + 12);
      const uint32_t src2 = *reinterpret_cast<const uint32_t*>(p2 + 12);
      const uint32_t src3 = *reinterpret_cast<const uint32_t*>(p3 + 12);

      const uint16_t dst_port0 = *reinterpret_cast<const uint16_t*>(p0 + 22);
      const uint16_t dst_port1 = *reinterpret_cast<const uint16_t*>(p1 + 22);
      const uint16_t dst_port2 = *reinterpret_cast<const uint16_t*>(p2 + 22);
      const uint16_t dst_port3 = *reinterpret_cast<const uint16_t*>(p3 + 22);

      // Compute keys
      const uint32_t ttl_bucket0 = ttl0 >> 6;
      const uint32_t port_bucket0 = (proto0 == 6u || proto0 == 17u)
          ? ((dst_port0 < 1024u) ? 0u : ((dst_port0 < 8192u) ? 1u : 2u)) : 3u;
      const uint32_t key0 = proto0 * 131u + ttl_bucket0 * 17u +
          ((src0 & 0xFFu) == 10u ||
           ((src0 & 0xFFu) == 172u && ((src0 >> 8) & 0xFFu) >= 16u && ((src0 >> 8) & 0xFFu) < 32u) ||
           ((src0 & 0xFFu) == 192u && ((src0 >> 8) & 0xFFu) == 168u) ? 5u : 11u) +
          port_bucket0 * 23u;

      const uint32_t ttl_bucket1 = ttl1 >> 6;
      const uint32_t port_bucket1 = (proto1 == 6u || proto1 == 17u)
          ? ((dst_port1 < 1024u) ? 0u : ((dst_port1 < 8192u) ? 1u : 2u)) : 3u;
      const uint32_t key1 = proto1 * 131u + ttl_bucket1 * 17u +
          ((src1 & 0xFFu) == 10u ||
           ((src1 & 0xFFu) == 172u && ((src1 >> 8) & 0xFFu) >= 16u && ((src1 >> 8) & 0xFFu) < 32u) ||
           ((src1 & 0xFFu) == 192u && ((src1 >> 8) & 0xFFu) == 168u) ? 5u : 11u) +
          port_bucket1 * 23u;

      const uint32_t ttl_bucket2 = ttl2 >> 6;
      const uint32_t port_bucket2 = (proto2 == 6u || proto2 == 17u)
          ? ((dst_port2 < 1024u) ? 0u : ((dst_port2 < 8192u) ? 1u : 2u)) : 3u;
      const uint32_t key2 = proto2 * 131u + ttl_bucket2 * 17u +
          ((src2 & 0xFFu) == 10u ||
           ((src2 & 0xFFu) == 172u && ((src2 >> 8) & 0xFFu) >= 16u && ((src2 >> 8) & 0xFFu) < 32u) ||
           ((src2 & 0xFFu) == 192u && ((src2 >> 8) & 0xFFu) == 168u) ? 5u : 11u) +
          port_bucket2 * 23u;

      const uint32_t ttl_bucket3 = ttl3 >> 6;
      const uint32_t port_bucket3 = (proto3 == 6u || proto3 == 17u)
          ? ((dst_port3 < 1024u) ? 0u : ((dst_port3 < 8192u) ? 1u : 2u)) : 3u;
      const uint32_t key3 = proto3 * 131u + ttl_bucket3 * 17u +
          ((src3 & 0xFFu) == 10u ||
           ((src3 & 0xFFu) == 172u && ((src3 >> 8) & 0xFFu) >= 16u && ((src3 >> 8) & 0xFFu) < 32u) ||
           ((src3 & 0xFFu) == 192u && ((src3 >> 8) & 0xFFu) == 168u) ? 5u : 11u) +
          port_bucket3 * 23u;

      // Mix keys into hash
      hash ^= key0; hash *= kFnvPrime;
      hash ^= key1; hash *= kFnvPrime;
      hash ^= key2; hash *= kFnvPrime;
      hash ^= key3; hash *= kFnvPrime;
    }

    // Remaining packets
    for (; i < n; ++i) {
      const uint8_t* p = base + i * kPacketSize;
      const uint8_t proto   = p[9];
      const uint8_t ttl     = p[8];
      const uint16_t dst_port = *reinterpret_cast<const uint16_t*>(p + 22);
      const uint32_t src32 = *reinterpret_cast<const uint32_t*>(p + 12);

      const uint32_t ttl_bucket = ttl >> 6;
      const uint32_t port_bucket = (proto == 6u || proto == 17u)
          ? ((dst_port < 1024u) ? 0u : ((dst_port < 8192u) ? 1u : 2u)) : 3u;
      const uint32_t key = proto * 131u + ttl_bucket * 17u +
          ((src32 & 0xFFu) == 10u ||
           ((src32 & 0xFFu) == 172u && ((src32 >> 8) & 0xFFu) >= 16u && ((src32 >> 8) & 0xFFu) < 32u) ||
           ((src32 & 0xFFu) == 192u && ((src32 >> 8) & 0xFFu) == 168u) ? 5u : 11u) +
          port_bucket * 23u;
      hash ^= key;
      hash *= kFnvPrime;
    }
  }
  return hash;
}
