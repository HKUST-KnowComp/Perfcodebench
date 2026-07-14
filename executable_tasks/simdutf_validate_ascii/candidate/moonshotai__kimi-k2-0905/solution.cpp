#include "interface.h"
#include <simdutf.h>

std::size_t count_ascii_strings(const std::vector<std::string>& inputs) {
    std::size_t count = 0;

    for (const std::string& s : inputs) {
        const char* data = s.data();
        std::size_t len  = s.size();

        // simdutf::validate_ascii returns simdutf::error_code::SUCCESS
        // only if *all* bytes are ASCII.
        if (simdutf::validate_ascii(data, len) == simdutf::error_code::SUCCESS) {
            ++count;
            continue;
        }

        // SIMD routine failed; do a precise scalar scan on the whole string
        // to confirm the first non-ASCII byte.
        bool ascii = true;
        for (std::size_t i = 0; i < len; ++i) {
            if (static_cast<unsigned char>(data[i]) > 0x7F) {
                ascii = false;
                break;
            }
        }
        count += ascii;
    }

    return count;
}