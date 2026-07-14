#include "interface.h"

#include <cstdint>
#include <cstddef>
#include <vector>

namespace {
constexpr std::size_t kPacketSize = 64;
constexpr uint64_t kInitialHash = 1469598103934665603ULL;
constexpr uint64_t kMul = 1099511628211ULL;

inline bool is_private_ip(const uint8_t* ip) {
    return ip[0] == 10 ||
           (ip[0] == 172 && ip[1] >= 16 && ip[1] < 32) ||
           (ip[0] == 192 && ip[1] == 168);
}
} // namespace

uint64_t classify_packets_checksum(
    const std::vector<uint8_t>& packets,
    std::size_t packet_count,
    int iters) {
    if (iters <= 0) return 0;
    uint64_t hash = kInitialHash;
    const uint8_t* data = packets.data();
    for (std::size_t i = 0; i < packet_count; ++i) {
        const uint8_t* p = data + i * kPacketSize;
        uint8_t protocol = p[9];
        uint8_t ttl = p[8];
        uint32_t ttl_bucket = ttl >> 6;
        uint16_t dst_port = static_cast<uint16_t>((p[22] << 8) | p[23]);
        uint32_t port_bucket;
        if (protocol == 6 || protocol == 17) {
            if (dst_port < 1024) port_bucket = 0;
            else if (dst_port < 8192) port_bucket = 1;
            else port_bucket = 2;
        } else {
            port_bucket = 3;
        }
        uint32_t key = static_cast<uint32_t>(protocol) * 131 + ttl_bucket * 17 +
                       (is_private_ip(p + 12) ? 5 : 11) + port_bucket * 23;
        hash ^= static_cast<uint64_t>(key);
        hash *= kMul;
    }
    return hash;
}