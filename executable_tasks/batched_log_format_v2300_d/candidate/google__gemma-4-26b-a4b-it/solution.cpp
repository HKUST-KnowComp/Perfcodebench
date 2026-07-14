#include "interface.h"
#include <string>
#include <vector>
#include <cstring>

namespace {

// Fast integer to string conversion writing directly to a buffer
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
inline uint64_t update_hash(uint64_t hash, const char* start, const char* end) {
    for (const char* p = start; p < end; ++p) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(*p));
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
    
    if (ids.empty()) return 1469598103934665603ULL;

    // Pre-calculate maximum possible size to avoid reallocations
    // Max uint32 is 10 digits, max uint16 is 5, plus separators and newline
    size_t max_line_size = 11 + 11 + 6 + 1 + 1; // timestamp|id|level|msg\n
    // We use a large buffer to avoid reallocations. 
    // In a real production environment, we might use a more sophisticated growth strategy.
    std::vector<char> buffer;
    buffer.reserve(ids.size() * 128); 

    uint64_t final_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        buffer.clear();
        // We use a local pointer to write into the vector's data
        // To ensure safety, we resize the vector first.
        // However, since we don't know the exact size, we'll use a growing approach or a large enough estimate.
        // For performance, let's estimate size.
        size_t estimated_size = 0;
        for(size_t i=0; i<ids.size(); ++i) {
            estimated_size += 15 + messages[i].size();
        }
        if (buffer.capacity() < estimated_size) buffer.resize(estimated_size);
        
        char* ptr = buffer.data();
        char* end_ptr = ptr + estimated_size;

        for (std::size_t i = 0; i < ids.size(); ++i) {
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

        // Calculate hash on the written portion
        uint64_t hash = 1469598103934665603ULL;
        // The actual data written is from buffer.data() to ptr
        for (char* p = buffer.data(); p < ptr; ++p) {
            hash ^= static_cast<uint64_t>(static_cast<unsigned char>(*p));
            hash *= 1099511628211ULL;
        }
        final_hash = hash;
    }

    return final_hash;
}
