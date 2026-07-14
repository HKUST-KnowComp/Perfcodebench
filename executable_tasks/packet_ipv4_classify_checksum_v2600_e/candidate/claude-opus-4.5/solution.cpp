#include "interface.h"

#include <cstddef>
#include <cstdint>

namespace {

constexpr std::size_t kPacketSize = 64;

__attribute__((always_inline)) inline uint32_t classify_single(
    const uint8_t* __restrict__ pkt) noexcept {
  const uint8_t ttl = pkt[8];
  const uint8_t protocol = pkt[9];
  const uint8_t src0 = pkt[12];
  const uint8_t src1 = pkt[13];
  const uint16_t dst_port = static_cast<uint16_t>(
      (static_cast<uint16_t>(pkt[22]) << 8) | pkt[23]);
  
  const uint32_t ttl_bucket = static_cast<uint32_t>(ttl >> 6);
  
  // Branchless private IP check
  const uint32_t is_10 = (src0 == 10u);
  const uint32_t is_172_16 = (src0 == 172u) & (src1 >= 16u) & (src1 < 32u);
  const uint32_t is_192_168 = (src0 == 192u) & (src1 == 168u);
  const uint32_t is_private = is_10 | is_172_16 | is_192_168;
  const uint32_t addr_class = is_private ? 5u : 11u;
  
  // Branchless port bucket
  const uint32_t is_tcp_udp = (protocol == 6u) | (protocol == 17u);
  const uint32_t port_lt_1024 = (dst_port < 1024u);
  const uint32_t port_lt_8192 = (dst_port < 8192u);
  const uint32_t port_bucket_tcp_udp = port_lt_1024 ? 0u : (port_lt_8192 ? 1u : 2u);
  const uint32_t port_bucket = is_tcp_udp ? port_bucket_tcp_udp : 3u;
  
  return static_cast<uint32_t>(protocol) * 131u + ttl_bucket * 17u +
         addr_class + port_bucket * 23u;
}

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
    const std::size_t unroll_limit = packet_count & ~static_cast<std::size_t>(3);
    
    // Unrolled loop - process 4 packets at a time
    for (; i < unroll_limit; i += 4) {
      const uint8_t* __restrict__ pkt0 = data + i * kPacketSize;
      const uint8_t* __restrict__ pkt1 = pkt0 + kPacketSize;
      const uint8_t* __restrict__ pkt2 = pkt1 + kPacketSize;
      const uint8_t* __restrict__ pkt3 = pkt2 + kPacketSize;
      
      // Prefetch next batch
      if (i + 8 < packet_count) {
        __builtin_prefetch(pkt0 + 4 * kPacketSize, 0, 1);
        __builtin_prefetch(pkt0 + 5 * kPacketSize, 0, 1);
      }
      
      const uint32_t key0 = classify_single(pkt0);
      const uint32_t key1 = classify_single(pkt1);
      const uint32_t key2 = classify_single(pkt2);
      const uint32_t key3 = classify_single(pkt3);
      
      hash ^= static_cast<uint64_t>(key0);
      hash *= 1099511628211ULL;
      hash ^= static_cast<uint64_t>(key1);
      hash *= 1099511628211ULL;
      hash ^= static_cast<uint64_t>(key2);
      hash *= 1099511628211ULL;
      hash ^= static_cast<uint64_t>(key3);
      hash *= 1099511628211ULL;
    }
    
    // Handle remaining packets
    for (; i < packet_count; ++i) {
      const uint32_t key = classify_single(data + i * kPacketSize);
      hash ^= static_cast<uint64_t>(key);
      hash *= 1099511628211ULL;
    }
  }
  
  return hash;
}