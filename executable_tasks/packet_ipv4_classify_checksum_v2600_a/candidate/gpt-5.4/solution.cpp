#include "interface.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

constexpr std::size_t kPacketSize = 64;
constexpr uint64_t kFnvOffset = 1469598103934665603ULL;
constexpr uint64_t kFnvPrime = 1099511628211ULL;

inline uint32_t classify_one(const uint8_t* p) {
  const uint32_t ttl_bucket = static_cast<uint32_t>(p[8] >> 6);
  const uint32_t protocol = static_cast<uint32_t>(p[9]);

  const uint8_t s0 = p[12];
  const uint8_t s1 = p[13];
  const uint32_t private_ip =
      (s0 == 10u) |
      ((s0 == 172u) & (s1 >= 16u) & (s1 < 32u)) |
      ((s0 == 192u) & (s1 == 168u));

  uint32_t port_bucket = 3u;
  if (protocol == 6u || protocol == 17u) {
    const uint32_t dst_port =
        (static_cast<uint32_t>(p[22]) << 8) | static_cast<uint32_t>(p[23]);
    port_bucket = (dst_port < 1024u) ? 0u : ((dst_port < 8192u) ? 1u : 2u);
  }

  return protocol * 131u + ttl_bucket * 17u + (private_ip ? 5u : 11u) + port_bucket * 23u;
}

}  // namespace

uint64_t classify_packets_checksum(
    const std::vector<uint8_t>& packets,
    std::size_t packet_count,
    int iters) {
  const uint8_t* const data = packets.data();
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = kFnvOffset;
    const uint8_t* p = data;
    for (std::size_t i = 0; i < packet_count; ++i, p += kPacketSize) {
      h ^= static_cast<uint64_t>(classify_one(p));
      h *= kFnvPrime;
    }
    hash = h;
  }

  return hash;
}
