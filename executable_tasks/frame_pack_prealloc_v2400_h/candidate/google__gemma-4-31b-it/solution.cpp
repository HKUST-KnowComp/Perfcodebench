#include "interface.h"
#include <vector>
#include <string>

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
    uint64_t final_hash = 0;
    
    // Pre-calculate the sequence of lengths and pointers to avoid repeated string size calls
    struct FrameInfo {
        uint16_t len;
        const char* data;
    };
    
    std::vector<FrameInfo> info;
    info.reserve(frames.size());
    for (const auto& frame : frames) {
        info.push_back({static_cast<uint16_t>(frame.size()), frame.data()});
    }

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t hash = 1469598103934665603ULL;
        const uint64_t multiplier = 1099511628211ULL;

        for (const auto& f : info) {
            // Length prefix (little-endian)
            uint8_t len_low = static_cast<uint8_t>(f.len);
            uint8_t len_high = static_cast<uint8_t>(f.len >> 8);

            hash ^= static_cast<uint64_t>(len_low);
            hash *= multiplier;
            hash ^= static_cast<uint64_t>(len_high);
            hash *= multiplier;

            // Frame content
            const uint8_t* ptr = reinterpret_cast<const uint8_t*>(f.data);
            for (uint16_t i = 0; i < f.len; ++i) {
                hash ^= static_cast<uint64_t>(ptr[i]);
                hash *= multiplier;
            }
        }
        final_hash = hash;
    }
    
    return final_hash;
}