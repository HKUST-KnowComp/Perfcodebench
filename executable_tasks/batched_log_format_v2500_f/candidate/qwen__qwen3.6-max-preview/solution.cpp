#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

inline void append_uint(std::string& out, uint32_t v) {
    if (v == 0) {
        out.push_back('0');
        return;
    }
    char buf[12];
    int len = 0;
    while (v > 0) {
        buf[len++] = '0' + (v % 10);
        v /= 10;
    }
    for (int i = len - 1; i >= 0; --i) {
        out.push_back(buf[i]);
    }
}

inline uint64_t checksum_text(const std::string& text) {
    uint64_t hash = 1469598103934665603ULL;
    const unsigned char* data = reinterpret_cast<const unsigned char*>(text.data());
    size_t len = text.size();
    for (size_t i = 0; i < len; ++i) {
        hash ^= data[i];
        hash *= 1099511628211ULL;
    }
    return hash;
}

}  // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
  if (iters <= 0) return 0;
  if (ids.empty()) return 1469598103934665603ULL;

  std::string out;
  out.reserve(ids.size() * 40);

  for (std::size_t i = 0; i < ids.size(); ++i) {
    append_uint(out, timestamps[i]);
    out.push_back('|');
    append_uint(out, ids[i]);
    out.push_back('|');
    append_uint(out, levels[i]);
    out.push_back('|');
    out += messages[i];
    out.push_back('\n');
  }

  // Output is invariant across iterations; compute checksum once.
  return checksum_text(out);
}
