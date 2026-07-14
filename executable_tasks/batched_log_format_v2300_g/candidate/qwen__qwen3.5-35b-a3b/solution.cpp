#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

int to_string(uint32_t num, char* buffer) {
    if (num == 0) {
        buffer[0] = '0';
        buffer[1] = '\0';
        return 1;
    }
    int i = 0;
    while (num > 0) {
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }
    for (int j = 0; j < i/2; ++j) {
        char temp = buffer[j];
        buffer[j] = buffer[i - 1 - j];
        buffer[i - 1 - j] = temp;
    }
    buffer[i] = '\0';
    return i;
}

}  // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (std::size_t i = 0; i < ids.size(); ++i) {
            char buf[12];
            int len = to_string(timestamps[i], buf);
            for (int j = 0; j < len; ++j) {
                hash ^= static_cast<uint64_t>(buf[j]);
                hash *= 1099511628211ULL;
            }
            hash ^= '|';
            hash *= 1099511628211ULL;
            len = to_string(ids[i], buf);
            for (int j = 0; j < len; ++j) {
                hash ^= static_cast<uint64_t>(buf[j]);
                hash *= 1099511628211ULL;
            }
            hash ^= '|';
            hash *= 1099511628211ULL;
            len = to_string(levels[i], buf);
            for (int j = 0; j < len; ++j) {
                hash ^= static_cast<uint64_t>(buf[j]);
                hash *= 1099511628211ULL;
            }
            hash ^= '|';
            hash *= 1099511628211ULL;
            for (char c : messages[i]) {
                hash ^= static_cast<uint64_t>(c);
                hash *= 1099511628211ULL;
            }
            hash ^= '\n';
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}