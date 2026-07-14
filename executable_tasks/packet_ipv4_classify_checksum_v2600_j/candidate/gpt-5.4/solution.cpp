#include "interface.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

constexpr std::size_t kPacketSize = 64;
constexpr uint64_t kFnvOffset = 1469598103934665603ULL;
constexpr uint64_t kFnvPrime = 1099511628211ULL;

}  // namespace

uint64_t classify_packets_checksum(
    const std::vector<uint8_t>& packets,
    std::size_t packet_count,
    int iters) {
  const uint8_t* const data = packets.data();
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    hash = kFnvOffset;
    const uint8_t* p = data;

    for (std::size_t i = 0; i < packet_count; ++i, p += kPacketSize) {
      const uint8_t ttl = p[8];
      const uint8_t protocol = p[9];
      const uint8_t src0 = p[12];
      const uint8_t src1 = p[13];
      const uint8_t dst_port_hi = p[22];
      const uint8_t dst_port_lo = p[23];

      const uint32_t ttl_bucket = static_cast<uint32_t>(ttl >> 6);
      const uint32_t is_tcp_udp =
          static_cast<uint32_t>(protocol == 6u || protocol == 17u);
      const uint32_t dst_port =
          (static_cast<uint32_t>(dst_port_hi) << 8) | static_cast<uint32_t>(dst_port_lo);
      const uint32_t port_bucket =
          is_tcp_udp ? (dst_port < 1024u ? 0u : (dst_port < 8192u ? 1u : 2u)) : 3u;

      const uint32_t is_private =
          static_cast<uint32_t>(src0 == 10u ||
                                (src0 == 172u && src1 >= 16u && src1 < 32u) ||
                                (src0 == 192u && src1 == 168u));

      const uint32_t key = static_cast<uint32_t>(protocol) * 131u +
                           ttl_bucket * 17u +
                           (is_private ? 5u : 11u) +
                           port_bucket * 23u;

      hash ^= static_cast<uint64_t>(key);
      hash *= kFnvPrime;
    }
  }

  return hash;
}
