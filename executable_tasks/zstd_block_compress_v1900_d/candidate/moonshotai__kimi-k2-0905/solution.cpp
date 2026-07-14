#include "interface.h"
#include "zstd.h"
#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
    size_t const bound = ZSTD_compressBound(input.size());
    compressed.resize(bound);
    size_t const n = ZSTD_compress(compressed.data(), bound,
                                   input.data(), input.size(), 19);
    if (ZSTD_isError(n)) return -1;
    compressed.resize(n);
    return static_cast<int>(n);
}