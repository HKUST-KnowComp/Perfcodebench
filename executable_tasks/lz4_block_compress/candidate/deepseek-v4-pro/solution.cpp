#include "interface.h"

#include "lz4.h"

int compress_payload(const std::string& input, std::string& compressed) {
    const int srcSize = static_cast<int>(input.size());
    const int dstCapacity = LZ4_compressBound(srcSize);
    compressed.resize(static_cast<std::size_t>(dstCapacity));
    const int compressedSize = LZ4_compress_default(input.data(), compressed.data(), srcSize, dstCapacity);
    if (compressedSize <= 0) {
        return -1;
    }
    compressed.resize(static_cast<std::size_t>(compressedSize));
    return compressedSize;
}
