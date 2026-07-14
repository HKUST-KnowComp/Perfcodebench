#include "interface.h"
#include <cstdint>
#include <string>

namespace {

inline uint64_t mix(uint64_t h, uint64_t v) {
    h ^= v;
    h *= 1099511628211ULL;
    return h;
}

} // namespace

uint64_t newline_split_checksum(const std::string& text, int iters) {
    const char* const data = text.data();
    const size_t n = text.size();
    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        const char* start = data;
        const char* const end = data + n;

        while (start < end) {
            const char* nl = start;
            while (nl < end && *nl != '\n') ++nl;

            const size_t len = static_cast<size_t>(nl - start);
            hash = mix(hash, len);
            hash = mix(hash, len ? static_cast<unsigned char>(*start) : 0u);

            start = nl + 1;
        }
    }
    return hash;
}