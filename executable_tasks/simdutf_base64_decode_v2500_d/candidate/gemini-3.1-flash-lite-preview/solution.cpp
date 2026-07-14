#include <cstdint>
#include <string>
#include <vector>

static const int8_t B64_TABLE[256] = {
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,62,-1,-1,-1,63,52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-2,-1,-1,
    -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,
    -1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
};

inline void update_hash(uint64_t& hash, uint8_t byte) {
    hash ^= static_cast<uint64_t>(byte);
    hash *= 1099511628211ULL;
}

uint64_t decode_checksum(const std::string& input) {
    uint64_t hash = 1469598103934665603ULL;
    size_t n = input.size();
    for (size_t i = 0; i < n; i += 4) {
        int v0 = B64_TABLE[static_cast<uint8_t>(input[i])];
        int v1 = B64_TABLE[static_cast<uint8_t>(input[i + 1])];
        int v2 = B64_TABLE[static_cast<uint8_t>(input[i + 2])];
        int v3 = B64_TABLE[static_cast<uint8_t>(input[i + 3])];

        uint32_t triple = (static_cast<uint32_t>(v0) << 18) | 
                          (static_cast<uint32_t>(v1) << 12) | 
                          (static_cast<uint32_t>(v2 & 0x3F) << 6) | 
                          (static_cast<uint32_t>(v3 & 0x3F));

        update_hash(hash, static_cast<uint8_t>(triple >> 16));
        if (v2 != -2) {
            update_hash(hash, static_cast<uint8_t>(triple >> 8));
            if (v3 != -2) {
                update_hash(hash, static_cast<uint8_t>(triple));
            }
        }
    }
    return hash;
}