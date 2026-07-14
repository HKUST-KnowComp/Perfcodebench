#include "interface.h"
#include <string>
#include <vector>
#include <cstdint>

namespace {

// Fast integer to string conversion that writes directly to a buffer
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

// FNV-1a hash implementation
inline void update_hash(uint64_t& hash, const char* data, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
        hash *= 1099511628211ULL;
    }
}

} // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
    
    if (ids.empty()) return 1469598103934665603ULL;

    // Pre-calculate a safe upper bound for buffer size to avoid reallocations
    // Max uint32 is 10 digits, max uint16 is 5, plus separators and message lengths
    size_t estimated_size = 0;
    for (const auto& msg : messages) {
        estimated_size += 10 + 10 + 5 + msg.size() + 2; 
    }

    std::string buffer;
    buffer.resize(estimated_size);
    char* buf_ptr = buffer.data();
    char* end_ptr = buffer.data() + buffer.size();

    uint64_t final_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        char* curr = buf_ptr;
        
        for (std::size_t i = 0; i < ids.size(); ++i) {
            curr = append_uint(curr, timestamps[i]);
            *curr++ = '|';
            curr = append_uint(curr, ids[i]);
            *curr++ = '|';
            curr = append_uint16(curr, levels[i]);
            *curr++ = '|';
            
            const std::string& msg = messages[i];
            for (char c : msg) *curr++ = c;
            *curr++ = '\n';
        }

        // Calculate hash on the valid portion of the buffer
        uint64_t hash = 1469598103934665603ULL;
        size_t actual_len = curr - buf_ptr;
        
        // Inline hash calculation for speed
        for (size_t i = 0; i < actual_len; ++i) {
            hash ^= static_cast<uint64_t>(static_cast<unsigned char>(buf_ptr[i]));
            hash *= 1099511628211ULL;
        }
        final_hash = hash;
    }

    return final_hash;
}