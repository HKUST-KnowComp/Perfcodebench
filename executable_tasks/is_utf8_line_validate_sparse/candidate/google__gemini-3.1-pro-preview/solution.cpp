#include "interface.h"
#include "is_utf8.h"
#include <cstring>

/**
 * Performance Engineer's Optimization:
 * 1. Replaced manual scalar UTF-8 validation with the provided `is_utf8` library function,
 *    which typically leverages SIMD (SSE/AVX/NEON) for high-throughput validation.
 * 2. Replaced manual character-by-character newline scanning with `std::memchr`,
 *    which is highly optimized (often using SIMD) to find the next delimiter.
 * 3. The workload is 'sparse-invalid', meaning most lines are valid. This justifies
 *    the overhead of calling a robust validator per line.
 */

std::size_t count_valid_lines(const std::string& input) {
    if (input.empty()) {
        return 0;
    }

    std::size_t count = 0;
    const char* data = input.data();
    const std::size_t total_size = input.size();
    const char* const end = data + total_size;
    const char* current = data;

    while (current < end) {
        // Find the next newline character efficiently
        const char* next_newline = static_cast<const char*>(std::memchr(current, '\n', static_cast<std::size_t>(end - current)));
        
        std::size_t line_len;
        const char* next_start;

        if (next_newline) {
            line_len = static_cast<std::size_t>(next_newline - current);
            next_start = next_newline + 1;
        } else {
            line_len = static_cast<std::size_t>(end - current);
            next_start = end;
        }

        // Use the optimized is_utf8 function from the provided header
        if (is_utf8(current, line_len)) {
            ++count;
        }

        current = next_start;
    }

    return count;
}