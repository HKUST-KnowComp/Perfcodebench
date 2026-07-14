#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <memory>

uint64_t transcode_checksum(const std::string& input) {
    if (input.empty()) {
        return 1469598103934665603ULL;
    }

    const char* src = input.data();
    size_t len = input.size();

    // Calculate the exact size needed for the UTF-16 output
    size_t utf16_len = simdutf::utf8_to_utf16_size(src, len);
    
    // Use a buffer to hold the transcoded UTF-16 data
    // For very large strings, we use a vector; for smaller ones, we could use stack, 
    // but vector is safe and efficient enough given the SIMD speedup.
    std::vector<char16_t> buffer(utf16_len);
    
    // Perform the SIMD-accelerated conversion
    simdutf::convert_utf8_to_utf16(src, len, buffer.data());

    // Compute the FNV-1a style checksum over the UTF-16 sequence
    uint64_t hash = 1469598103934665603ULL;
    const uint16_t* data = reinterpret_cast<const uint16_t*>(buffer.data());
    for (size_t i = 0; i < utf16_len; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= 1099511628211ULL;
    }

    return hash;
}