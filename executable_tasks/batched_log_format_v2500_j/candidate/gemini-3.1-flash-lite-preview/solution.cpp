#include "interface.h"
#include <string>
#include <vector>
#include <cstdio>

namespace {

inline uint64_t update_checksum(uint64_t hash, const char* data, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
    
    size_t n = ids.size();
    std::vector<std::string> formatted_lines(n);
    char buffer[128];
    
    for (size_t i = 0; i < n; ++i) {
        int len = std::snprintf(buffer, sizeof(buffer), "%u|%u|%u|%s\n", 
                                timestamps[i], ids[i], levels[i], messages[i].c_str());
        formatted_lines[i].assign(buffer, len);
    }

    uint64_t final_hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t hash = 1469598103934665603ULL;
        for (size_t i = 0; i < n; ++i) {
            hash = update_checksum(hash, formatted_lines[i].data(), formatted_lines[i].size());
        }
        final_hash = hash;
    }
    
    return final_hash;
}