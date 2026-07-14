#include "interface.h"
#include "simdutf.h"
#include <cstddef>
#include <string>
#include <vector>

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
    std::size_t count = 0;
    for (const auto& s : inputs) {
        count += simdutf::validate_utf8(s.data(), s.size()) ? 1U : 0U;
    }
    return count;
}