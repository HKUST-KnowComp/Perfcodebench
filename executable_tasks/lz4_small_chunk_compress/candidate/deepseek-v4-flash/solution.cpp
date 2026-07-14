#include "interface.h"

#include "lz4.h"

#include <string>
#include <vector>

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
    constexpr std::size_t kChunkSize = 1 << 12;
    const std::size_t input_size = input.size();
    chunks.clear();
    const std::size_t num_chunks = (input_size + kChunkSize - 1) / kChunkSize;
    chunks.reserve(num_chunks);

    std::vector<char> compressed_buf(LZ4_compressBound(static_cast<int>(kChunkSize)));

    int total = 0;
    for (std::size_t offset = 0; offset < input_size; offset += kChunkSize) {
        const std::size_t take = (offset + kChunkSize <= input_size) ? kChunkSize : (input_size - offset);
        const char* src = input.data() + offset;
        const int src_size = static_cast<int>(take);
        const int dest_capacity = static_cast<int>(compressed_buf.size());
        const int n = LZ4_compress_fast(src, compressed_buf.data(), src_size, dest_capacity, 9);
        if (n <= 0) {
            return -1;
        }
        chunks.emplace_back(compressed_buf.data(), compressed_buf.data() + n);
        total += n;
    }
    return total;
}
