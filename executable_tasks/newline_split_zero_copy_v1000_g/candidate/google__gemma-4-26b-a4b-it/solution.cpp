#include "interface.h"
#include <string>
#include <vector>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

} // namespace

uint64_t newline_split_checksum(const std::string& text, int iters) {
    const char* data = text.data();
    const size_t n = text.size();
    uint64_t final_hash = 0;

    // Pre-calculate line metadata to avoid repeated scanning if iters > 1
    // However, to strictly follow the baseline's logic where it re-scans and re-calculates,
    // we optimize the scanning process itself.
    
    // We use a vector of pairs (length, first_char) to avoid string copies.
    struct LineInfo {
        size_t len;
        uint8_t first_char;
    };
    std::vector<LineInfo> line_metadata;
    line_metadata.reserve(n / 10); // Heuristic allocation

    for (int iter = 0; iter < iters; ++iter) {
        line_metadata.clear();
        size_t start = 0;
        while (start < n) {
            size_t end = 0;
            // Use std::string::find which is highly optimized (often SIMD)
            size_t found = text.find('\n', start);
            if (found == std::string::npos) {
                end = n;
            } else {
                end = found;
            }

            size_t len = end - start;
            uint8_t first = (len > 0) ? static_cast<uint8_t>(data[start]) : 0;
            line_metadata.push_back({len, first});
            
            if (found == std::string::npos) break;
            start = found + 1;
        }

        // The baseline resets hash to the FNV offset basis every iteration
        uint64_t hash = 1469598103934665603ULL;
        for (const auto& info : line_metadata) {
            hash = mix(hash, info.len);
            hash = mix(hash, static_cast<uint64_t>(info.first_char));
        }
        final_hash = hash;
    }

    return final_hash;
}