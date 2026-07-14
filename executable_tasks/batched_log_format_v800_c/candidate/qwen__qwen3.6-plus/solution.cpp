#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
    
    uint64_t hash = 0;
    constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
    constexpr uint64_t FNV_PRIME = 1099511628211ULL;

    for (int iter = 0; iter < iters; ++iter) {
        hash = FNV_OFFSET;
        const size_t n = ids.size();
        for (size_t i = 0; i < n; ++i) {
            // timestamp
            uint32_t val = timestamps[i];
            char buf[12];
            int len = 0;
            if (val == 0) buf[len++] = '0';
            else while (val) { buf[len++] = '0' + (val % 10); val /= 10; }
            for (int j = len - 1; j >= 0; --j) { hash ^= static_cast<uint64_t>(static_cast<unsigned char>(buf[j])); hash *= FNV_PRIME; }
            
            hash ^= '|'; hash *= FNV_PRIME;
            
            // id
            val = ids[i];
            len = 0;
            if (val == 0) buf[len++] = '0';
            else while (val) { buf[len++] = '0' + (val % 10); val /= 10; }
            for (int j = len - 1; j >= 0; --j) { hash ^= static_cast<uint64_t>(static_cast<unsigned char>(buf[j])); hash *= FNV_PRIME; }
            
            hash ^= '|'; hash *= FNV_PRIME;
            
            // level
            val = levels[i];
            len = 0;
            if (val == 0) buf[len++] = '0';
            else while (val) { buf[len++] = '0' + (val % 10); val /= 10; }
            for (int j = len - 1; j >= 0; --j) { hash ^= static_cast<uint64_t>(static_cast<unsigned char>(buf[j])); hash *= FNV_PRIME; }
            
            hash ^= '|'; hash *= FNV_PRIME;
            
            // message
            const std::string& msg = messages[i];
            const char* p = msg.data();
            const char* end = p + msg.size();
            while (p != end) {
                hash ^= static_cast<uint64_t>(static_cast<unsigned char>(*p));
                hash *= FNV_PRIME;
                ++p;
            }
            
            hash ^= '\n'; hash *= FNV_PRIME;
        }
    }
    return hash;
}