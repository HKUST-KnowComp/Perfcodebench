#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <cstdint>

namespace {

// FNV-1a style checksum as per the original implementation requirements
inline uint64_t checksum_utf32_buffer(const char32_t* data, size_t count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < count; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t transcode_checksum(const std::string& input) {
    if (input.empty()) {
        return 1469598103934665603ULL;
    }

    // simdutf::utf8_to_utf32 requires a destination buffer.
    // The maximum possible number of UTF-32 code points is input.size().
    std::vector<char32_t> out(input.size());
    
    size_t decoded_size = simdutf::utf8_to_utf32(
        input.data(), 
        input.size(), 
        out.data()
    );

    return checksum_utf32_buffer(out.data(), decoded_size);
}
