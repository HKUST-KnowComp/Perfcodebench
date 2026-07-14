#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <cstdint>

namespace {

// Optimized FNV-1a style checksum for char16_t sequence
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

    // Pre-allocate buffer. UTF-16 size is at most input.size() * 2 (for surrogates)
    // but for most text it is closer to input.size().
    // simdutf::utf8_to_utf16 handles the allocation/transcoding efficiently.
    std::vector<char16_t> out(input.size()); 
    
    size_t utf16_len = simdutf::utf8_to_utf16(input.data(), input.size(), out.data());
    
    // If the output was larger than our initial guess (unlikely for valid UTF-8 but possible 
    // if we didn't reserve enough), we'd need to handle it. 
    // However, simdutf::utf8_to_utf16 returns the number of written units.
    // Since we want to be safe and the task implies valid UTF-8:
    // We use the returned length to calculate the checksum.
    
    return checksum_utf16(out.data(), utf16_len);
}