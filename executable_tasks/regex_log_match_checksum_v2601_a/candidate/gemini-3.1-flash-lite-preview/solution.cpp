#include "interface.h"
#include <cstdint>
#include <string>
#include <vector>

static inline bool is_match(const std::string& s) {
    if (s.length() != 18) return false;
    // Format: ^[A-Z]{2}[0-9]{4}-[a-z]{6}-[0-9A-F]{8}$
    // Indices: 012345678901234567
    if (!(s[0] >= 'A' && s[0] <= 'Z')) return false;
    if (!(s[1] >= 'A' && s[1] <= 'Z')) return false;
    for (int i = 2; i < 6; ++i) if (!(s[i] >= '0' && s[i] <= '9')) return false;
    if (s[6] != '-') return false;
    for (int i = 7; i < 13; ++i) if (!(s[i] >= 'a' && s[i] <= 'z')) return false;
    if (s[13] != '-') return false;
    for (int i = 14; i < 18; ++i) {
        char c = s[i];
        if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F'))) return false;
    }
    return true;
}

uint64_t regex_log_match_checksum(const std::vector<std::string>& records, int iters) {
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (const std::string& s : records) {
            const bool ok = is_match(s);
            const uint64_t v = ok ? static_cast<uint64_t>(s[2] + s[14]) : 0u;
            hash ^= v + 1u;
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}