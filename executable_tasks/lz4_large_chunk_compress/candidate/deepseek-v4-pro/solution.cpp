#include "interface.h"

#include "lz4.h"

#include <algorithm>

namespace {
constexpr std::size_t kChunkSize = 1 << 18;   // 256 KB
constexpr int kAcceleration = 5;
}  // namespace

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
    chunks.clear();
    if (input.empty()) {
        return 0;
    }

    const std::size_t numChunks = (input.size() + kChunkSize - 1) / kChunkSize;
    chunks.reserve(numChunks);

    // Reusable compression state to avoid per‑chunk allocation overhead
    LZ4_stream_t lz4State{};

    int total = 0;
    for (std::size_t offset = 0; offset < input.size(); offset += kChunkSize) {
        const std::size_t srcSize = std::min(kChunkSize, input.size() - offset);
        const int srcSizeInt = static_cast<int>(srcSize);
        const int maxDstSize = LZ4_compressBound(srcSizeInt);

        std::string compressed;
        compressed.resize(maxDstSize);

        const int n = LZ4_compress_fast_extState(
            &lz4State, input.data() + offset, compressed.data(),
            srcSizeInt, maxDstSize, kAcceleration);

        if (n <= 0) {
            return -1;
        }

        compressed.resize(static_cast<std::size_t>(n));
        chunks.push_back(std::move(compressed));
        total += n;
    }

    return total;
}