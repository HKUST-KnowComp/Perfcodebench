#include "interface.h"

#include "zstd.h"

#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
    const size_t srcSize = input.size();
    const size_t maxDstSize = ZSTD_compressBound(srcSize);
    compressed.resize(maxDstSize);
    size_t const cSize = ZSTD_compress(compressed.data(), maxDstSize, input.data(), srcSize, /* compression level */ 1);
    if (ZSTD_isError(cSize)) {
        return -1;
    }
    compressed.resize(cSize);
    return static_cast<int>(cSize);
}