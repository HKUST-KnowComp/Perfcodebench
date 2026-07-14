#include "interface.h"

#include <cstddef>
#include <cstdint>

namespace {
constexpr std::size_t kPacketSize = 64;
}  // namespace

uint64_t classify_packets_checksum(
    const std::vector<uint8_t>& packets,
    std::size_t packet_count,
    int iters) {
  const uint8_t* __restrict__ data = packets.data();
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    std::size_t i = 0;
    const std::size_t unroll_limit = packet_count & ~std::size_t(3);
    
    for (; i < unroll_limit; i += 4) {
      const uint8_t* p0 = data + i * kPacketSize;
      const uint8_t* p1 = p0 + kPacketSize;
      const uint8_t* p2 = p1 + kPacketSize;
      const uint8_t* p3 = p2 + kPacketSize;
      
      // Packet 0
      {
        const uint8_t ttl = p0[8];
        const uint8_t protocol = p0[9];
        const uint8_t src0 = p0[12];
        const uint8_t src1 = p0[13];
        const uint16_t dst_port = (static_cast<uint16_t>(p0[22]) << 8) | p0[23];
        
        const uint32_t ttl_bucket = static_cast<uint32_t>(ttl >> 6);
        const bool is_private = (src0 == 10u) |
                                ((src0 == 172u) & (src1 >= 16u) & (src1 < 32u)) |
                                ((src0 == 192u) & (src1 == 168u));
        const uint32_t port_bucket =
            __builtin_expect((protocol == 6u) | (protocol == 17u), 1)
                ? ((dst_port < 1024u) ? 0u : ((dst_port < 8192u) ? 1u : 2u))
                : 3u;
        const uint32_t key =
            static_cast<uint32_t>(protocol) * 131u + ttl_bucket * 17u +
            (is_private ? 5u : 11u) + port_bucket * 23u;
        hash ^= static_cast<uint64_t>(key);
        hash *= 1099511628211ULL;
      }
      
      // Packet 1
      {
        const uint8_t ttl = p1[8];
        const uint8_t protocol = p1[9];
        const uint8_t src0 = p1[12];
        const uint8_t src1 = p1[13];
        const uint16_t dst_port = (static_cast<uint16_t>(p1[22]) << 8) | p1[23];
        
        const uint32_t ttl_bucket = static_cast<uint32_t>(ttl >> 6);
        const bool is_private = (src0 == 10u) |
                                ((src0 == 172u) & (src1 >= 16u) & (src1 < 32u)) |
                                ((src0 == 192u) & (src1 == 168u));
        const uint32_t port_bucket =
            __builtin_expect((protocol == 6u) | (protocol == 17u), 1)
                ? ((dst_port < 1024u) ? 0u : ((dst_port < 8192u) ? 1u : 2u))
                : 3u;
        const uint32_t key =
            static_cast<uint32_t>(protocol) * 131u + ttl_bucket * 17u +
            (is_private ? 5u : 11u) + port_bucket * 23u;
        hash ^= static_cast<uint64_t>(key);
        hash *= 1099511628211ULL;
      }
      
      // Packet 2
      {
        const uint8_t ttl = p2[8];
        const uint8_t protocol = p2[9];
        const uint8_t src0 = p2[12];
        const uint8_t src1 = p2[13];
        const uint16_t dst_port = (static_cast<uint16_t>(p2[22]) << 8) | p2[23];
        
        const uint32_t ttl_bucket = static_cast<uint32_t>(ttl >> 6);
        const bool is_private = (src0 == 10u) |
                                ((src0 == 172u) & (src1 >= 16u) & (src1 < 32u)) |
                                ((src0 == 192u) & (src1 == 168u));
        const uint32_t port_bucket =
            __builtin_expect((protocol == 6u) | (protocol == 17u), 1)
                ? ((dst_port < 1024u) ? 0u : ((dst_port < 8192u) ? 1u : 2u))
                : 3u;
        const uint32_t key =
            static_cast<uint32_t>(protocol) * 131u + ttl_bucket * 17u +
            (is_private ? 5u : 11u) + port_bucket * 23u;
        hash ^= static_cast<uint64_t>(key);
        hash *= 1099511628211ULL;
      }
      
      // Packet 3
      {
        const uint8_t ttl = p3[8];
        const uint8_t protocol = p3[9];
        const uint8_t src0 = p3[12];
        const uint8_t src1 = p3[13];
        const uint16_t dst_port = (static_cast<uint16_t>(p3[22]) << 8) | p3[23];
        
        const uint32_t ttl_bucket = static_cast<uint32_t>(ttl >> 6);
        const bool is_private = (src0 == 10u) |
                                ((src0 == 172u) & (src1 >= 16u) & (src1 < 32u)) |
                                ((src0 == 192u) & (src1 == 168u));
        const uint32_t port_bucket =
            __builtin_expect((protocol == 6u) | (protocol == 17u), 1)
                ? ((dst_port < 1024u) ? 0u : ((dst_port < 8192u) ? 1u : 2u))
                : 3u;
        const uint32_t key =
            static_cast<uint32_t>(protocol) * 131u + ttl_bucket * 17u +
            (is_private ? 5u : 11u) + port_bucket * 23u;
        hash ^= static_cast<uint64_t>(key);
        hash *= 1099511628211ULL;
      }
    }
    
    // Handle remaining packets
    for (; i < packet_count; ++i) {
      const uint8_t* p = data + i * kPacketSize;
      const uint8_t ttl = p[8];
      const uint8_t protocol = p[9];
      const uint8_t src0 = p[12];
      const uint8_t src1 = p[13];
      const uint16_t dst_port = (static_cast<uint16_t>(p[22]) << 8) | p[23];
      
      const uint32_t ttl_bucket = static_cast<uint32_t>(ttl >> 6);
      const bool is_private = (src0 == 10u) |
                              ((src0 == 172u) & (src1 >= 16u) & (src1 < 32u)) |
                              ((src0 == 192u) & (src1 == 168u));
      const uint32_t port_bucket =
          __builtin_expect((protocol == 6u) | (protocol == 17u), 1)
              ? ((dst_port < 1024u) ? 0u : ((dst_port < 8192u) ? 1u : 2u))
              : 3u;
      const uint32_t key =
          static_cast<uint32_t>(protocol) * 131u + ttl_bucket * 17u +
          (is_private ? 5u : 11u) + port_bucket * 23u;
      hash ^= static_cast<uint64_t>(key);
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}