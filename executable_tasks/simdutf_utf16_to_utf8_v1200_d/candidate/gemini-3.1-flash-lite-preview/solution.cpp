#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <cstdint>

uint64_t transcode_checksum(const std::u16string& input) {
    simdutf::implementation* impl = simdutf::get_available_implementations().detect_best_supported();
    
    // UTF-16 to UTF-8 can expand up to 3x (or 4x for surrogate pairs, but 3x is safe for 16-bit input)
    size_t max_len = impl->utf8_length_from_utf16(input.size(), reinterpret_cast<const char16_t*>(input.data()));
    std::vector<char> buffer(max_len);
    
    size_t actual_len = impl->convert_utf16_to_utf8(reinterpret_cast<const char16_t*>(input.data()), input.size(), buffer.data());
    
    // FNV-1a checksum
    uint64_t hash = 1469598103934665603ULL;
    const uint8_t* data = reinterpret_cast<const uint8_t*>(buffer.data());
    for (size_t i = 0; i < actual_len; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= 1099511628211ULL;
    }
    
    return hash;
}