#include "interface.h"
#include <string>
#include <vector>
#include <cstring>

namespace {

// Fast integer to string conversion for unsigned integers
inline char* append_uint(char* ptr, uint32_t val) {
    if (val == 0) {
        *ptr++ = '0';
        return ptr;
    }
    char buf[10];
    int len = 0;
    while (val > 0) {
        buf[len++] = static_cast<char>('0' + (val % 10));
        val /= 10;
    }
    for (int i = len - 1; i >= 0; --i) {
        *ptr++ = buf[i];
    }
    return ptr;
}

inline char* append_uint16(char* ptr, uint16_t val) {
    if (val == 0) {
        *ptr++ = '0';
        return ptr;
    }
    char buf[5];
    int len = 0;
    while (val > 0) {
        buf[len++] = static_cast<char>('0' + (val % 10));
        val /= 10;
    }
    for (int i = len - 1; i >= 0; --i) {
        *ptr++ = buf[i];
    }
    return ptr;
}

} // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
    
    if (ids.empty()) return 0;

    // Pre-calculate a safe upper bound for buffer size to avoid reallocations
    // Max uint32 is 10 digits, max uint16 is 5, plus separators and message lengths
    size_t total_max_size = 0;
    for (const auto& msg : messages) {
        total_max_size += 10 + 1 + 10 + 1 + 5 + 1 + msg.size() + 1;
    }

    std::vector<char> buffer(total_max_size);
    size_t n = ids.size();

    for (int iter = 0; iter < iters; ++iter) {
        char* ptr = buffer.data();
        for (size_t i = 0; i < n; ++i) {
            ptr = append_uint(ptr, timestamps[i]);
            *ptr++ = '|';
            ptr = append_uint(ptr, ids[i]);
            *ptr++ = '|';
            ptr = append_uint16(ptr, levels[i]);
            *ptr++ = '|';
            
            const std::string& msg = messages[i];
            std::memcpy(ptr, msg.data(), msg.size());
            ptr += msg.size();
            *ptr++ = '\n';
        }

        // Compute FNV-1a checksum on the populated part of the buffer
        uint64_t hash = 1469598103934665603ULL;
        size_t final_len = ptr - buffer.data();
        for (size_t i = 0; i < final_len; ++i) {
            hash ^= static_cast<uint64_t>(static_cast<unsigned char>(buffer[i]));
            hash *= 1099511628211ULL;
        }
        
        // The problem implies the checksum is returned for the last iteration
        // based on the baseline logic (hash is overwritten each iter).
        // However, the baseline returns the hash of the last iteration's 'out'.
        if (iter == iters - 1) {
            return hash;
        }
    }

    return 0;
}