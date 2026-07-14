#include "interface.h"
#include <simdutf.h>
#include <cstdint>
#include <memory>

uint64_t transcode_checksum(const std::u16string& input) {
    size_t utf8_size = simdutf::utf8_length_from_utf16le(input.data(), input.size());
    std::unique_ptr<char[]> buffer(new char[utf8_size]);
    char* out = buffer.get();
    simdutf::convert_utf16le_to_utf8(input.data(), input.size(), out);
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < utf8_size; ++i) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(out[i]));
        hash *= 1099511628211ULL;
    }
    return hash;
}