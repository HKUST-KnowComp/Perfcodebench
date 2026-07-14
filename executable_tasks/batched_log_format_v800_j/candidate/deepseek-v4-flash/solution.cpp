#include "interface.h"
#include <charconv>
#include <cstdint>
#include <string>
#include <vector>

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
  std::size_t n = ids.size();
  uint64_t final_hash = 0;
  char buf[16];
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < n; ++i) {
      auto [p, _] = std::to_chars(buf, buf + sizeof(buf), timestamps[i]);
      for (char* c = buf; c != p; ++c) {
        hash ^= static_cast<uint64_t>(*c);
        hash *= 1099511628211ULL;
      }
      hash ^= '|';
      hash *= 1099511628211ULL;

      auto [p2, _2] = std::to_chars(buf, buf + sizeof(buf), ids[i]);
      for (char* c = buf; c != p2; ++c) {
        hash ^= static_cast<uint64_t>(*c);
        hash *= 1099511628211ULL;
      }
      hash ^= '|';
      hash *= 1099511628211ULL;

      auto [p3, _3] = std::to_chars(buf, buf + sizeof(buf), levels[i]);
      for (char* c = buf; c != p3; ++c) {
        hash ^= static_cast<uint64_t>(*c);
        hash *= 1099511628211ULL;
      }
      hash ^= '|';
      hash *= 1099511628211ULL;

      for (unsigned char c : messages[i]) {
        hash ^= static_cast<uint64_t>(c);
        hash *= 1099511628211ULL;
      }
      hash ^= '\n';
      hash *= 1099511628211ULL;
    }
    final_hash = hash;
  }
  return final_hash;
}
