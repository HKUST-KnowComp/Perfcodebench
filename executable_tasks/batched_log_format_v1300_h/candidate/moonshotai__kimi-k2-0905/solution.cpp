#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

inline uint64_t fast_itoa(uint32_t value, char* buffer) {
    char* start = buffer;
    if (value == 0) {
        *buffer++ = '0';
    } else {
        char* end = buffer;
        while (value) {
            *end++ = static_cast<char>('0' + (value % 10));
            value /= 10;
        }
        std::reverse(buffer, end);
        buffer = end;
    }
    return static_cast<uint64_t>(buffer - start);
}

inline uint64_t checksum_inline(const char* data, size_t len, uint64_t hash) {
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
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
  std::string out;
  size_t total = 0;
  for (size_t i = 0; i < ids.size(); ++i) {
      total += std::to_string(timestamps[i]).size() + 1 +
               std::to_string(ids[i]).size() + 1 +
               std::to_string(levels[i]).size() + 1 +
               messages[i].size() + 1;
  }
  out.reserve(total);

  char num_buf[16];
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
      out.clear();
      for (size_t i = 0; i < ids.size(); ++i) {
          uint32_t ts = timestamps[i];
          uint32_t id = ids[i];
          uint16_t lv = levels[i];

          size_t len = fast_itoa(ts, num_buf);
          out.append(num_buf, len);
          out += '|';

          len = fast_itoa(id, num_buf);
          out.append(num_buf, len);
          out += '|';

          len = fast_itoa(lv, num_buf);
          out.append(num_buf, len);
          out += '|';

          out += messages[i];
          out += '\n';
      }
      hash = checksum_inline(out.data(), out.size(), 1469598103934665603ULL);
  }
  return hash;
}