#include "interface.h"

#include <string>
#include <vector>

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
  size_t total_size = 0;
  for (size_t i = 0; i < ids.size(); ++i) {
    total_size += std::to_string(timestamps[i]).size() +
                  1 + // |
                  std::to_string(ids[i]).size() +
                  1 + // |
                  std::to_string(levels[i]).size() +
                  1 + // |
                  messages[i].size() +
                  1; // \n
  }

  std::string out;
  out.reserve(total_size);

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    out.clear();
    hash = 1469598103934665603ULL;
    for (size_t i = 0; i < ids.size(); ++i) {
      std::string ts = std::to_string(timestamps[i]);
      for (char c : ts) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(c));
        hash *= 1099511628211ULL;
      }
      out += ts;

      hash ^= static_cast<uint64_t>(static_cast<unsigned char>('|'));
      hash *= 1099511628211ULL;
      out += '|';

      std::string id_str = std::to_string(ids[i]);
      for (char c : id_str) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(c));
        hash *= 1099511628211ULL;
      }
      out += id_str;

      hash ^= static_cast<uint64_t>(static_cast<unsigned char>('|'));
      hash *= 1099511628211ULL;
      out += '|';

      std::string level_str = std::to_string(levels[i]);
      for (char c : level_str) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(c));
        hash *= 1099511628211ULL;
      }
      out += level_str;

      hash ^= static_cast<uint64_t>(static_cast<unsigned char>('|'));
      hash *= 1099511628211ULL;
      out += '|';

      for (char c : messages[i]) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(c));
        hash *= 1099511628211ULL;
      }
      out += messages[i];

      hash ^= static_cast<uint64_t>(static_cast<unsigned char>('\n'));
      hash *= 1099511628211ULL;
      out += '\n';
    }
  }
  return hash;
}