#include "interface.h"
#include <simdutf.h>
#include <string>

uint64_t transcode_checksum(const std::u16string& input) {
    if (input.empty()) {
        return 1469598103934665603ULL;
    }

    // Max expansion is 3 bytes per char16_t for BMP characters.
    // Surrogate pairs (2 char16_t) expand to 4 bytes, so 2 bytes per char16_t.
    // Thus, input.size() * 3 is strictly sufficient.
    std::string out(input.size() * 3, '\0');
    
    size_t len = simdutf::utf16_to_utf8(input.data(), input.size(), out.data());

    uint64_t hash = 1469598103934665603ULL;
    const uint64_t FNV_PRIME = 1099511628211ULL;
    const unsigned char* data = reinterpret_cast<const unsigned char*>(out.data());
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= FNV_PRIME;
    }
    return hash;
}
