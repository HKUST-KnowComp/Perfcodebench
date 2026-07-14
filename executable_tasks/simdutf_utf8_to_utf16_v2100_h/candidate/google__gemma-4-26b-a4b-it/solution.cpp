#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <cstdint>

namespace {

// Optimized FNV-1a checksum for char16_t sequence
inline uint64_t checksum_utf16(const char16_t* data, size_t len) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t transcode_checksum(const std::string& input) {
    if (input.empty()) {
        return checksum_utf16(nullptr, 0);
    }

    // Pre-allocate buffer for UTF-16. 
    // In the worst case (all 1-byte UTF-8), size is input.size().
    // In the worst case for UTF-16 (all 4-byte UTF-8), size is input.size() * 2 (surrogates).
    // However, simdutf handles the allocation/sizing efficiently.
    // To avoid multiple allocations, we use a vector and resize it.
    std::vector<char16_t> out(input.size()); 
    
    size_t utf16_len = simdutf::convert_utf8_to_utf16(
        input.data(), 
        input.size(), 
        out.data()
    );

    return checksum_utf16(out.data(), utf16_len);
}
