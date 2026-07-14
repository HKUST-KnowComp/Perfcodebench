#include "interface.h"
#include <is_utf8.h>
#include <cstring>

std::size_t count_valid_lines(const std::string& input) {
    const unsigned char* data = reinterpret_cast<const unsigned char*>(input.data());
    std::size_t size = input.size();
    std::size_t count = 0;
    const unsigned char* ptr = data;
    const unsigned char* const end_ptr = data + size;
    
    while (ptr < end_ptr) {
        const unsigned char* nl = static_cast<const unsigned char*>(std::memchr(ptr, '\n', end_ptr - ptr));
        if (!nl) nl = end_ptr;
        std::size_t line_len = nl - ptr;
        if (is_utf8(ptr, line_len)) {
            ++count;
        }
        ptr = nl + 1;
    }
    return count;
}
