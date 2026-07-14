#include "interface.h"

#include "zstd.h"
#include <string>
#include <vector>

int compress_payload(const std::string& input, std::string& compressed) {
    thread_local std::vector<char> buf;

    const size_t bound = ZSTD_compressBound(input.size());
    if (bound > buf.size()) {
        buf.resize(bound);
    }

    const size_t cSize = ZSTD_compress2(
        ZSTD_createCCtx(),
        buf.data(), buf.size(),
        input.data(), input.size()
    );

    if (ZSTD_isError(cSize)) {
        return -1;
    }

    compressed.assign(buf.data(), cSize);
    return static_cast<int>( (cSize);
}