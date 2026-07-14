#include "interface.h"
#include <cstring>
#include <string>

namespace {

static inline bool is_valid_utf8_fast(const char* s, size_t len) {
    const unsigned char* p = reinterpret_cast<const unsigned char*>(s);
    const unsigned char* end = p + len;
    while (p < end) {
        unsigned char c = *p++;
        if (c < 0x80) continue;
        if (c < 0xC2 || c > 0xF4) return false;
        
        if (p >= end) return false;
        unsigned char c2 = *p++;
        if ((c2 & 0xC0) != 0x80) return false;
        
        if (c < 0xE0) continue;
        
        if (p >= end) return false;
        unsigned char c3 = *p++;
        if ((c3 & 0xC0) != 0x80) return false;
        
        if (c < 0xF0) {
            if (c == 0xE0 && c2 < 0xA0) return false;
            if (c == 0xED && c2 > 0x9F) return false;
            continue;
        }
        
        if (p >= end) return false;
        unsigned char c4 = *p++;
        if ((c4 & 0xC0) != 0x80) return false;
        if (c == 0xF0 && c2 < 0x90) return false;
        if (c == 0xF4 && c2 > 0x8F) return false;
    }
    return true;
}

} // namespace

std::size_t count_valid_lines(const std::string& input) {
    std::size_t count = 0;
    const char* data = input.data();
    const char* end = data + input.size();
    const char* line_start = data;
    
    while (line_start < end) {
        const char* nl = static_cast<const char*>(std::memchr(line_start, '\n', end - line_start));
        const char* line_end = nl ? nl : end;
        
        if (is_valid_utf8_fast(line_start, line_end - line_start)) {
            ++count;
        }
        
        line_start = line_end + 1;
    }
    
    return count;
}