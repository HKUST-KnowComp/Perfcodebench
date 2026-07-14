#include "interface.h"
#include "simdutf.h"
#include <string>
#include <memory>

uint64_t encode_checksum(const std::string& input) {
    size_t expected_len = ((input.size() + 2) / 3) * 4;
    std::unique_ptr<char[]> out(new char[expected_len]);
    size_t written = simdutf::binary_to_base64(input.data(), input.size(), out.get());
    
    uint64_t hash = 1469598103934665603ULL;
    const char* data = out.get();
    for (size_t i = 0; i < written; ++i) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
        hash *= 1099511628211ULL;
    }
    return hash;
}
