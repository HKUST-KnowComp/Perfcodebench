#include "interface.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

constexpr std::size_t kPacketSize = 64;
constexpr uint64_t kFnvOffset = 1469598103934665603ULL;
constexpr uint64_t kFnvPrime = 1099511628211ULL;

inline uint16_t load_be_u16(const uint8_t* p) {
  return static_cast<uint16_t>((static_cast<uint16_t>(p[0]) << 8) |
                               static_cast<uint16_t>(p[1]));
}

inline uint32_t private_ip_term(const uint8_t* ip) {
  const uint8_t b0 = ip[0];
  const uint8_t b1 = ip[1];
  const bool is_private =
      (b0 == 10u) |
      ((b0 == 172u) & (b1 >= 16u) & (b1 < 32u)) |
      ((b0 == 192u) & (b1 == 168u));
  return is_private ? 5u : 11u;
}

inline uint32_t port_bucket(uint8_t protocol, uint16_t dst_port) {
  if (protocol == 6u || protocol == 17u) {
    if (dst_port < 1024u) return 0u;
    if (dst_port < 8192u) return 1u;
    return 2u;
  }
  return 3u;
}

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
    const uint8_t* const end = data + packet_count * kPacketSize;

    for (; p != end; p += kPacketSize) {
      const uint8_t ttl = p[8];
      const uint8_t protocol = p[9];
      const uint8_t* const src = p + 12;
      const uint16_t dst_port = load_be_u16(p + 22);

      const uint32_t key =
          static_cast<uint32_t>(protocol) * 131u +
          static_cast<uint32_t>(ttl >> 6) * 17u +
          private_ip_term(src) +
          port_bucket(protocol, dst_port) * 23u;

      hash ^= static_cast<uint64_t>(key);
      hash *= kFnvPrime;
    }
  }

  return hash;
}
