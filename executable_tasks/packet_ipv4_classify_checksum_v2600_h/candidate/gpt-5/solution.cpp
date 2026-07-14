#include "interface.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace {
constexpr std::size_t kPacketSize = 64;
constexpr uint64_t kFNVOffsetBasis = 1469598103934665603ULL;
constexpr uint64_t kFNVPrime = 1099511628211ULL;
}

uint64_t classify_packets_checksum(
    const std::vector<uint8_t>& packets,
    std::size_t packet_count,
    int iters) {
  uint64_t hash = 0;
  const uint8_t* data = packets.data();

  for (int iter = 0; iter < iters; ++iter) {
    hash = kFNVOffsetBasis;
    const uint8_t* p = data;

    for (std::size_t i = 0; i < packet_count; ++i, p += kPacketSize) {
      // IPv4 header fields
      const uint8_t ttl = p[8];
      const uint8_t protocol = p[9];
      const uint8_t src0 = p[12];
      const uint8_t src1 = p[13];

      // Big-endian 16-bit destination port at bytes 22-23
      const uint16_t dst_port = static_cast<uint16_t>((static_cast<uint16_t>(p[22]) << 8) | p[23]);

      // TTL bucket: upper two bits of TTL
      const uint32_t ttl_bucket = static_cast<uint32_t>(ttl >> 6);

      // Protocol bucket: TCP (6) or UDP (17)
      const uint32_t is_tcpudp = static_cast<uint32_t>((protocol == 6u) | (protocol == 17u));

      // Port bucket for TCP/UDP: 0 (<1024), 1 ([1024,8192)), 2 (>=8192)
      const uint32_t port_bucket_tcpudp = static_cast<uint32_t>((dst_port >= 1024u)) + static_cast<uint32_t>((dst_port >= 8192u));
      // Final port bucket: tcp/udp specific or 3 for others
      const uint32_t port_bucket = port_bucket_tcpudp * is_tcpudp + (3u * (1u - is_tcpudp));

      // Private IP check for source address (branchless)
      const uint32_t is10 = static_cast<uint32_t>(src0 == 10u);
      const uint32_t is172 = static_cast<uint32_t>(src0 == 172u);
      const uint32_t is192 = static_cast<uint32_t>(src0 == 192u);
      const uint32_t in16_32 = static_cast<uint32_t>((src1 >= 16u) & (src1 < 32u));
      const uint32_t is168 = static_cast<uint32_t>(src1 == 168u);
      const uint32_t is_private = (is10 | (is172 & in16_32) | (is192 & is168));

      // Source class contribution: 5 for private, 11 otherwise
      const uint32_t src_contrib = 11u - 6u * is_private;

      // Key composition
      const uint32_t key = static_cast<uint32_t>(protocol) * 131u + ttl_bucket * 17u + src_contrib + port_bucket * 23u;

      // FNV-1a update
      hash ^= static_cast<uint64_t>(key);
      hash *= kFNVPrime;
    }
  }

  return hash;
}
