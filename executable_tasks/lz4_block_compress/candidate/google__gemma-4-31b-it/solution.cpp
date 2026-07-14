#include "interface.h"
#include "lz4.h"

int compress_payload(const std::string& input, std::string& compressed) {
    if (input.empty()) {
        compressed.clear();
        return 0;
    }

    const int input_size = static_cast<int>(input.size());
    const int max_compressed_size = LZ4_compressBound(input_size);
    
    compressed.resize(static_cast<std::size_t>(max_compressed_size));
    
    // Use LZ4_compress_fast for significantly higher throughput than LZ4_compress_HC
    // The acceleration parameter (e.g., 1) provides a good balance of speed and ratio
    const int compressed_size = LZ4_compress_fast(
        input.data(), 
        compressed.data(), 
        input_size, 
        max_compressed_size, 
        1
    );

    if (compressed_size <= 0) {
        return -1;
    }

    compressed.resize(static_cast<std::size_t>(compressed_size));
    return compressed_size;
}