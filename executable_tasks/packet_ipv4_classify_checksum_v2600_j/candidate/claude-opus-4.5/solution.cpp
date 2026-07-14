#include "interface.h"

#include <cstddef>
#include <cstdint>

uint64_t classify_packets_checksum(
    const std::vector<uint8_t>& packets,
    std::size_t packet_count,
    int iters) {
  constexpr std::size_t kPacketSize = 64;
  constexpr uint64_t kFNVPrime = 1099511628211ULL;
  constexpr uint64_t kFNVOffset = 1469598103934665603ULL;
  
  const uint8_t* __restrict__ data = packets.data();
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = kFNVOffset;
    const uint8_t* __restrict__ pkt = data;
    
    std::size_t i = 0;
    const std::size_t unroll_limit = packet_count & ~3ULL;
    
    for (; i < unroll_limit; i += 4) {
      // Packet 0
      {
        const uint8_t ttl0 = pkt[8];
        const uint8_t proto0 = pkt[9];
        const uint8_t src0_0 = pkt[12];
        const uint8_t src0_1 = pkt[13];
        const uint16_t dst_port0 = (static_cast<uint16_t>(pkt[22]) << 8) | pkt[23];
        
        const uint32_t ttl_bucket0 = ttl0 >> 6;
        const bool is_tcp_udp0 = (proto0 == 6u) | (proto0 == 17u);
        const uint32_t port_bucket0 = is_tcp_udp0 ? 
            ((dst_port0 < 1024u) ? 0u : ((dst_port0 < 8192u) ? 1u : 2u)) : 3u;
        const bool is_priv0 = (src0_0 == 10u) | 
            ((src0_0 == 172u) & (src0_1 >= 16u) & (src0_1 < 32u)) |
            ((src0_0 == 192u) & (src0_1 == 168u));
        const uint32_t key0 = static_cast<uint32_t>(proto0) * 131u + 
            ttl_bucket0 * 17u + (is_priv0 ? 5u : 11u) + port_bucket0 * 23u;
        hash ^= key0;
        hash *= kFNVPrime;
      }
      pkt += kPacketSize;
      
      // Packet 1
      {
        const uint8_t ttl1 = pkt[8];
        const uint8_t proto1 = pkt[9];
        const uint8_t src1_0 = pkt[12];
        const uint8_t src1_1 = pkt[13];
        const uint16_t dst_port1 = (static_cast<uint16_t>(pkt[22]) << 8) | pkt[23];
        
        const uint32_t ttl_bucket1 = ttl1 >> 6;
        const bool is_tcp_udp1 = (proto1 == 6u) | (proto1 == 17u);
        const uint32_t port_bucket1 = is_tcp_udp1 ? 
            ((dst_port1 < 1024u) ? 0u : ((dst_port1 < 8192u) ? 1u : 2u)) : 3u;
        const bool is_priv1 = (src1_0 == 10u) | 
            ((src1_0 == 172u) & (src1_1 >= 16u) & (src1_1 < 32u)) |
            ((src1_0 == 192u) & (src1_1 == 168u));
        const uint32_t key1 = static_cast<uint32_t>(proto1) * 131u + 
            ttl_bucket1 * 17u + (is_priv1 ? 5u : 11u) + port_bucket1 * 23u;
        hash ^= key1;
        hash *= kFNVPrime;
      }
      pkt += kPacketSize;
      
      // Packet 2
      {
        const uint8_t ttl2 = pkt[8];
        const uint8_t proto2 = pkt[9];
        const uint8_t src2_0 = pkt[12];
        const uint8_t src2_1 = pkt[13];
        const uint16_t dst_port2 = (static_cast<uint16_t>(pkt[22]) << 8) | pkt[23];
        
        const uint32_t ttl_bucket2 = ttl2 >> 6;
        const bool is_tcp_udp2 = (proto2 == 6u) | (proto2 == 17u);
        const uint32_t port_bucket2 = is_tcp_udp2 ? 
            ((dst_port2 < 1024u) ? 0u : ((dst_port2 < 8192u) ? 1u : 2u)) : 3u;
        const bool is_priv2 = (src2_0 == 10u) | 
            ((src2_0 == 172u) & (src2_1 >= 16u) & (src2_1 < 32u)) |
            ((src2_0 == 192u) & (src2_1 == 168u));
        const uint32_t key2 = static_cast<uint32_t>(proto2) * 131u + 
            ttl_bucket2 * 17u + (is_priv2 ? 5u : 11u) + port_bucket2 * 23u;
        hash ^= key2;
        hash *= kFNVPrime;
      }
      pkt += kPacketSize;
      
      // Packet 3
      {
        const uint8_t ttl3 = pkt[8];
        const uint8_t proto3 = pkt[9];
        const uint8_t src3_0 = pkt[12];
        const uint8_t src3_1 = pkt[13];
        const uint16_t dst_port3 = (static_cast<uint16_t>(pkt[22]) << 8) | pkt[23];
        
        const uint32_t ttl_bucket3 = ttl3 >> 6;
        const bool is_tcp_udp3 = (proto3 == 6u) | (proto3 == 17u);
        const uint32_t port_bucket3 = is_tcp_udp3 ? 
            ((dst_port3 < 1024u) ? 0u : ((dst_port3 < 8192u) ? 1u : 2u)) : 3u;
        const bool is_priv3 = (src3_0 == 10u) | 
            ((src3_0 == 172u) & (src3_1 >= 16u) & (src3_1 < 32u)) |
            ((src3_0 == 192u) & (src3_1 == 168u));
        const uint32_t key3 = static_cast<uint32_t>(proto3) * 131u + 
            ttl_bucket3 * 17u + (is_priv3 ? 5u : 11u) + port_bucket3 * 23u;
        hash ^= key3;
        hash *= kFNVPrime;
      }
      pkt += kPacketSize;
    }
    
    for (; i < packet_count; ++i) {
      const uint8_t ttl = pkt[8];
      const uint8_t proto = pkt[9];
      const uint8_t src_0 = pkt[12];
      const uint8_t src_1 = pkt[13];
      const uint16_t dst_port = (static_cast<uint16_t>(pkt[22]) << 8) | pkt[23];
      
      const uint32_t ttl_bucket = ttl >> 6;
      const bool is_tcp_udp = (proto == 6u) | (proto == 17u);
      const uint32_t port_bucket = is_tcp_udp ? 
          ((dst_port < 1024u) ? 0u : ((dst_port < 8192u) ? 1u : 2u)) : 3u;
      const bool is_priv = (src_0 == 10u) | 
          ((src_0 == 172u) & (src_1 >= 16u) & (src_1 < 32u)) |
          ((src_0 == 192u) & (src_1 == 168u));
      const uint32_t key = static_cast<uint32_t>(proto) * 131u + 
          ttl_bucket * 17u + (is_priv ? 5u : 11u) + port_bucket * 23u;
      hash ^= key;
      hash *= kFNVPrime;
      pkt += kPacketSize;
    }
  }
  return hash;
}