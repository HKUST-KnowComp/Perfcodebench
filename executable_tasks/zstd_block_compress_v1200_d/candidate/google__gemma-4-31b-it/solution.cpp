#include "interface.h"
#include "zstd.h"
#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
    // Remove the 'const std::string local = input;' copy to avoid O(N) allocation and memory copy
    size_t const input_size = input.size();
    size_t const max_compressed_size = ZSTD_compressBound(input_size);
    
    compressed.resize(max_compressed_size);
    
    // Use compression level 3 instead of 19. 
    // Level 19 is extremely CPU intensive and usually overkill for general throughput tasks.
    // Level 3 provides a much better balance of speed and compression ratio.
    size_t const compressed_size = ZSTD_compress(
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