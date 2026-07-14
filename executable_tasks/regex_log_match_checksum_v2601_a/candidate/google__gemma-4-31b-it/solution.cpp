#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

inline bool matches_pattern(const std::string& s) {
    if (s.length() != 21) return false;

    // [A-Z]{2}
    if (!(s[0] >= 'A' && s[0] <= 'Z') || !(s[1] >= 'A' && s[1] <= 'Z')) return false;
    // [0-9]{4}
    if (!(s[2] >= '0' && s[2] <= '9') || !(s[3] >= '0' && s[3] <= '9') || 
        !(s[4] >= '0' && s[4] <= '9') || !(s[5] >= '0' && s[5] <= '9')) return false;
    // -
    if (s[6] != '-') return false;
    // [a-z]{6}
    if (!(s[7] >= 'a' && s[7] <= 'z') || !(s[8] >= 'a' && s[8] <= 'z') || 
        !(s[9] >= 'a' && s[9] <= 'z') || !(s[10] >= 'a' && s[10] <= 'z') || 
        !(s[11] >= 'a' && s[11] <= 'z') || !(s[12] >= 'a' && s[12] <= 'z')) return false;
    // -
    if (s[13] != '-') return false;
    // [0-9A-F]{8}
    for (int i = 14; i < 21; ++i) {
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
            bool ok = matches_pattern(s);
            uint64_t v = ok ? static_cast<uint64_t>(s[2] + s[14]) : 0u;
            hash ^= v + 1u;
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}