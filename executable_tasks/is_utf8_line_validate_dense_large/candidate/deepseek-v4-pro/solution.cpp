#include "interface.h"
#include <cstring>
#include "is_utf8.h"

std::size_t count_valid_lines(const std::string& input) {
    const char* p = input.data();
    const char* const end = p + input.size();
    std::size_t count = 0;
    while (p < end) {
        const char* eol = static_cast<const char*>(std::memchr(p, '\n', static_cast<std::size_t>(end - p)));
        if (!eol) eol = end;
        std::size_t len = static_cast<std::size_t>(eol - p);
        // empty lines are valid UTF-8
        if (is_utf8(reinterpret_cast<const unsigned char*>(p), len)) {
            ++count;
        }
        p = eol + (eol < end ? 1 : 0);
    }
    return count;
}