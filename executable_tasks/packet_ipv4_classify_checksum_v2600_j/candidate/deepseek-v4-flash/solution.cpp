#include "interface.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <vector>

namespace {

constexpr std::size_t kPacketSize = 64;

}  // namespace

uint64_t classify_packets_checksum(
    const std::vector<uint8_t>& packets,
    std::size_t packet_count,
    int iters) {
  // Precompute the classification key for each packet.
  std::vector<uint32_t> keys(packet_count);
  const uint8_t* base = packets.data();
  for (std::size_t i = 0; i < packet_count; ++i) {
    const uint8_t* p = base + i * kPacketSize;

    // Read protocol (offset 9)
    uint8_t protocol;
    std::memcpy(&protocol, p + 9, 1);

    // Read TTL (offset 8)
    uint8_t ttl;
    std::memcpy(&ttl, p + 8, 1);
    uint32_t ttl_bucket = static_cast<uint32_t>(ttl >> 6);

    // Read source IP (offset 12-15)
    uint8_t src[4];
    std::memcpy(src, p + 12, 4);
    bool priv = (src[0] == 10u) ||
                (src[0] == 172u && src[1] >= 16u && src[1] < 32u) ||
                (src[0] == 192u && src[1] == 168u);
    uint32_t addr_class = priv ? 5u : 11u;

    // Read destination port (big-endian, offset 22-23)
    uint8_t hi, lo;
    std::memcpy(&hi, p + 22, 1);
    std::memcpy(&lo, p + 23, 1);
    uint16_t dst_port = (static_cast<uint16_t>(hi) << 8) | lo;

    uint32_t port_bucket;
    if (protocol == 6u || protocol == 17u) {
      port_bucket = (dst_port < 1024u) ? 0u : ((dst_port < 8192u) ? 1u : 2u);
    } else {
      port_bucket = 3u;
    }

    keys[i] = static_cast<uint32_t>(protocol) * 131u +
              ttl_bucket * 17u +
              addr_class +
              port_bucket * 23u;
  }

  // Apply FNV-1a hash over the keys for each iteration.
  constexpr uint64_t kOffset = 1469598103934665603ULL;
  constexpr uint64_t kPrime = 1099511628211ULL;

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = kOffset;
    for (std::size_t i = 0; i < packet_count; ++i) {
      hash ^= static_cast<uint64_t>(keys[i]);
      hash *= kPrime;
    }
  }

  return hash;
}
