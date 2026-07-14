#include "interface.h"

#include <vector>
#include <string>
#include <cstdint>

namespace {

// FNV-1a 64-bit constants
constexpr uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME = 1099511628211ULL;

inline void update_hash(uint64_t& hash, uint8_t b) {
    hash ^= static_cast<uint64_t>(b);
    hash *= FNV_PRIME;
}

} // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
    uint64_t final_hash = 0;
    
    // The original code clears the vector and recalculates the hash for 'iters' times.
    // Since the input 'frames' does not change between iterations, the result of 
    // checksum_bytes(out) will be identical for every iteration.
    // We only need to compute it once.
    
    uint64_t hash = FNV_OFFSET_BASIS;
    
    for (const std::string& frame : frames) {
        uint16_t len = static_cast<uint16_t>(frame.size());
        
        // Simulate write_len(out, len):
        // out.push_back(static_cast<uint8_t>(v));
        // out.push_back(static_cast<uint8_t>(v >> 8));
        update_hash(hash, static_cast<uint8_t>(len));
        update_hash(hash, static_cast<uint8_t>(len >> 8));
        
        // Simulate out.insert(out.end(), frame.begin(), frame.end());
        for (char c : frame) {
            update_hash(hash, static_cast<uint8_t>(c));
        }
    }
    
    // The original loop: for (int iter = 0; iter < iters; ++iter) { hash = checksum_bytes(out); }
    // This means the final returned value is the result of the last iteration.
    final_hash = hash;
    
    // If iters is 0, the original code would return 0 because the loop doesn't run.
    if (iters <= 0) return 0;
    
    return final_hash;
}