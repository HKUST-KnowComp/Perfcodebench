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
  uint64_t hash = 0;
  const uint8_t* base = packets.data();

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL; // FNV offset basis
    const uint8_t* p = base;

    for (std::size_t i = 0; i < packet_count; ++i) {
      // Offsets within a 64-byte packet:
      // TTL: 8, Protocol: 9, Src IP: 12..15, Dst port: 22..23
      const uint8_t ttl = p[8];
      const uint8_t protocol = p[9];

      const uint8_t s0 = p[12];
      const uint8_t s1 = p[13];

      const uint16_t dst_port = static_cast<uint16_t>((static_cast<uint16_t>(p[22]) << 8) | p[23]);

      const uint32_t ttl_bucket = static_cast<uint32_t>(ttl >> 6);

      const bool is_tcp_udp = (protocol == 6u) | (protocol == 17u);
      const uint32_t bucket_tcpudp = static_cast<uint32_t>((dst_port >= 1024u)) + static_cast<uint32_t>((dst_port >= 8192u));
      const uint32_t port_bucket = is_tcp_udp ? bucket_tcpudp : 3u;

      // Private IP check for source address
      const bool is_private = (s0 == 10u) || ((s0 == 172u) && (s1 >= 16u) && (s1 < 32u)) || ((s0 == 192u) && (s1 == 168u));

      // (is_private ? 5u : 11u) == 11u - (is_private ? 6u : 0u)
      const uint32_t private_add = 11u - (static_cast<uint32_t>(is_private) * 6u);

      const uint32_t key = static_cast<uint32_t>(protocol) * 131u + ttl_bucket * 17u + private_add + port_bucket * 23u;

      hash ^= static_cast<uint64_t>(key);
      hash *= 1099511628211ULL; // FNV prime

      p += kPacketSize;
    }
  }

  return hash;
}
