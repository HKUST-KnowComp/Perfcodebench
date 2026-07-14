#include "interface.h"
#include <simdutf.h>
#include <memory>

uint64_t transcode_checksum(const std::u16string& input) {
    if (input.empty()) {
        return 1469598103934665603ULL;
    }
    
    size_t utf8_len = simdutf::utf8_length_from_utf16le(input.data(), input.size());
    
    char buffer[8192];
    char* out = buffer;
    std::unique_ptr<char[]> heap_out;
    if (utf8_len > sizeof(buffer)) {
        heap_out.reset(new char[utf8_len]);
        out = heap_out.get();
    }
    
    simdutf::convert_valid_utf16le_to_utf8(input.data(), input.size(), out);
    
    uint64_t hash = 1469598103934665603ULL;
    const unsigned char* data = reinterpret_cast<const unsigned char*>(out);
    for (size_t i = 0; i < utf8_len; ++i) {
        hash ^= data[i];
        hash *= 1099511628211ULL;
    }
    return hash;
}
