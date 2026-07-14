#include "interface.h"
#include <string>
#include <vector>
#include <cstdint>

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
    (void)iters;
    size_t n = ids.size();
    if (n == 0) return 1469598103934665603ULL;

    size_t total_len = 0;
    for (size_t i = 0; i < n; ++i) {
        total_len += messages[i].size() + 4;
        uint32_t t = timestamps[i]; do { t /= 10; total_len++; } while (t);
        uint32_t id = ids[i]; do { id /= 10; total_len++; } while (id);
        uint16_t lv = levels[i]; do { lv /= 10; total_len++; } while (lv);
    }

    std::string out;
    out.resize(total_len);
    char* p = &out[0];
    
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t FNV_PRIME = 1099511628211ULL;

    for (size_t i = 0; i < n; ++i) {
        uint32_t t = timestamps[i];
        char tmp[10]; int k = 0;
        do { tmp[k++] = '0' + (t % 10); t /= 10; } while (t);
        while (k) { unsigned char c = tmp[--k]; *p++ = c; hash ^= c; hash *= FNV_PRIME; }
        
        *p++ = '|'; hash ^= '|'; hash *= FNV_PRIME;
        
        uint32_t id = ids[i];
        k = 0;
        do { tmp[k++] = '0' + (id % 10); id /= 10; } while (id);
        while (k) { unsigned char c = tmp[--k]; *p++ = c; hash ^= c; hash *= FNV_PRIME; }
        
        *p++ = '|'; hash ^= '|'; hash *= FNV_PRIME;
        
        uint16_t lv = levels[i];
        k = 0;
        do { tmp[k++] = '0' + (lv % 10); lv /= 10; } while (lv);
        while (k) { unsigned char c = tmp[--k]; *p++ = c; hash ^= c; hash *= FNV_PRIME; }
        
        *p++ = '|'; hash ^= '|'; hash *= FNV_PRIME;
        
        for (char c : messages[i]) {
            *p++ = c; hash ^= c; hash *= FNV_PRIME;
        }
        *p++ = '\n'; hash ^= '\n'; hash *= FNV_PRIME;
    }
    
    return hash;
}