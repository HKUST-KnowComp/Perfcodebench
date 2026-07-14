#include "interface.h"

#include "lz4.h"

#include <chrono>
#include <cstddef>
#include <iostream>
#include <random>
#include <string>
#include <vector>

namespace {

constexpr std::size_t kChunkSize = 1 << 16;

std::string make_input(std::size_t size) {
  std::mt19937 rng(424242);
  std::uniform_int_distribution<int> dist(0, 15);
  std::string s;
  s.resize(size);
  for (std::size_t i = 0; i < size; ++i) {
    s[i] = static_cast<char>('a' + dist(rng));
  }
  return s;
}

bool roundtrip_ok(const std::string& input, const std::vector<std::string>& chunks) {
  std::size_t offset = 0;
  for (const auto& chunk : chunks) {
    const std::size_t take = std::min(kChunkSize, input.size() - offset);
    std::string restored;
    restored.resize(take);
    const int decoded = LZ4_decompress_safe(
        chunk.data(), restored.data(), static_cast<int>(chunk.size()), static_cast<int>(restored.size()));
    if (decoded != static_cast<int>(take) || restored != input.substr(offset, take)) {
      return false;
    }
    offset += take;
  }
  return offset == input.size();
}

}  // namespace

int main() {
  const std::string input = make_input(1 << 20);
  std::vector<std::string> chunks;

  const auto start = std::chrono::steady_clock::now();
  const int total_size = compress_chunks(input, chunks);
  const auto end = std::chrono::steady_clock::now();

  const bool ok = total_size >= 0 && roundtrip_ok(input, chunks);
  const long long elapsed_ns =
      std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

  std::cout << std::boolalpha;
  std::cout << "{"
            << "\"ok\":" << ok << ","
            << "\"elapsed_ns\":" << elapsed_ns << ","
            << "\"compressed_size\":" << total_size << "}\n";
  return ok ? 0 : 1;
}
