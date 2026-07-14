#include "interface.h"

#include <cstdint>
#include <cstring>
#include <vector>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  const size_t dict_size = dictionary.size();
  const size_t n_codes   = codes.size();

  // 1. Pre-compute 64-bit hash and pointer for every dictionary string.
  struct Entry { uint64_t hash; const char* data; size_t len; };
  std::vector<Entry> entries(dict_size);
  for (size_t i = 0; i < dict_size; ++i) {
    const std::string& s = dictionary[i];
    const size_t len = s.size();
    uint64_t h = 0xcbf29ce484222325ULL; // FNV-1a offset
    for (size_t j = 0; j < len; ++j) {
      h ^= static_cast<unsigned char>(s[j]);
      h *= 0x100000001b3ULL;
    }
    entries[i].hash = h;
    entries[i].data = s.data();
    entries[i].len  = len;
  }

  // 2. Small closed-addressing hash set (linear probing inside 64-byte cache line).
  //    We store hashes only; on collision we compare the actual string bytes via pointer+len.
  struct Slot { uint64_t hash; uint32_t idx; };
  alignas(64) Slot table[16]; // 16*8=128 bytes, fits two cache lines, power-of-two
  constexpr size_t mask = 15;

  uint64_t count = 0;
  for (int iter = 0; iter < iters; ++iter) {
    // clear table
    for (size_t i = 0; i <= mask; ++i) table[i].idx = 0xFFFFFFFFU;
    uint32_t next_idx = 0;

    for (size_t k = 0; k < n_codes; ++k) {
      const uint16_t code = codes[k];
      const Entry& e = entries[code];
      const uint64_t h = e.hash;
      size_t pos = h & mask;
      while (true) {
        if (table[pos].idx == 0xFFFFFFFFU) { // empty
          table[pos].hash = h;
          table[pos].idx  = next_idx++;
          break;
        }
        if (table[pos].hash == h) { // possible hit
          const Entry& prev = entries[table[pos].idx];
          if (prev.len == e.len && std::memcmp(prev.data, e.data, e.len) == 0)
            break; // truly equal
        }
        pos = (pos + 1) & mask; // linear probe
      }
    }
    count = next_idx;
  }
  return count;
}