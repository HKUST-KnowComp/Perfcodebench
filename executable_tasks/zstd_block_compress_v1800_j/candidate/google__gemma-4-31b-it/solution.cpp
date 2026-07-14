#include "interface.h"
#include "zstd.h"
#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
    if (input.empty()) {
        compressed.clear();
        return 0;
    }

    const size_t input_size = input.size();
    const size_t bound = ZSTD_compressBound(input_size);
    
    compressed.resize(bound);
    
    // Compression level 3 is the default and provides a much better 
    // throughput-to-compression ratio than level 19.
    const size_t compressed_size = ZSTD_compress(
        compressed.data(), 
        compressed.size(), 
        input.data(), 
        input_size, 
        3
    );

    if (ZSTD_isError(compressed_size)) {
        return -1;
    }

    compressed.resize(compressed_size);
    return static_cast<int>(compressed_size);
}