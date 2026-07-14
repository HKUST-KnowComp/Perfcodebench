#include "interface.h"
#include "simdutf.h"
#include <memory>

uint64_t transcode_checksum(const std::u16string& input) {
    thread_local std::unique_ptr<char[]> buffer;
    thread_local size_t buffer_size = 0;
    
    // A UTF-16 to UTF-8 conversion yields at most 3 bytes per char16_t.
    size_t max_len = input.size() * 3;
    if (buffer_size < max_len) {
        buffer.reset(new char[max_len]);
        buffer_size = max_len;
    }
    
    // Transcode using SIMD-accelerated simdutf library
    size_t utf8_len = simdutf::convert_utf16le_to_utf8(
        reinterpret_cast<const char16_t*>(input.data()), 
        input.size(), 
        buffer.get()
    );
    
    // Compute FNV-1a checksum over the resulting UTF-8 bytes
    uint64_t hash = 1469598103934665603ULL;
    const unsigned char* data = reinterpret_cast<const unsigned char*>(buffer.get());
    for (size_t i = 0; i < utf8_len; ++i) {
        hash ^= data[i];
        hash *= 1099511628211ULL;
    }
    
    return hash;
}
