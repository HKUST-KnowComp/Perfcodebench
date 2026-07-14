#include "interface.h"

#include "lz4.h"

#include <string>
#include <vector>

namespace {
constexpr int kChunkSize = 1 << 16;
}

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
  const std::size_t input_size = input.size();
  const std::size_t chunk_count = (input_size + static_cast<std::size_t>(kChunkSize) - 1) /
                                  static_cast<std::size_t>(kChunkSize);

  chunks.clear();
  chunks.resize(chunk_count);

  if (input_size == 0) {
    return 0;
  }

  const char* src = input.data();
  const int max_dst = LZ4_compressBound(kChunkSize);
  int total = 0;

  LZ4_stream_t stream;
  LZ4_initStream(&stream, sizeof(stream));

  std::size_t offset = 0;
  for (std::size_t i = 0; i < chunk_count; ++i) {
    const int take = static_cast<int>((input_size - offset) >= static_cast<std::size_t>(kChunkSize)
                                          ? kChunkSize
                                          : (input_size - offset));

    std::string& compressed = chunks[i];
    compressed.resize(static_cast<std::size_t>(max_dst));

    const int n = LZ4_compress_fast_continue(
        &stream,
        src + offset,
        compressed.data(),
        take,
        max_dst,
        1);
    if (n <= 0) {
      chunks.clear();
      return -1;
    }

    compressed.resize(static_cast<std::size_t>(n));
    total += n;
    offset += static_cast<std::size_t>(take);
  }

  return total;
}
