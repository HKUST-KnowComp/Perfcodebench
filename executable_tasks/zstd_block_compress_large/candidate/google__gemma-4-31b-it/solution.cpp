#include "interface.h"
#include "zstd.h"
#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
    // Avoid copying the input string
    const size_t input_size = input.size();
    const char* input_ptr = input.data();

    // Pre-allocate the output buffer to avoid multiple reallocations
    const size_t bound = ZSTD_compressBound(input_size);
    compressed.resize(bound);

    // Use a lower compression level (e.g., 3) for significantly higher throughput
    // while still maintaining good compression ratios. Level 19 is extremely slow.
    const size_t compressed_size = ZSTD_compress(
        compressed.data(), bound, 
        input_ptr, input_size, 
        3
    );

    if (ZSTD_isError(compressed_size)) {
        return -1;
    }

    // Shrink the string to the actual size of the compressed data
    compressed.resize(compressed_size);
    return static_cast<int>(compressed_size);
}