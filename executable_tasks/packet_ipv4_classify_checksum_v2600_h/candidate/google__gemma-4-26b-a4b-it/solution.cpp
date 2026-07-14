#include "interface.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

constexpr std::size_t kPacketSize = 64;

// Optimized private IP check using direct access to avoid struct overhead
inline bool is_private_ip_fast(const uint8_t* src) {
    uint8_t p0 = src[0];
    uint8_t p1 = src[1];
    return (p0 == 10u) || 
           (p0 == 172u && p1 >= 16u && p1 < 32u) || 
           (p0 == 192u && p1 == 168u);
}

} // namespace

uint64_t classify_packets_checksum(
    const std::vector<uint8_t>& packets, 
    std::size_t packet_count, 
    int iters) {
    
    const uint8_t* data = packets.data();
    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (std::size_t i = 0; i < packet_count; ++i) {
            const uint8_t* p = data + (i * kPacketSize);
            
            // IPv4 Header offsets:
            // ttl: offset 8
            // protocol: offset 9
            // src: offset 12
            // dst_port: offset 22 (part of payload/transport)
            
            const uint8_t ttl = p[8];
            const uint8_t protocol = p[9];
            const uint8_t* src_ptr = p + 12;
            
            // dst_port is at offset 22 (2 bytes)
            // We read it as a big-endian 16-bit integer
            const uint16_t dst_port = static_cast<uint16_t>((p[22] << 8) | p[23]);

            const uint32_t ttl_bucket = static_cast<uint32_t>(ttl >> 6);
            
            uint32_t port_bucket;
            if (protocol == 6u || protocol == 17u) {
                if (dst_port < 1024u) {
                    port_bucket = 0u;
                } else if (dst_port < 8192u) {
                    port_bucket = 1u;
                } else {
                    port_bucket = 2u;
                }
            } else {
                port_bucket = 3u;
            }

            const uint32_t ip_val = is_private_ip_fast(src_ptr) ? 5u : 11u;
            
            const uint32_t key = static_cast<uint32_t>(protocol) * 131u + 
                                 ttl_bucket * 17u + 
                                 ip_val + 
                                 port_bucket * 23u;

            hash ^= static_cast<uint64_t>(key);
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}
