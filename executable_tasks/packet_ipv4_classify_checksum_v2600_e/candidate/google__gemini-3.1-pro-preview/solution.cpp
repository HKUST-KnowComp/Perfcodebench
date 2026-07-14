#include "interface.h"

#include <cstddef>
#include <cstdint>
#include <vector>

/**
 * Performance-optimized IPv4 packet classification.
 * 
 * Optimization strategies:
 * 1. Direct pointer access to avoid std::vector overhead and redundant memcpys.
 * 2. Branch-reduced logic for classification (port buckets and private IP checks).
 * 3. Manual loop unrolling to improve instruction-level parallelism (ILP).
 * 4. Efficient big-endian port extraction.
 */

uint64_t classify_packets_checksum(
    const std::vector<uint8_t>& packets,
    std::size_t packet_count,
    int iters) {
  
  const uint8_t* const base_ptr = packets.data();
  const uint64_t fnv_prime = 1099511628211ULL;
  const uint64_t fnv_offset_basis = 1469598103934665603ULL;
  
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    hash = fnv_offset_basis;
    std::size_t i = 0;

    // Unroll loop to process 4 packets at a time for better ILP
    for (; i + 3 < packet_count; i += 4) {
      for (int j = 0; j < 4; ++j) {
        const uint8_t* p = base_ptr + (i + j) * 64;
        
        const uint32_t protocol = p[9];
        const uint32_t ttl_bucket = p[8] >> 6;
        
        // Private IP check: 10.x.x.x, 172.16-31.x.x, 192.168.x.x
        const uint8_t ip0 = p[12];
        const uint8_t ip1 = p[13];
        bool is_priv = (ip0 == 10) || 
                       (ip0 == 172 && (ip1 >= 16 && ip1 < 32)) || 
                       (ip0 == 192 && ip1 == 168);
        
        uint32_t port_bucket = 3;
        if (protocol == 6 || protocol == 17) {
          // Big-endian port extraction
          const uint32_t dst_port = (static_cast<uint32_t>(p[22]) << 8) | p[23];
          port_bucket = (dst_port >= 1024) + (dst_port >= 8192);
        }

        const uint32_t key = protocol * 131 + 
                             ttl_bucket * 17 + 
                             (is_priv ? 5 : 11) + 
                             port_bucket * 23;
        
        hash ^= static_cast<uint64_t>(key);
        hash *= fnv_prime;
      }
    }

    // Handle remaining packets
    for (; i < packet_count; ++i) {
      const uint8_t* p = base_ptr + i * 64;
      
      const uint32_t protocol = p[9];
      const uint32_t ttl_bucket = p[8] >> 6;
      
      const uint8_t ip0 = p[12];
      const uint8_t ip1 = p[13];
      bool is_priv = (ip0 == 10) || 
                     (ip0 == 172 && (ip1 >= 16 && ip1 < 32)) || 
                     (ip0 == 192 && ip1 == 168);
      
      uint32_t port_bucket = 3;
      if (protocol == 6 || protocol == 17) {
        const uint32_t dst_port = (static_cast<uint32_t>(p[22]) << 8) | p[23];
        port_bucket = (dst_port >= 1024) + (dst_port >= 8192);
      }

      const uint32_t key = protocol * 131 + 
                           ttl_bucket * 17 + 
                           (is_priv ? 5 : 11) + 
                           port_bucket * 23;
      
      hash ^= static_cast<uint64_t>(key);
      hash *= fnv_prime;
    }
  }

  return hash;
}
