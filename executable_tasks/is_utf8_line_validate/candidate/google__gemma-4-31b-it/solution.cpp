#include "interface.h"
#include <string>

namespace {

// Optimized UTF-8 validation that works on a raw pointer range
bool is_valid_utf8_range(const unsigned char* p, const unsigned char* end) {
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
            unsigned char c2 = p[0];
            unsigned char c3 = p[1];
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
            unsigned char c2 = p[0];
            unsigned char c3 = p[1];
            unsigned char c4 = p[2];
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
    std::size_t count = 0;
    const unsigned char* p = reinterpret_cast<const unsigned char*>(input.data());
    const unsigned char* const end = p + input.size();

    while (p < end) {
        const unsigned char* line_start = p;
        while (p < end && *p != '\n') {
            p++;
        }
        
        if (is_valid_utf8_range(line_start, p)) {
            count++;
        }
        
        if (p < end && *p == '\n') {
            p++;
        }
    }
    
    // Handle trailing newline case: if the string ends with '\n', 
    // the loop above processes the content before the last '\n'.
    // If the string is empty or ends with '\n', the logic above is correct
    // because the loop terminates. However, if the input is "abc\n", 
    // it counts "abc". If it is "abc\n\n", it counts "abc" and "".
    // The baseline logic: start = end + 1. If input is "\n", start=0, end=0, 
    // substr(0,0) is valid, count=1, start=1. Loop ends. Correct.
    
    // To match baseline exactly for trailing newlines:
    // If the input ends with '\n', the baseline loop runs one more time 
    // because start < input.size() is checked after start = end + 1.
    // Wait, if input = "\n", size=1. start=0. end=0. substr(0,0) valid. count=1. start=1. 
    // Loop ends. 
    // If input = "a\n", size=2. start=0. end=1. substr(0,1) valid. count=1. start=2. 
    // Loop ends.
    // My current loop: input="\n". p=0, end=1. line_start=0, p=0. is_valid(0,0) true. count=1. p=1. Loop ends.
    // input="a\n". p=0, end=2. line_start=0, p=1. is_valid(0,1) true. count=1. p=2. Loop ends.
    // It matches.

    return count;
}