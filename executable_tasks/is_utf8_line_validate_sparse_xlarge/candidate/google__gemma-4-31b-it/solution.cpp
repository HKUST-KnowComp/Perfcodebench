#include "interface.h"
#include <cstdint>
#include <cstring>

std::size_t count_valid_lines(const std::string& input) {
    const unsigned char* p = reinterpret_cast<const unsigned char*>(input.data());
    const unsigned char* end = p + input.size();
    std::size_t valid_count = 0;

    auto validate_utf8 = [](const unsigned char* start, const unsigned char* end_ptr) -> bool {
        const unsigned char* curr = start;
        while (curr < end_ptr) {
            unsigned char c = *curr++;
            if (c <= 0x7F) continue;
            if (c >= 0xC2 && c <= 0xDF) {
                if (curr >= end_ptr || (curr[0] & 0xC0) != 0x80) return false;
                curr++;
            } else if (c >= 0xE0 && c <= 0xEF) {
                if (curr + 1 >= end_ptr) return false;
                unsigned char c2 = curr[0];
                unsigned char c3 = curr[1];
                if ((c3 & 0xC0) != 0x80) return false;
                if (c == 0xE0) {
                    if (c2 < 0xA0 || c2 > 0xBF) return false;
                } else if (c == 0xED) {
                    if (c2 < 0x80 || c2 > 0x9F) return false;
                } else {
                    if ((c2 & 0xC0) != 0x80) return false;
                }
                curr += 2;
            } else if (c >= 0xF0 && c <= 0xF4) {
                if (curr + 2 >= end_ptr) return false;
                unsigned char c2 = curr[0];
                unsigned char c3 = curr[1];
                unsigned char c4 = curr[2];
                if ((c3 & 0xC0) != 0x80 || (c4 & 0xC0) != 0x80) return false;
                if (c == 0xF0) {
                    if (c2 < 0x90 || c2 > 0xBF) return false;
                } else if (c == 0xF4) {
                    if (c2 < 0x80 || c2 > 0x8F) return false;
                } else {
                    if ((c2 & 0xC0) != 0x80) return false;
                }
                curr += 3;
            } else {
                return false;
            }
        }
        return true;
    };

    while (p < end) {
        const unsigned char* line_start = p;
        const unsigned char* line_end = p;

        // Fast scan for newline or non-ASCII
        while (line_end < end) {
            // Process in chunks of 8 bytes if possible
            if (line_end + 8 <= end) {
                uint64_t chunk;
                std::memcpy(&chunk, line_end, 8);
                // Check if any byte has the high bit set or is a newline (0x0A)
                // Mask for high bits: 0x8080808080808080
                // Mask for newline: 0x0A0A0A0A0A0A0A0A
                // We can use a trick: (chunk & 0x80...) != 0 or (chunk & 0x0A...) != 0
                // But simpler: check if any byte is not in [0x20, 0x7F] or is 0x0A
                // Actually, just check for high bit or 0x0A
                if ((chunk & 0x8080808080808080ULL) || (chunk & 0x0A0A0A0A0A0A0A0AULL) || 
                    ((chunk + 0x0101010101010101ULL) & 0x0101010101010101ULL) == 0) {
                    // This is a bit complex, let's use a simpler fast path
                    break;
                }
                line_end += 8;
            } else {
                if (*line_end == '\n' || (*line_end & 0x80)) break;
                line_end++;
            }
        }

        // If we broke because of a chunk, find the exact position
        if (line_end < end) {
            // We need to find the first byte that is either '\n' or has high bit set
            // The chunk logic above was a sketch; let's refine the scan
        }

        // Corrected scan: find the first character that is NOT (ASCII > 0x7F) AND NOT '\n'
        // To keep it simple and fast, we use a basic loop but the compiler optimizes it well
        // For xlarge sparse, the most important thing is finding the newline quickly.
        
        // Reset line_end to p and do a clean scan
        line_end = p;
        while (line_end < end && *line_end != '\n') {
            line_end++;
        }

        if (validate_utf8(line_start, line_end)) {
            valid_count++;
        }

        p = (line_end < end) ? line_end + 1 : end;
    }

    return valid_count;
}
