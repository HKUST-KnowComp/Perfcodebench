#include "interface.h"

#include <cstddef>
#include <cstdint>

namespace {

constexpr std::size_t kPacketSize = 64;

inline uint32_t classify_one(const uint8_t* __restrict__ p) {
    const uint8_t ttl = p[8];
    const uint8_t protocol = p[9];
    const uint8_t src0 = p[12];
    const uint8_t src1 = p[13];
    const uint16_t dst_port = static_cast<uint16_t>((static_cast<uint16_t>(p[22]) << 8) | p[23]);

    const uint32_t ttl_bucket = static_cast<uint32_t>(ttl >> 6);

    // Branchless port bucket: 0 if <1024, 1 if <8192, 2 otherwise; 3 if not TCP/UDP
    const bool is_tcp_udp = (protocol == 6u) | (protocol == 17u);
    const uint32_t port_bucket = is_tcp_udp
        ? ((dst_port < 1024u) ? 0u : ((dst_port < 8192u) ? 1u : 2u))
        : 3u;

    // Inline private IP check
    const bool priv = (src0 == 10u) |
                      ((src0 == 172u) & (src1 >= 16u) & (src1 < 32u)) |
                      ((src0 == 192u) & (src1 == 168u));
    const uint32_t addr_class = priv ? 5u : 11u;

    return static_cast<uint32_t>(protocol) * 131u + ttl_bucket * 17u + addr_class + port_bucket * 23u;
}

} // namespace

uint64_t classify_packets_checksum(
    const std::vector<uint8_t>& packets,
    std::size_t packet_count,
    int iters) {

    const uint8_t* __restrict__ base = packets.data();
    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        const uint8_t* p = base;

        std::size_t i = 0;
        const std::size_t unroll_end = packet_count & ~static_cast<std::size_t>(3);

        // Unrolled loop by 4
        for (; i < unroll_end; i += 4) {
            uint32_t k0 = classify_one(p);
            p += kPacketSize;
            uint32_t k1 = classify_one(p);
            p += kPacketSize;
            uint32_t k2 = classify_one(p);
            p += kPacketSize;
            uint32_t k3 = classify_one(p);
            p += kPacketSize;

            hash ^= static_cast<uint64_t>(k0);
            hash *= 1099511628211ULL;
            hash ^= static_cast<uint64_t>(k1);
            hash *= 1099511628211ULL;
            hash ^= static_cast<uint64_t>(k2);
            hash *= 1099511628211ULL;
            hash ^= static_cast<uint64_t>(k3);
            hash *= 1099511628211ULL;
        }

        // Remainder
        for (; i < packet_count; ++i) {
            uint32_t key = classify_one(p);
            p += kPacketSize;
            hash ^= static_cast<uint64_t>(key);
            hash *= 1099511628211ULL;
        }
    }

    return hash;
}
