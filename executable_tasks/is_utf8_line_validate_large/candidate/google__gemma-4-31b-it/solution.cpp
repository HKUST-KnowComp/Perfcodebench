#include "interface.h"

#include <cstddef>
#include <string>

namespace {

inline bool is_valid_utf8_segment(const unsigned char* p, const unsigned char* end) {
    while (p < end) {
        const unsigned char c = *p++;
        if (c <= 0x7F) continue;
        if (c >= 0xC2 && c <= 0xDF) {
            if (p >= end || (p[0] & 0xC0) != 0x80) return false;
            p++;
            continue;
        }
        if (c >= 0xE0 && c <= 0xEF) {
            if (end - p < 2) return false;
            const unsigned char c2 = p[0];
            const unsigned char c3 = p[1];
            if ((c3 & 0xC0) != 0x80) return false;
            if (c == 0xE0) {
                if (c2 < 0xA0 || c2 > 0xBF) return false;
            } else if (c == 0xED) {
                if (c2 < 0x80 || c2 > 0x9F) return false;
            } else {
                if ((c2 & 0xC0) != 0x80) return false;
            }
            p += 2;
            continue;
        }
        if (c >= 0xF0 && c <= 0xF4) {
            if (end - p < 3) return false;
            const unsigned char c2 = p[0];
            const unsigned char c3 = p[1];
            const unsigned char c4 = p[2];
            if ((c3 & 0xC0) != 0x80 || (c4 & 0xC0) != 0x80) return false;
            if (c == 0xF0) {
                if (c2 < 0x90 || c2 > 0xBF) return false;
            } else if (c == 0xF4) {
                if (c2 < 0x80 || c2 > 0x8F) return false;
            } else {
                if ((c2 & 0xC0) != 0x80) return false;
            }
            p += 3;
            continue;
        }
        return false;
    }
    return true;
}

} // namespace

std::size_t count_valid_lines(const std::string& input) {
    if (input.empty()) return 0;
    
    std::size_t count = 0;
    const unsigned char* p = reinterpret_cast<const unsigned char*>(input.data());
    const unsigned char* const end = p + input.size();
    
    while (p < end) {
        const unsigned char* line_start = p;
        while (p < end && *p != '\n') {
            p++;
        }
        
        if (is_valid_utf8_segment(line_start, p)) {
            count++;
        }
        
        if (p < end && *p == '\n') {
            p++;
        }
    }
    
    // Handle trailing newline case: if the string ends with '\n', 
    // the loop above processes the last segment before the final '\n'.
    // If the string does NOT end with '\n', the last segment is processed.
    // However, if the string ends with '\n', there is technically an empty line at the end.
    // The baseline logic: while (start < input.size()) { ... start = end + 1; }
    // If input is "abc\n", start=0, end=3, line="abc", start=4. Loop ends. Count=1.
    // If input is "abc\n\n", start=0, end=3, line="abc", start=4; start=4, end=4, line="", start=5. Loop ends. Count=2.
    // My loop: "abc\n" -> line_start=0, p=3, valid, count=1, p=4. Loop ends.
    // "abc\n\n" -> line_start=0, p=3, valid, count=1, p=4; line_start=4, p=4, valid, count=2, p=5. Loop ends.
    // This matches baseline behavior.
    
    return count;
}