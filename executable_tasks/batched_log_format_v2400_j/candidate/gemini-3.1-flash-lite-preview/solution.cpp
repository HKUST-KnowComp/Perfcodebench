#include "interface.h"
#include <string>
#include <vector>
#include <cstdio>

uint64_t batched_log_checksum(const std::vector<uint32_t>& timestamps, const std::vector<uint32_t>& ids, const std::vector<uint16_t>& levels, const std::vector<std::string>& messages, int iters) {
    size_t n = ids.size();
    std::vector<std::string> formatted_lines(n);
    size_t total_size = 0;
    for (size_t i = 0; i < n; ++i) {
        formatted_lines[i] = std::to_string(timestamps[i]) + "|" + std::to_string(ids[i]) + "|" + std::to_string(levels[i]) + "|" + messages[i] + "\n";
        total_size += formatted_lines[i].size();
    }

    std::string buffer;
    buffer.reserve(total_size);
    for (const auto& line : formatted_lines) {
        buffer.append(line);
    }

    uint64_t hash = 1469598103934665603ULL;
    for (unsigned char c : buffer) {
        hash ^= static_cast<uint64_t>(c);
        hash *= 1099511628211ULL;
    }

    return hash;
}