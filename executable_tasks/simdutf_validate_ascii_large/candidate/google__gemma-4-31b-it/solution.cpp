#include "interface.h"
#include "simdutf.h"

std::size_t count_ascii_strings(const std::vector<std::string>& inputs) {
    std::size_t count = 0;
    for (const auto& s : inputs) {
        // simdutf::validate_utf8 returns true if the string is valid UTF-8.
        // However, the task specifically asks for ASCII (bytes 0-127).
        // simdutf provides a specialized function for ASCII validation.
        if (simdutf::validate_ascii(s.data(), s.size())) {
            count++;
        }
    }
    return count;
}