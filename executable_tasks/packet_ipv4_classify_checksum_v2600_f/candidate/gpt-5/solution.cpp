#include "interface.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace {
constexpr std::size_t kPacketSize = 64;

static inline bool is_private_ip_bytes(const uint8_t a, const uint8_t b) {
  // Only need first two octets for the private ranges used
  // 10.0.0.0/8, 172.16.0.0/12, 192.168.0.0/16
  return (a == 10u) || (a == 172u && b >= 16u && b < 32u) || (a == 192u && b == 168u);
}
}  // namespace

uint64_t classify_packets_checksum(
    const std::vector<uint8_t>& packets,
    std::size_t packet_count,
    int iters) {
  // Preserve baseline semantics: if iters <= 0, return 0
  if (iters <= 0) {
    return 0ULL;
  }

  const uint8_t* data = packets.data();
  uint64_t hash = 1469598103934665603ULL;  // FNV-1a 64-bit offset basis

  const uint8_t* p = data;
  for (std::size_t i = 0; i < packet_count; ++i, p += kPacketSize) {
    const uint8_t ttl = p[8];
    const uint8_t protocol = p[9];

    const uint32_t ttl_bucket = static_cast<uint32_t>(ttl >> 6);

    uint32_t port_bucket = 3u;
    if (protocol == 6u || protocol == 17u) {  // TCP or UDP
      const uint16_t dst_port = static_cast<uint16_t>(
          (static_cast<uint16_t>(p[22]) << 8) | static_cast<uint16_t>(p[23]));
      port_bucket = (dst_port < 1024u) ? 0u : ((dst_port < 8192u) ? 1u : 2u);
    }

    const bool priv = is_private_ip_bytes(p[12], p[13]);

    const uint32_t key = static_cast<uint32_t>(protocol) * 131u +
                         ttl_bucket * 17u +
                         (priv ? 5u : 11u) +
                         port_bucket * 23u;

    hash ^= static_cast<uint64_t>(key);
    hash *= 1099511628211ULL;  // FNV-1a 64-bit prime
  }

  return hash;
}
