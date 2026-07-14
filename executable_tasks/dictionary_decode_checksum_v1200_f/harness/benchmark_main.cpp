#include "interface.h"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

namespace {

struct Input {
  std::vector<std::string> dict;
  std::vector<uint32_t> codes;
};

Input make_input(std::size_t dict_size, std::size_t rows) {
  Input out;
  out.dict.resize(dict_size);
  uint64_t state = 0x123456789abcdef0ULL;
  for (std::size_t i = 0; i < dict_size; ++i) {
    const std::size_t len = 4 + static_cast<std::size_t>(state % 28ULL);
    out.dict[i].reserve(len);
    for (std::size_t j = 0; j < len; ++j) {
      state = state * 2862933555777941757ULL + 3037000493ULL;
      out.dict[i].push_back(static_cast<char>('a' + (state % 26ULL)));
    }
  }
  out.codes.resize(rows);
  for (std::size_t i = 0; i < rows; ++i) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    out.codes[i] = static_cast<uint32_t>(state % dict_size);
  }
  return out;
}

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

uint64_t expected_checksum(const std::vector<std::string>& dict, const std::vector<uint32_t>& codes) {
  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t code : codes) {
    const std::string& token = dict[static_cast<std::size_t>(code)];
    hash = mix(hash, token.size());
    hash = mix(hash, token.empty() ? 0u : static_cast<unsigned char>(token[0]));
  }
  return hash;
}

}  // namespace

int main() {
  const Input input = make_input(128, 1u << 21);
  constexpr int kIters = 24;
  const uint64_t expected = expected_checksum(input.dict, input.codes);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual = dictionary_decode_checksum(input.dict, input.codes, kIters);
  const auto end = std::chrono::steady_clock::now();

  const bool ok = actual == expected;
  const long long elapsed_ns =
      std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

  std::cout << std::boolalpha;
  std::cout << "{"
            << "\"ok\":" << ok << ","
            << "\"elapsed_ns\":" << elapsed_ns << ","
            << "\"expected_checksum\":" << expected << ","
            << "\"actual_checksum\":" << actual << "}\n";
  return ok ? 0 : 1;
}
