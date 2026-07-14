#include "interface.h"

#include <cstdint>
#include <string>

uint64_t newline_split_checksum(const std::string& text, int iters) {
    uint64_t hash = 0;
    const char* data = text.data();
    size_t n = text.size();
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t h = 1469598103934665603ULL;
        size_t start = 0;
        for (size_t i = 0; i <= n; ++i) {
            if (i == n || data[i] == '\n') {
                size_t len = i - start;
                unsigned char fc = (len > 0) ? static_cast<unsigned char>(data[start]) : 0;
                h ^= len;
                h *= 1099511628211ULL;
                h ^= fc;
                h *= 1099511628211ULL;
                start = i + 1;
            }
        }
        hash = h;
    }
    return hash;
}