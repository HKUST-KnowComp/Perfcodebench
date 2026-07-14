#include "interface.h"
#include <vector>
#include <string>
#include <cstring>

namespace {

// Fast integer to string conversion writing directly to buffer
inline char* write_uint(char* ptr, uint32_t val) {
    if (val == 0) {
        *ptr++ = '0';
        return ptr;
    }
    char buf[10];
    int i = 0;
    while (val > 0) {
        buf[i++] = (val % 10) + '0';
        val /= 10;
    }
    while (i > 0) {
        *ptr++ = buf[--i];
    }
    return ptr;
}

inline char* write_uint16(char* ptr, uint16_t val) {
    if (val == 0) {
        *ptr++ = '0';
        return ptr;
    }
    char buf[5];
    int i = 0;
    while (val > 0) {
        buf[i++] = (val % 10) + '0';
        val /= 10;
    }
    while (i > 0) {
        *ptr++ = buf[--i];
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
    size_t max_line_size = 11 + 1 + 11 + 1 + 6 + 1 + 1024; // heuristic
    size_t total_capacity = 0;
    for (const auto& m : messages) {
        total_capacity += 30 + m.size();
    }
    
    std::vector<char> buffer(total_capacity + 1024);
    uint64_t fnv_offset_basis = 1469598103934665603ULL;
    uint64_t fnv_prime = 1099511628211ULL;

    for (int iter = 0; iter < iters; ++iter) {
        char* ptr = buffer.data();
        for (std::size_t i = 0; i < ids.size(); ++i) {
            ptr = write_uint(ptr, timestamps[i]);
            *ptr++ = '|';
            ptr = write_uint(ptr, ids[i]);
            *ptr++ = '|';
            ptr = write_uint16(ptr, levels[i]);
            *ptr++ = '|';
            
            const std::string& msg = messages[i];
            std::memcpy(ptr, msg.data(), msg.size());
            ptr += msg.size();
            *ptr++ = '\n';
        }

        // Calculate checksum on the populated part of the buffer
        uint64_t hash = fnv_offset_basis;
        size_t len = ptr - buffer.data();
        for (size_t i = 0; i < len; ++i) {
            hash ^= static_cast<uint64_t>(static_cast<unsigned char>(buffer[i]));
            hash *= fnv_prime;
        }
        
        // The problem implies the checksum is returned for the final iteration
        // but the loop structure suggests we return the last calculated hash.
        if (iter == iters - 1) {
            return hash;
        }
    }

    return 0;
}