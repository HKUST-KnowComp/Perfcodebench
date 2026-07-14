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
    
    const uint64_t FNV_OFFSET = 1469598103934665603ULL;
    const uint64_t FNV_PRIME = 1099511628211ULL;
    
    uint64_t final_hash = 0;
    const size_t n = ids.size();
    
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t h = FNV_OFFSET;
        
        for (size_t i = 0; i < n; ++i) {
            // Format timestamps[i]
            uint32_t val = timestamps[i];
            char buf[12];
            char* p = buf + 11;
            if (val == 0) { *--p = '0'; }
            else { while (val) { *--p = '0' + (val % 10); val /= 10; } }
            for (char* q = p; q < buf + 11; ++q) { h ^= (uint64_t)*q; h *= FNV_PRIME; }
            
            h ^= '|'; h *= FNV_PRIME;
            
            // Format ids[i]
            val = ids[i];
            p = buf + 11;
            if (val == 0) { *--p = '0'; }
            else { while (val) { *--p = '0' + (val % 10); val /= 10; } }
            for (char* q = p; q < buf + 11; ++q) { h ^= (uint64_t)*q; h *= FNV_PRIME; }
            
            h ^= '|'; h *= FNV_PRIME;
            
            // Format levels[i]
            uint16_t val16 = levels[i];
            p = buf + 11;
            if (val16 == 0) { *--p = '0'; }
            else { while (val16) { *--p = '0' + (val16 % 10); val16 /= 10; } }
            for (char* q = p; q < buf + 11; ++q) { h ^= (uint64_t)*q; h *= FNV_PRIME; }
            
            h ^= '|'; h *= FNV_PRIME;
            
            // messages[i]
            const std::string& msg = messages[i];
            for (unsigned char c : msg) { h ^= (uint64_t)c; h *= FNV_PRIME; }
            
            h ^= '\n'; h *= FNV_PRIME;
        }
        final_hash = h;
    }
    return final_hash;
}
