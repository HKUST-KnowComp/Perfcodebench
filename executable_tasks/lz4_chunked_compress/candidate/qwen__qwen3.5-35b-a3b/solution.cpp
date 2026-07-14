#pragma once

#include "interface.h"

#include "lz4.h"

#include <algorithm>
#include <vector>

namespace {
constexpr std::size_t kChunkSize = 1 << 16;
}

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
  chunks.clear();
  if (input.empty()) return 0;

  // Pre-allocate chunks vector to avoid reallocations
  std::size_t num_chunks = (input.size() + kChunkSize - 1) / kChunkSize;
  chunks.reserve(num_chunks);

  // Static buffer to avoid repeated allocations across calls
  static std::vector<char> buffer;
  if (buffer.empty()) {
    buffer.resize(LZ4_compressBound(kChunkSize));
  }

  int total = 0;
  for (std::size_t offset = 0; offset < input.size(); offset += kChunkSize) {
    std::size_t take = std::min(kChunkSize, input.size() - offset);
    int n = LZ4_compress_fast(
        input.data() + offset, buffer.data(), static_cast<int>(take), static_cast<int>(buffer.size()), 100);
    if (n <= 0) {
      return -1;
    }
    chunks.emplace_back(buffer.data(), n);
    total += n;
  }
  return total;
}