#include "interface.h"
#include <string>
#include <vector>
#include <simdutf.h>

uint64_t transcode_checksum(const std::u16string& input) {
    if (input.empty()) {
        return 1469598103934665603ULL;
    }

    size_t max_output_size = input.size() * 4;
    std::vector<char> output(max_output_size);

    size_t output_size = simdutf::convert_utf16_to_utf8(
        input.data(), input.size(), output.data()
    );

    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < output_size; ++i) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(output[i]));
        hash *= 1099511628211ULL;
    }

    return hash;
}