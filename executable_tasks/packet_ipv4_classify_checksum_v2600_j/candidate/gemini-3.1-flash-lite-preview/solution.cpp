#include "interface.h"
#include <cstddef>
#include <cstdint>
#include <vector>

uint64_t classify_packets_checksum(const std::vector<uint8_t>& packets, std::size_t packet_count, int iters) {
    uint64_t hash = 0;
    const uint8_t* data = packets.data();
    
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (std::size_t i = 0; i < packet_count; ++i) {
            const uint8_t* p = data + (i << 6);
            
            const uint8_t ttl = p[8];
            const uint8_t protocol = p[9];
            const uint8_t src0 = p[12];
            const uint8_t src1 = p[13];
            const uint16_t dst_port = (static_cast<uint16_t>(p[22]) << 8) | p[23];
            
            const uint32_t ttl_bucket = ttl >> 6;
            
            bool is_private = (src0 == 10u) || 
                              (src0 == 172u && (src1 >= 16u && src1 < 32u)) || 
                              (src0 == 192u && src1 == 168u);
            
            uint32_t port_bucket = 3u;
            if (protocol == 6u || protocol == 17u) {
                if (dst_port < 1024u) port_bucket = 0u;
                else if (dst_port < 8192u) port_bucket = 1u;
                else port_bucket = 2u;
            }
            
            const uint32_t key = (static_cast<uint32_t>(protocol) * 131u) + 
                                 (ttl_bucket * 17u) + 
                                 (is_private ? 5u : 11u) + 
                                 (port_bucket * 23u);
            
            hash ^= static_cast<uint64_t>(key);
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}