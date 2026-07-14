#include "interface.h"

#include <string>
#include <vector>

namespace {

void append_and_hash(std::string& out, uint64_t& hash, const std::string& s) {
    for (char c : s) {
        out.push_back(c);
        unsigned char uc = static_cast<unsigned char>(c);
        hash ^= static_cast<uint64_t>(uc);
        hash *= 1099511628211ULL;
    }
}

}  // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
  std::string out;
  uint64_t hash = 1469598103934665603ULL;
  for (int iter = 0; iter < iters; ++iter) {
    out.clear();
    for (std::size_t i = 0; i < ids.size(); ++i) {
      append_and_hash(out, hash, std::to_string(timestamps[i]));
      out.push_back('|');
      hash ^= static_cast<uint64_t>(static_cast<unsigned char>('|'));
      hash *= 1099511628211ULL;
      append_and_hash(out, hash, std::to_string(ids[i]));
      out.push_back('|');
      hash ^= static_cast<uint64_t>(static_cast<unsigned char>('|'));
      hash *= 1099511628211ULL;
      append_and_hash(out, hash, std::to_string(levels[i]));
      out.push_back('|');
      hash ^= static_cast<uint64_t>(static_cast<unsigned char>('|'));
      hash *= 1099511628211ULL;
      append_and_hash(out, hash, messages[i]);
      out.push_back('\n');
      hash ^= static_cast<uint64_t>(static_cast<unsigned char>('\n'));
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}