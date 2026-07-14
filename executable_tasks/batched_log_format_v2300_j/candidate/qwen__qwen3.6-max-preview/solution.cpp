#include "interface.h"
#include <string>
#include <vector>
#include <cstdint>
#include <cstring>

namespace {

inline int fast_utoa(uint32_t val, char* buf) {
    if (val == 0) { buf[0] = '0'; return 1; }
    char tmp[12];
    char* p = tmp;
    while (val >= 10) {
        uint32_t q = val / 10;
        *p++ = '0' + (val - q * 10);
        val = q;
    }
    *p++ = '0' + val;
    int len = p - tmp;
    for (int i = 0; i < len; ++i) buf[i] = tmp[len - 1 - i];
    return len;
}

inline uint64_t checksum_text(const char* data, size_t len) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
    
    size_t n = ids.size();
    if (n == 0) {
        return 1469598103934665603ULL;
    }

    size_t estimated_size = 0;
    for (size_t i = 0; i < n; ++i) {
        estimated_size += 32 + messages[i].size();
    }
    
    std::vector<char> buf(estimated_size);
    char num_buf[16];
    uint64_t hash = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        char* ptr = buf.data();
        for (size_t i = 0; i < n; ++i) {
            int len;
            
            len = fast_utoa(timestamps[i], num_buf);
            std::memcpy(ptr, num_buf, len); ptr += len;
            *ptr++ = '|';
            
            len = fast_utoa(ids[i], num_buf);
            std::memcpy(ptr, num_buf, len); ptr += len;
            *ptr++ = '|';
            
            len = fast_utoa(levels[i], num_buf);
            std::memcpy(ptr, num_buf, len); ptr += len;
            *ptr++ = '|';
            
            const std::string& msg = messages[i];
            std::memcpy(ptr, msg.data(), msg.size()); ptr += msg.size();
            *ptr++ = '\n';
        }
        hash = checksum_text(buf.data(), ptr - buf.data());
    }
    return hash;
}
