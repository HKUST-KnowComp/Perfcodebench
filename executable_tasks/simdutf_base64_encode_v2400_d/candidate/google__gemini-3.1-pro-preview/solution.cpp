#include "interface.h"
#include <string>
#include <memory>
#include <cstdint>
#include "simdutf.h"

uint64_t encode_checksum(const std::string& input) {
    size_t expected_len = ((input.size() + 2) / 3) * 4;
    
    char stack_buf[4096];
    char* out = stack_buf;
    std::unique_ptr<char[]> heap_buf;
    
    if (expected_len > sizeof(stack_buf)) {
        heap_buf.reset(new char[expected_len]);
        out = heap_buf.get();
    }
    
    size_t written = simdutf::binary_to_base64(input.data(), input.size(), out);
    
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;
    size_t i = 0;
    
    for (; i + 8 <= written; i += 8) {
        hash ^= static_cast<uint8_t>(out[i]);     hash *= prime;
        hash ^= static_cast<uint8_t>(out[i+1]);   hash *= prime;
        hash ^= static_cast<uint8_t>(out[i+2]);   hash *= prime;
        hash ^= static_cast<uint8_t>(out[i+3]);   hash *= prime;
        hash ^= static_cast<uint8_t>(out[i+4]);   hash *= prime;
        hash ^= static_cast<uint8_t>(out[i+5]);   hash *= prime;
        hash ^= static_cast<uint8_t>(out[i+6]);   hash *= prime;
        hash ^= static_cast<uint8_t>(out[i+7]);   hash *= prime;
    }
    for (; i < written; ++i) {
        hash ^= static_cast<uint8_t>(out[i]);
        hash *= prime;
    }
    
    return hash;
}
