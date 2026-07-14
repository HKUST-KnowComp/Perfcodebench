#include "interface.h"

#include <cstddef>
#include <cstdint>

namespace {

constexpr std::size_t kPacketSize = 64;

// Lookup table for first octet: 1 if could be private (10, 172, 192), 0 otherwise
// For 10.x.x.x -> always private
// For 172.x.x.x -> need to check second octet (16-31)
// For 192.x.x.x -> need to check second octet (168)
alignas(64) constexpr uint8_t kFirstOctetPrivate[256] = {
    [10] = 2,   // definitely private
    [172] = 1,  // maybe private (need second octet check)
    [192] = 1,  // maybe private (need second octet check)
};

inline uint32_t classify_private(const uint8_t* src) {
    const uint8_t o0 = src[0];
    const uint8_t o1 = src[1];
    const uint8_t tag = kFirstOctetPrivate[o0];
    if (tag == 2) return 5u;  // 10.x.x.x
    if (tag == 1) {
        if (o0 == 172u && o1 >= 16u && o1 < 32u) return 5u;
        if (o0 == 192u && o1 == 168u) return 5u;
    }
    return 11u;
}

inline uint32_t compute_port_bucket(uint8_t protocol, uint16_t dst_port) {
    if (protocol != 6u && protocol != 17u) return 3u;
    if (dst_port < 1024u) return 0u;
    if (dst_port < 8192u) return 1u;
    return 2u;
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
        const std::size_t count4 = packet_count & ~std::size_t(3);
        
        // Process 4 packets at a time
        for (; i < count4; i += 4) {
            const uint8_t* p0 = data + i * kPacketSize;
            const uint8_t* p1 = p0 + kPacketSize;
            const uint8_t* p2 = p1 + kPacketSize;
            const uint8_t* p3 = p2 + kPacketSize;
            
            // Packet 0
            {
                const uint8_t ttl = p0[8];
                const uint8_t protocol = p0[9];
                const uint8_t* src = p0 + 12;
                const uint16_t dst_port = (static_cast<uint16_t>(p0[22]) << 8) | p0[23];
                
                const uint32_t ttl_bucket = ttl >> 6;
                const uint32_t port_bucket = compute_port_bucket(protocol, dst_port);
                const uint32_t priv = classify_private(src);
                
                const uint32_t key = static_cast<uint32_t>(protocol) * 131u + 
                                     ttl_bucket * 17u + priv + port_bucket * 23u;
                hash ^= static_cast<uint64_t>(key);
                hash *= 1099511628211ULL;
            }
            
            // Packet 1
            {
                const uint8_t ttl = p1[8];
                const uint8_t protocol = p1[9];
                const uint8_t* src = p1 + 12;
                const uint16_t dst_port = (static_cast<uint16_t>(p1[22]) << 8) | p1[23];
                
                const uint32_t ttl_bucket = ttl >> 6;
                const uint32_t port_bucket = compute_port_bucket(protocol, dst_port);
                const uint32_t priv = classify_private(src);
                
                const uint32_t key = static_cast<uint32_t>(protocol) * 131u + 
                                     ttl_bucket * 17u + priv + port_bucket * 23u;
                hash ^= static_cast<uint64_t>(key);
                hash *= 1099511628211ULL;
            }
            
            // Packet 2
            {
                const uint8_t ttl = p2[8];
                const uint8_t protocol = p2[9];
                const uint8_t* src = p2 + 12;
                const uint16_t dst_port = (static_cast<uint16_t>(p2[22]) << 8) | p2[23];
                
                const uint32_t ttl_bucket = ttl >> 6;
                const uint32_t port_bucket = compute_port_bucket(protocol, dst_port);
                const uint32_t priv = classify_private(src);
                
                const uint32_t key = static_cast<uint32_t>(protocol) * 131u + 
                                     ttl_bucket * 17u + priv + port_bucket * 23u;
                hash ^= static_cast<uint64_t>(key);
                hash *= 1099511628211ULL;
            }
            
            // Packet 3
            {
                const uint8_t ttl = p3[8];
                const uint8_t protocol = p3[9];
                const uint8_t* src = p3 + 12;
                const uint16_t dst_port = (static_cast<uint16_t>(p3[22]) << 8) | p3[23];
                
                const uint32_t ttl_bucket = ttl >> 6;
                const uint32_t port_bucket = compute_port_bucket(protocol, dst_port);
                const uint32_t priv = classify_private(src);
                
                const uint32_t key = static_cast<uint32_t>(protocol) * 131u + 
                                     ttl_bucket * 17u + priv + port_bucket * 23u;
                hash ^= static_cast<uint64_t>(key);
                hash *= 1099511628211ULL;
            }
        }
        
        // Handle remaining packets
        for (; i < packet_count; ++i) {
            const uint8_t* p = data + i * kPacketSize;
            
            const uint8_t ttl = p[8];
            const uint8_t protocol = p[9];
            const uint8_t* src = p + 12;
            const uint16_t dst_port = (static_cast<uint16_t>(p[22]) << 8) | p[23];
            
            const uint32_t ttl_bucket = ttl >> 6;
            const uint32_t port_bucket = compute_port_bucket(protocol, dst_port);
            const uint32_t priv = classify_private(src);
            
            const uint32_t key = static_cast<uint32_t>(protocol) * 131u + 
                                 ttl_bucket * 17u + priv + port_bucket * 23u;
            hash ^= static_cast<uint64_t>(key);
            hash *= 1099511628211ULL;
        }
    }
    
    return hash;
}