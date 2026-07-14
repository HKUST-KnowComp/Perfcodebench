#include "interface.h"
#include <vector>
#include <array>

namespace {
static const std::array<int8_t, 256> base64_table = []() {
    std::array<int8_t, 256> table;
    table.fill(-1);
    for (int i = 0; i < 26; ++i) table['A' + i] = i;
    for (int i = 0; i < 26; ++i) table['a' + i] = 26 + i;
    for (int i = 0; i < 10; ++i) table['0' + i] = 52 + i;
    table['+'] = 62;
    table['/'] = 63;
    table['='] = -2;
    return table;
}();

inline uint64_t update_hash(uint64_t hash, uint8_t byte) {
    return (hash ^ static_cast<uint64_t>(byte)) * 1099511628211ULL;
}
} // namespace

uint64_t decode_checksum(const std::string& input) {
    uint64_t hash = 1469598103934665603ULL;
    const size_t n = input.size();
    for (size_t i = 0; i < n; i += 4) {
        int c0 = base64_table[static_cast<uint8_t>(input[i])];
        int c1 = base64_table[static_cast<uint8_t>(input[i + 1])];
        int c2 = base64_table[static_cast<uint8_t>(input[i + 2])];
        int c3 = base64_table[static_cast<uint8_t>(input[i + 3])];

        uint32_t triple = (static_cast<uint32_t>(c0) << 18) |
                          (static_cast<uint32_t>(c1) << 12) |
                          (static_cast<uint32_t>(c2 > 0 ? c2 : 0) << 6) |
                          (static_cast<uint32_t>(c3 > 0 ? c3 : 0));

        hash = update_hash(hash, static_cast<uint8_t>((triple >> 16) & 0xFF));
        if (c2 != -2) {
            hash = update_hash(hash, static_cast<uint8_t>((triple >> 8) & 0xFF));
        }
        if (c3 != -2) {
            hash = update_hash(hash, static_cast<uint8_t>(triple & 0xFF));
        }
    }
    return hash;
}