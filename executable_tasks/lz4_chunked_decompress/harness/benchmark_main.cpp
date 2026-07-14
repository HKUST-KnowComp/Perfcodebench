#include "interface.h"

#include "lz4.h"

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <random>
#include <string>
#include <vector>

namespace {

constexpr std::size_t kChunkSize = 1 << 12;

std::string make_input(std::size_t size) {
  std::mt19937 rng(13579);
  std::uniform_int_distribution<int> dist(0, 15);
  std::string s;
  s.resize(size);
  for (std::size_t i = 0; i < size; ++i) {
    s[i] = static_cast<char>('a' + dist(rng));
  }
  return s;
}

void make_compressed_chunks(
    const std::string& input,
    std::vector<std::string>& chunks,
    std::vector<int>& sizes) {
  chunks.clear();
  sizes.clear();
  chunks.reserve((input.size() + kChunkSize - 1) / kChunkSize);
  sizes.reserve((input.size() + kChunkSize - 1) / kChunkSize);
  for (std::size_t offset = 0; offset < input.size(); offset += kChunkSize) {
    const std::size_t take = std::min(kChunkSize, input.size() - offset);
    std::string compressed;
    compressed.resize(LZ4_compressBound(static_cast<int>(take)));
    const int n = LZ4_compress_default(
        input.data() + offset, compressed.data(), static_cast<int>(take), static_cast<int>(compressed.size()));
    compressed.resize(static_cast<std::size_t>(n));
    chunks.push_back(std::move(compressed));
    sizes.push_back(static_cast<int>(take));
  }
}

uint64_t expected_checksum(const std::string& input) {
  uint64_t sum = 0;
  for (unsigned char ch : input) {
    sum += static_cast<uint64_t>(ch);
  }
  return sum;
}

}  // namespace

int main() {
  const std::string input = make_input(1 << 22);
  std::vector<std::string> chunks;
  std::vector<int> sizes;
  make_compressed_chunks(input, chunks, sizes);
  const uint64_t expected = expected_checksum(input);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual = decompress_checksum(chunks, sizes);
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
