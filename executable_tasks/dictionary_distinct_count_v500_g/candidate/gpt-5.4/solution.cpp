#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  uint64_t count = 0;

  if (iters <= 0) return 0;
  if (codes.empty() || dictionary.empty()) return 0;

  const std::size_t dict_size = dictionary.size();

  // Fast path: codes are uint16_t, so when the dictionary fits in 65536 entries
  // every valid code maps to a unique dictionary entry. Distinct decoded strings
  // are then exactly the distinct dictionary strings referenced by the codes.
  // We avoid materializing decoded strings and avoid hashing strings entirely.
  if (dict_size <= 65536u) {
    std::vector<uint8_t> code_seen(dict_size, 0);
    std::vector<uint16_t> touched_codes;
    touched_codes.reserve(codes.size() < dict_size ? codes.size() : dict_size);

    std::vector<uint8_t> dict_mark(dict_size, 0);
    std::vector<std::size_t> touched_dict;
    touched_dict.reserve(codes.size() < dict_size ? codes.size() : dict_size);

    for (int iter = 0; iter < iters; ++iter) {
      touched_codes.clear();
      for (uint16_t code : codes) {
        const std::size_t idx = static_cast<std::size_t>(code);
        if (!code_seen[idx]) {
          code_seen[idx] = 1;
          touched_codes.push_back(code);
        }
      }

      touched_dict.clear();
      for (uint16_t code : touched_codes) {
        const std::size_t idx = static_cast<std::size_t>(code);
        const std::string& s = dictionary[idx];

        // Compare only against already-touched unique-code entries. This keeps
        // work proportional to the number of unique referenced codes and avoids
        // expensive hash tables / string copies.
        bool found = false;
        for (std::size_t existing_idx : touched_dict) {
          if (dictionary[existing_idx] == s) {
            found = true;
            break;
          }
        }
        if (!found) {
          dict_mark[idx] = 1;
          touched_dict.push_back(idx);
        }
      }

      count = static_cast<uint64_t>(touched_dict.size());

      for (uint16_t code : touched_codes) {
        code_seen[static_cast<std::size_t>(code)] = 0;
      }
      for (std::size_t idx : touched_dict) {
        dict_mark[idx] = 0;
      }
    }

    return count;
  }

  // Fallback for oversized dictionaries: still avoid decoding/materialization.
  // Since codes are uint16_t, only indices representable by uint16_t can appear.
  const std::size_t reachable = 65536u;
  const std::size_t mark_size = dict_size < reachable ? dict_size : reachable;

  std::vector<uint8_t> code_seen(mark_size, 0);
  std::vector<uint16_t> touched_codes;
  touched_codes.reserve(codes.size() < mark_size ? codes.size() : mark_size);

  for (int iter = 0; iter < iters; ++iter) {
    touched_codes.clear();
    for (uint16_t code : codes) {
      const std::size_t idx = static_cast<std::size_t>(code);
      if (idx < mark_size && !code_seen[idx]) {
        code_seen[idx] = 1;
        touched_codes.push_back(code);
      }
    }

    std::size_t distinct = 0;
    for (std::size_t i = 0; i < touched_codes.size(); ++i) {
      const std::string& s = dictionary[static_cast<std::size_t>(touched_codes[i])];
      bool found = false;
      for (std::size_t j = 0; j < i; ++j) {
        if (dictionary[static_cast<std::size_t>(touched_codes[j])] == s) {
          found = true;
          break;
        }
      }
      if (!found) ++distinct;
    }

    count = static_cast<uint64_t>(distinct);

    for (uint16_t code : touched_codes) {
      code_seen[static_cast<std::size_t>(code)] = 0;
    }
  }

  return count;
}
