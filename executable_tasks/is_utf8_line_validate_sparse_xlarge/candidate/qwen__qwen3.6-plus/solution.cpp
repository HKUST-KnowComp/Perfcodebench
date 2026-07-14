#include "interface.h"
#include "is_utf8.h"
#include <cstring>

std::size_t count_valid_lines(const std::string& input) {
    std::size_t count = 0;
    const char* p = input.data();
    const char* const end = p + input.size();
    while (p < end) {
        const char* nl = static_cast<const char*>(std::memchr(p, '\n', end - p));
        std::size_t len = nl ? static_cast<std::size_t>(nl - p) : static_cast<std::size_t>(end - p);
        if (is_utf8(p, len)) {
            ++count;
        }
        p = nl ? nl + 1 : end;
    }
    return count;
}