#include "interface.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <vector>

uint64_t classify_packets_checksum(
    const std::vector<uint8_t>& packets,
    std::size_t packet_count,
    int iters) {
  // Precompute keys for all packets (classification invariant across iterations)
  std::vector<uint32_t> keys(packet_count);
  const uint8_t* base = packets.data();
  constexpr std::size_t kPacketSize = 64;

  for (std::size_t i = 0; i < packet_count; ++i) {
    const uint8_t* pkt = base + i * kPacketSize;

    // Read fields directly
    uint8_t protocol = pkt[9];
    uint8_t ttl = pkt[8];
    uint32_t ttl_bucket = static_cast<uint32_t>(ttl >> 6);

    // Compute port bucket
    uint16_t dst_port = (static_cast<uint16_t>(pkt[22]) << 8) | pkt[23];
    uint32_t port_bucket;
    if (protocol == 6 || protocol == 17) {
      port_bucket = (dst_port < 1024) ? 0 : ((dst_port < 8192) ? 1 : 2);
    } else {
      port_bucket = 3;
    }

    // Private IP detection (using bytes 12-15, need only first two)
    uint8_t a = pkt[12];
    uint8_t b = pkt[13];
    bool is_private = (a == 10) ||
                     (a == 172 && b >= 16 && b < 32) ||
                     (a == 192 && b == 168);
    uint32_t ip_flag = is_private ? 5u : 11u;

    // Combine
    keys[i] = static_cast<uint32_t>(protocol) * 131u +
              ttl_bucket * 17u +
              ip_flag +
              port_bucket * 23u;
  }

  // Iterative FNV-1a hash
  uint64_t hash = 0;
  constexpr uint64_t kOffsetBasis = 1469598103934665603ULL;
  constexpr uint64_t kPrime = 1099511628211ULL;

  for (int iter = 0; iter < iters; ++iter) {
    hash = kOffsetBasis;
    for (std::size_t i = 0; i < packet_count; ++i) {
      hash ^= keys[i];
      hash *= kPrime;
    }
  }

  return hash;
}
