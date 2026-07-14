#include "interface.h"
#include "zstd.h"
#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
    // Avoid copying the input string
    size_t const input_size = input.size();
    
    // Pre-allocate buffer to avoid multiple reallocations
    size_t const bound = ZSTD_compressBound(input_size);
    compressed.resize(bound);
    
    // Use compression level 3 for a better balance of speed and ratio.
    // Level 19 is extremely slow and usually overkill for general throughput tasks.
    size_t const c_size = ZSTD_compress(
        compressed.data(), compressed.size(), 
        input.data(), input_size, 
        3
    );

    if (ZSTD_isError(c_size)) {
        return -1;
    }

    compressed.resize(c_size);
    return static_cast<int>(c_size);
}