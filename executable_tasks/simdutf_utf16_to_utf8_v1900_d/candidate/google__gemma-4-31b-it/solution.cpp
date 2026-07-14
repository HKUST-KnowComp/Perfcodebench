#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <string>

namespace {

uint64_t compute_fnv1a(const char* data, size_t len) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint8_t>(data[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t transcode_checksum(const std::u16string& input) {
    if (input.empty()) return 1469598103934665603ULL;

    // UTF-8 can be up to 3 bytes per UTF-16 code unit (for BMP) 
    // or 4 bytes for surrogate pairs (which use 2 units).
    // Max size is 3 * input.size().
    std::vector<char> buffer(input.size() * 3);
    
    // simdutf::utf16_to_utf8 returns the number of bytes written.
    // It expects pointers to the input and output buffers.
    simdutf::utf16_to_utf8(reinterpret_cast<const char16_t*>(input.data()), 
                            reinterpret_cast<char*>(buffer.data()), 
                            input.size(), 
                            buffer.size());

    // The simdutf library provides a way to get the actual written length.
    // However, the standard API for utf16_to_utf8 in simdutf usually returns the length.
    // Let's use the version that returns the size.
    size_t written = simdutf::utf16_to_utf8(reinterpret_cast<const char16_t*>(input.data()), 
                                              reinterpret_cast<char*>(buffer.data()), 
                                              input.size());

    return compute_fnv1a(buffer.data(), written);
}
