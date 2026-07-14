#include "interface.h"

#include <zstd.h>

#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
    // ensure the buffer is large enough for worst-case expansion
    size_t const dstCapacity = ZSTD_compressBound(input.size());
    if (compressed.capacity() < dstCapacity) {
        compressed.reserve(dstCapacity);
    }

    size_t const cSize = ZSTD_compress(
        compressed.data(),    // destination
        dstCapacity,        // destination capacity
        input.data(),       // source
        input.size(),       // source size
        19                  // compression level
    );

    if (ZSTD_isError(cSize)) {
        return -1;
    }

    // exact-size resize – no heap ops if previously reserved
    compressed.resize(cSize);
    return static_cast<int>(cSize);
}