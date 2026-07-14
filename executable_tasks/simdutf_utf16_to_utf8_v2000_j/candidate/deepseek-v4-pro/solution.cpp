#include "interface.h"

#include <cstddef>
#include <cstdint>
#include <vector>

#include <simdutf.h>

uint64_t transcode_checksum(const std::u16string& input) {
    // Over-allocate to guarantee enough space. Maximum expansion from UTF-16LE
    // to UTF-8 is 3 bytes per code unit (worst-case BMP non-surrogate > 0x7FF).
    std::vector<char> out(input.size() * 4);

    std::size_t written =
        simdutf::convert_utf16le_to_utf8(input.data(), input.size(), out.data());

    // Compute the same hash as the reference implementation.
    uint64_t hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < written; ++i) {
        hash ^=
            static_cast<uint64_t>(static_cast<unsigned char>(out[i]));
        hash *= 1099511628211ULL;
    }
    return hash;
}
