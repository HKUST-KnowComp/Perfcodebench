#include "interface.h"
#include <string>
#include <vector>

namespace {

uint64_t fnv1a_init() {
  return 1469598103934665603ULL;
}

uint64_t fnv1a_update(uint64_t hash, char c) {
  hash ^= static_cast<uint64_t>(static_cast<unsigned char>(c));
  hash *= 1099511628211ULL;
  return hash;
}

void append_int_and_hash(std::string& out, uint64_t& hash, uint64_t value) {
  char buf[20];
  char* p = buf + sizeof(buf);
  *--p = '\0';
  do {
    *--p = '0' + (value % 10);
    value /= 10;
  } while (value != 0);
  while (*p) {
    out.push_back(*p);
    hash = fnv1a_update(hash, *p);
    ++p;
  }
}

void append_string_and_hash(std::string& out, uint64_t& hash, const std::string& s) {
  for (char c : s) {
    out.push_back(c);
    hash = fnv1a_update(hash, c);
  }
}

void append_char_and_hash(std::string& out, uint64_t& hash, char c) {
  out.push_back(c);
  hash = fnv1a_update(hash, c);
}

}  // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
  uint64_t hash = 0;
  std::string out;
  const size_t n = ids.size();
  size_t total_message_len = 0;
  for (const auto& m : messages) total_message_len += m.size();
  out.reserve(total_message_len + n * 30);
  for (int iter = 0; iter < iters; ++iter) {
    out.clear();
    hash = fnv1a_init();
    for (size_t i = 0; i < n; ++i) {
      append_int_and_hash(out, hash, timestamps[i]);
      append_char_and_hash(out, hash, '|');
      append_int_and_hash(out, hash, ids[i]);
      append_char_and_hash(out, hash, '|');
      append_int_and_hash(out, hash, levels[i]);
      append_char_and_hash(out, hash, '|');
      append_string_and_hash(out, hash, messages[i]);
      append_char_and_hash(out, hash, '\n');
    }
  }
  return hash;
}
