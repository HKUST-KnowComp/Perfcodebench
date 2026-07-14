#include "interface.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

constexpr std::size_t kPacketSize = 64;

// Inline helper to check private IP range without struct overhead
inline uint32_t get_src_class_val(const uint8_t* ptr) {
    uint8_t ip0 = ptr[12];
    uint8_t ip1 = ptr[13];
    if (ip0 == 10u) return 5u;
    if (ip0 == 172u && ip1 >= 16u && ip1 < 32u) return 5u;
    if (ip0 == 192u && ip1 == 168u) return 5u;
    return 11u;
}

} // namespace

uint64_t classify_packets_checksum(
    const std::vector<uint8_t>& packets, 
    std::size_t packet_count, 
    int iters) {
    
    const uint8_t* data = packets.data();
    uint64_t final_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t hash = 1469598103934665603ULL;
        
        for (std::size_t i = 0; i < packet_count; ++i) {
            const uint8_t* p = data + (i * kPacketSize);
            
            // IPv4 Header offsets:
            // TTL: offset 8
            // Protocol: offset 9
            // Src IP: offset 12
            // Dst Port: offset 22 (assuming UDP/TCP structure in payload/header context)
            
            const uint8_t protocol = p[9];
            const uint8_t ttl = p[8];
            const uint32_t ttl_bucket = static_cast<uint32_t>(ttl >> 6);
            
            // Extract dst_port (big-endian from the packet data)
            const uint16_t dst_port = static_cast<uint16_t>((static_cast<uint16_t>(p[22]) << 8) | p[23]);
            
            // Port bucket logic
            uint32_t port_bucket;
            if (protocol == 6u || protocol == 17u) {
                if (dst_port < 1024u) port_bucket = 0u;
                else if (dst_port < 8192u) port_bucket = 1u;
                else port_bucket = 2u;
            } else {
                port_bucket = 3u;
            }

            const uint32_t src_class = get_src_class_val(p);
            
            const uint32_t key = static_cast<uint32_t>(protocol) * 131u + 
                                 ttl_bucket * 17u + 
                                 src_class + 
                                 port_bucket * 23u;
            
            hash ^= static_cast<uint64_t>(key);
            hash *= 1099511628211ULL;
        }
        final_hash = hash;
    }
    
    return final_hash;
}