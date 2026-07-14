#include "interface.h"
#include <simdutf.h>
#include <memory>

uint64_t transcode_checksum(const std::string& input) {
    size_t len = input.size();
    if (len == 0) {
        return 1469598103934665603ULL;
    }
    
    // The number of UTF-16 code units is guaranteed to be <= the number of UTF-8 bytes.
    // Using new char16_t[] avoids the zero-initialization overhead of std::vector.
    std::unique_ptr<char16_t[]> out(new char16_t[len]);
    
    // Transcode using SIMD acceleration, assuming valid UTF-8 as per task contract.
    size_t actual_length = simdutf::convert_valid_utf8_to_utf16le(input.data(), len, out.get());
    
    // Compute FNV-1a checksum over the resulting UTF-16 code units.
    uint64_t hash = 1469598103934665603ULL;
    const char16_t* data = out.get();
    const char16_t* end = data + actual_length;
    
    for (; data != end; ++data) {
        hash ^= static_cast<uint64_t>(*data);
        hash *= 1099511628211ULL;
    }
    
    return hash;
}
