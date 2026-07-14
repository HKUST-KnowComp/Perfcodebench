#include "interface.h"
#include "simdutf.h"

std::size_t count_ascii_strings(const std::vector<std::string>& inputs) {
    std::size_t cnt = 0;
    for (const auto& s : inputs) {
        cnt += simdutf::validate_ascii(s.data(), s.size());
    }
    return cnt;
}