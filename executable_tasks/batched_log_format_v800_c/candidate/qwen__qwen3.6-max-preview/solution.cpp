#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
  if (iters <= 0) return 0;

  uint64_t hash = 1469598103934665603ULL;
  const uint64_t prime = 1099511628211ULL;

  const size_t n = ids.size();
  char num_buf[12];

  for (size_t i = 0; i < n; ++i) {
    // Timestamp
    uint32_t val = timestamps[i];
    if (val == 0) {
      hash ^= static_cast<uint64_t>(static_cast<unsigned char>('0')); hash *= prime;
    } else {
      int len = 0;
      while (val > 0) { num_buf[len++] = static_cast<char>('0' + (val % 10)); val /= 10; }
      for (int j = len - 1; j >= 0; --j) { hash ^= static_cast<uint64_t>(static_cast<unsigned char>(num_buf[j])); hash *= prime; }
    }
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>('|')); hash *= prime;

    // ID
    val = ids[i];
    if (val == 0) {
      hash ^= static_cast<uint64_t>(static_cast<unsigned char>('0')); hash *= prime;
    } else {
      int len = 0;
      while (val > 0) { num_buf[len++] = static_cast<char>('0' + (val % 10)); val /= 10; }
      for (int j = len - 1; j >= 0; --j) { hash ^= static_cast<uint64_t>(static_cast<unsigned char>(num_buf[j])); hash *= prime; }
    }
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>('|')); hash *= prime;

    // Level
    uint32_t lval = levels[i];
    if (lval == 0) {
      hash ^= static_cast<uint64_t>(static_cast<unsigned char>('0')); hash *= prime;
    } else {
      int len = 0;
      while (lval > 0) { num_buf[len++] = static_cast<char>('0' + (lval % 10)); lval /= 10; }
      for (int j = len - 1; j >= 0; --j) { hash ^= static_cast<uint64_t>(static_cast<unsigned char>(num_buf[j])); hash *= prime; }
    }
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>('|')); hash *= prime;

    // Message
    const std::string& msg = messages[i];
    for (char c : msg) {
      hash ^= static_cast<uint64_t>(static_cast<unsigned char>(c));
      hash *= prime;
    }

    // Newline
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>('\n')); hash *= prime;
  }

  return hash;
}