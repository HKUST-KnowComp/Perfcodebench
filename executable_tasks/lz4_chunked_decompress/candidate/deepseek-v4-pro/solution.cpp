#include "interface.h"

#include "lz4.h"

uint64_t decompress_checksum(const std::vector<std::string>& chunks, const std::vector<int>& sizes) {
    uint64_t sum = 0;
    std::string restored;
    for (std::size_t i = 0; i < chunks.size(); ++i) {
        const int orig_size = sizes[i];
        restored.resize(static_cast<std::size_t>(orig_size));
        const int decoded = LZ4_decompress_safe(
            chunks[i].data(), restored.data(), static_cast<int>(chunks[i].size()), orig_size);
        if (decoded != orig_size) {
            return 0;
        }
        const unsigned char* p = reinterpret_cast<const unsigned char*>(restored.data());
        const unsigned char* end = p + orig_size;
        while (p < end) {
            sum += static_cast<uint64_t>(*p);
            ++p;
        }
    }
    return sum;
}
