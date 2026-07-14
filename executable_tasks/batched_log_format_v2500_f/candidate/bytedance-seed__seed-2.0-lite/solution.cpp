#include "interface.h"

#include <string>
#include <vector>
#include <cstdio>

namespace {

uint64_t checksum_text(const std::string& text) {
  uint64_t hash = 1469598103934665603ULL;
  for (unsigned char c : text) {
    hash ^= static_cast<uint64_t>(c);
    hash *= 1099511628211ULL;
  }
  return hash;
}

inline int count_digits(uint32_t x) {
    if (x < 10) return 1;
    if (x < 100) return 2;
    if (x < 1000) return 3;
    if (x < 10000) return 4;
    if (x < 100000) return 5;
    if (x < 1000000) return 6;
    if (x < 10000000) return 7;
    if (x < 100000000) return 8;
    if (x < 1000000000) return 9;
    return 10;
}

} // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
    if (ids.empty()) {
        uint64_t hash = 0;
        const std::string empty;
        for (int iter = 0; iter < iters; ++iter) {
            hash = checksum_text(empty);
        }
        return hash;
    }

    size_t total_size = 0;
    const size_t n = ids.size();
    for (size_t i = 0; i < n; ++i) {
        total_size += count_digits(timestamps[i]) + count_digits(ids[i]) + count_digits(levels[i]) + messages[i].size() + 4;
    }

    std::string out;
    out.reserve(total_size);
    char* ptr = out.data();
    for (size_t i = 0; i < n; ++i) {
        int written = snprintf(ptr, total_size - (ptr - out.data()), "%u|%u|%u|%s\n",
            timestamps[i], ids[i], levels[i], messages[i].c_str());
        ptr += written;
    }
    out.resize(ptr - out.data());

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = checksum_text(out);
    }
    return hash;
}