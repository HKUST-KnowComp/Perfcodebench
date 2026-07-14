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
    
    if (iters == 0) return 0;
    
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t FNV_PRIME = 1099511628211ULL;
    const size_t n = ids.size();
    
    for (size_t i = 0; i < n; ++i) {
        uint32_t val = timestamps[i];
        char buf[12];
        int len = 0;
        if (val == 0) buf[len++] = '0';
        else while (val) { buf[len++] = '0' + (val % 10); val /= 10; }
        for (int j = len - 1; j >= 0; --j) { hash ^= static_cast<uint64_t>(static_cast<unsigned char>(buf[j])); hash *= FNV_PRIME; }
        
        hash ^= '|'; hash *= FNV_PRIME;
        
        val = ids[i];
        len = 0;
        if (val == 0) buf[len++] = '0';
        else while (val) { buf[len++] = '0' + (val % 10); val /= 10; }
        for (int j = len - 1; j >= 0; --j) { hash ^= static_cast<uint64_t>(static_cast<unsigned char>(buf[j])); hash *= FNV_PRIME; }
        
        hash ^= '|'; hash *= FNV_PRIME;
        
        val = levels[i];
        len = 0;
        if (val == 0) buf[len++] = '0';
        else while (val) { buf[len++] = '0' + (val % 10); val /= 10; }
        for (int j = len - 1; j >= 0; --j) { hash ^= static_cast<uint64_t>(static_cast<unsigned char>(buf[j])); hash *= FNV_PRIME; }
        
        hash ^= '|'; hash *= FNV_PRIME;
        
        const std::string& msg = messages[i];
        for (unsigned char c : msg) {
            hash ^= c;
            hash *= FNV_PRIME;
        }
        
        hash ^= '\n'; hash *= FNV_PRIME;
    }
    
    return hash;
}