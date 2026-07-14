#include "interface.h"
#include <vector>
#include <cstring>

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
    size_t total_size = 0;
    for (const auto& f : frames) {
        total_size += 2 + f.size();
    }

    std::vector<uint8_t> out(total_size);
    uint64_t final_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint8_t* ptr = out.data();
        uint64_t hash = 1469598103934665603ULL;

        for (const auto& frame : frames) {
            uint16_t len = static_cast<uint16_t>(frame.size());
            uint8_t len_bytes[2] = {static_cast<uint8_t>(len), static_cast<uint8_t>(len >> 8)};
            
            for (int i = 0; i < 2; ++i) {
                hash ^= static_cast<uint64_t>(len_bytes[i]);
                hash *= 1099511628211ULL;
            }
            std::memcpy(ptr, len_bytes, 2);
            ptr += 2;

            const uint8_t* data = reinterpret_cast<const uint8_t*>(frame.data());
            for (size_t i = 0; i < frame.size(); ++i) {
                hash ^= static_cast<uint64_t>(data[i]);
                hash *= 1099511628211ULL;
            }
            std::memcpy(ptr, data, frame.size());
            ptr += frame.size();
        }
        final_hash = hash;
    }
    return final_hash;
}