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

    // simdutf::convert_utf8_to_utf32 is highly optimized using SIMD instructions.
    // We use a vector to manage the lifetime of the output buffer.
    // The size of the output is at most input.size() (for ASCII) and at least input.size()/4.
    // Pre-allocating based on input size is a safe upper bound for memory usage.
    std::vector<char32_t> output;
    try {
        // simdutf provides a high-performance conversion utility.
        // We use the version that returns a vector or we can use the raw pointer version.
        // To minimize overhead, we use the API that works with the input string's data.
        output = simdutf::convert_utf8_to_utf32(input.data(), input.size());
    } catch (...) {
        // In case of invalid UTF-8, though the task implies valid input.
        return 0;
    }

    return checksum_utf32_buffer(output.data(), output.size());
}
