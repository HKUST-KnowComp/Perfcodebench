#include "interface.h"

#include <string>
#include <vector>
#include <cstdio>

namespace {

// FNV-1a 64-bit constants
constexpr uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME = 1099511628211ULL;

// Fast integer to string conversion that writes directly to a buffer
inline char* write_uint(char* buf, uint32_t val) {
    if (val == 0) {
        *buf++ = '0';
        return buf;
    }
    char temp[10];
    int pos = 0;
    while (val > 0) {
        temp[pos++] = (val % 10) + '0';
        val /= 10;
    }
    while (pos > 0) {
        *buf++ = temp[--pos];
    }
    return buf;
}

}  // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
    
    if (iters <= 0) return 0;

    // Pre-calculate a reasonable buffer size to avoid reallocations
    // Max uint32 is 10 digits, max uint16 is 5 digits. 
    // Format: timestamp|id|level|message\n
    size_t total_msg_len = 0;
    for (const auto& m : messages) total_msg_len += m.length();
    
    // Estimate max buffer size: (10 + 1 + 10 + 1 + 5 + 1) * count + total_msg_len + count
    size_t estimated_size = (28ULL * messages.size()) + total_msg_len + messages.size();
    std::vector<char> buffer; 
    buffer.reserve(estimated_size);

    uint64_t final_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        buffer.clear();
        
        for (std::size_t i = 0; i < ids.size(); ++i) {
            // We use a small local buffer for the numbers to avoid frequent push_back
            char num_buf[32];
            char* p = num_buf;
            
            p = write_uint(p, timestamps[i]);
            *p++ = '|';
            p = write_uint(p, ids[i]);
            *p++ = '|';
            p = write_uint(p, (uint32_t)levels[i]);
            *p++ = '|';
            
            // Append the number block
            buffer.insert(buffer.end(), num_buf, p);
            // Append the message
            buffer.insert(buffer.end(), messages[i].begin(), messages[i].end());
            // Append newline
            buffer.push_back('\n');
        }

        // Compute FNV-1a hash of the resulting buffer
        uint64_t hash = FNV_OFFSET_BASIS;
        for (char c : buffer) {
            hash ^= static_cast<uint8_t>(c);
            hash *= FNV_PRIME;
        }
        final_hash = hash;
    }

    return final_hash;
}