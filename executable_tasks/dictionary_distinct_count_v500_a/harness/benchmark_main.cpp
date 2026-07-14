#include "interface.h"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

namespace {

std::string make_token(uint32_t id) {
  std::string token(10, 'a');
  token[0] = static_cast<char>('a' + (id % 26u));
  token[1] = static_cast<char>('a' + ((id / 26u) % 26u));
  uint32_t x = id * 1103515245u + 12345u;
  for (int i = 2; i < 10; ++i) {
    x = x * 1664525u + 1013904223u;
    token[static_cast<std::size_t>(i)] = static_cast<char>('a' + ((x >> 27) % 26u));
  }
  return token;
}

std::vector<std::string> make_dictionary(std::size_t n) {
  std::vector<std::string> dictionary;
  dictionary.reserve(n);
  for (std::size_t i = 0; i < n; ++i) {
    dictionary.push_back(make_token(static_cast<uint32_t>(i)));
  }
  return dictionary;
}

std::vector<uint16_t> make_codes(std::size_t n, std::size_t dict_size) {
  std::vector<uint16_t> codes(n);
  uint64_t state = 0x0fedcba987654321ULL;
  for (std::size_t i = 0; i < n; ++i) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    codes[i] = static_cast<uint16_t>(state & static_cast<uint64_t>(dict_size - 1));
  }
  return codes;
}

uint64_t expected_count(const std::vector<uint16_t>& codes, std::size_t dict_size) {
  std::vector<uint8_t> seen(dict_size, 0);
  uint64_t count = 0;
  for (uint16_t code : codes) {
    const std::size_t idx = static_cast<std::size_t>(code);
    if (!seen[idx]) {
      seen[idx] = 1;
      ++count;
    }
  }
  return count;
}

}  // namespace

int main() {
  constexpr std::size_t kDictSize = 1024;
  constexpr std::size_t kRows = 1u << 21;
  constexpr int kIters = 12;

  const std::vector<std::string> dictionary = make_dictionary(kDictSize);
  const std::vector<uint16_t> codes = make_codes(kRows, kDictSize);
  const uint64_t expected = expected_count(codes, kDictSize);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual = dictionary_distinct(dictionary, codes, kIters);
  const auto end = std::chrono::steady_clock::now();

  const bool ok = actual == expected;
  const long long elapsed_ns =
      std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

  std::cout << std::boolalpha;
  std::cout << "{"
            << "\"ok\":" << ok << ","
            << "\"elapsed_ns\":" << elapsed_ns << ","
            << "\"expected_count\":" << expected << ","
            << "\"actual_count\":" << actual << "}\n";
  return ok ? 0 : 1;
}
