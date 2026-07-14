#include "interface.h"

#include "lz4.h"

namespace {
constexpr std::size_t kChunkSize = 1 << 12; // 4 KiB
constexpr int kAcceleration = 8;            // higher accel -> faster, still lossless
}

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
  chunks.clear();
  const std::size_t input_size = input.size();
  if (input_size == 0) return 0;

  const char* src = input.data();

  // Reserve space to avoid vector reallocations
  const std::size_t num_chunks = (input_size + kChunkSize - 1) / kChunkSize;
  chunks.reserve(num_chunks);

  // Prepare reusable external state for LZ4 to avoid per-call allocations
  int stateSize = LZ4_sizeofState();
  std::vector<char> stateBuf;
  stateBuf.resize(stateSize > 0 ? static_cast<std::size_t>(stateSize) : 0);
  void* statePtr = stateBuf.empty() ? nullptr : static_cast<void*>(stateBuf.data());

  const int boundStandard = LZ4_compressBound(static_cast<int>(kChunkSize));

  int total = 0;
  for (std::size_t offset = 0; offset < input_size; offset += kChunkSize) {
    const std::size_t remaining = input_size - offset;
    const int isize = static_cast<int>(remaining < kChunkSize ? remaining : kChunkSize);
    const int bound = (isize == static_cast<int>(kChunkSize)) ? boundStandard : LZ4_compressBound(isize);

    // Allocate output buffer once and compress directly into it
    chunks.emplace_back();
    std::string& out = chunks.back();
    out.resize(static_cast<std::size_t>(bound));

    int n;
    if (statePtr) {
      n = LZ4_compress_fast_extState(statePtr, src + offset, &out[0], isize, bound, kAcceleration);
    } else {
      n = LZ4_compress_fast(src + offset, &out[0], isize, bound, kAcceleration);
    }

    if (n <= 0) {
      chunks.pop_back();
      return -1;
    }

    out.resize(static_cast<std::size_t>(n));
    total += n;
  }

  return total;
}
