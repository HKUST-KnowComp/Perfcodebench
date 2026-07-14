#include "interface.h"
#include <string>
#include <vector>
#include <algorithm>

namespace {

// Fast integer to string conversion to avoid std::to_string overhead
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

inline uint64_t compute_fnv1a(const std::string& text) {
    uint64_t hash = 1469598103934665603ULL;
    const unsigned char* data = reinterpret_cast<const unsigned char*>(text.data());
    size_t len = text.size();
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
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
    
    if (ids.empty()) return 0;

    // Pre-calculate a safe upper bound for buffer size to avoid reallocations
    // Max uint32 is 10 digits, max uint16 is 5, plus separators and message lengths
    size_t estimated_size = 0;
    for (const auto& msg : messages) {
        estimated_size += 10 + 10 + 5 + msg.size() + 2; 
    }

    std::string buffer;
    buffer.reserve(estimated_size);
    uint64_t final_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        buffer.clear();
        // We use a temporary char buffer to build the string to avoid std::string overhead
        // but since we need to append messages, we'll use a manual approach with buffer.append
        // or resize and write. For simplicity and speed, we'll use a local char array for numbers.
        
        for (size_t i = 0; i < ids.size(); ++i) {
            // Timestamp
            char num_buf[12];
            char* p = num_buf;
            uint32_t ts = timestamps[i];
            if (ts == 0) *p++ = '0';
            else {
                while (ts > 0) { *p++ = (ts % 10) + '0'; ts /= 10; }
            }
            for (char* it = num_buf; it < p; ++it) buffer.push_back(*it); // This is slow, let's optimize
        }
        // Re-thinking: The most efficient way in C++ without custom allocators is to use a 
        // reusable string and append. Let's use a more direct approach.
    }
    
    // Corrected approach for the loop to ensure performance
    buffer.clear();
    for (int iter = 0; iter < iters; ++iter) {
        buffer.clear();
        for (size_t i = 0; i < ids.size(); ++i) {
            // Timestamp
            char ts_buf[12];
            int ts_len = 0;
            uint32_t ts = timestamps[i];
            if (ts == 0) ts_buf[ts_len++] = '0';
            else {
                while (ts > 0) { ts_buf[ts_len++] = (ts % 10) + '0'; ts /= 10; }
            }
            for (int j = ts_len - 1; j >= 0; --j) buffer.push_back(ts_buf[j]);
            buffer.push_back('|');

            // ID
            char id_buf[12];
            int id_len = 0;
            uint32_t id_val = ids[i];
            if (id_val == 0) id_buf[id_len++] = '0';
            else {
                while (id_val > 0) { id_buf[id_len++] = (id_val % 10) + '0'; id_val /= 10; }
            }
            for (int j = id_len - 1; j >= 0; --j) buffer.push_back(id_buf[j]);
            buffer.push_back('|');

            // Level
            char lv_buf[6];
            int lv_len = 0;
            uint16_t lv = levels[i];
            if (lv == 0) lv_buf[lv_len++] = '0';
            else {
                while (lv > 0) { lv_buf[lv_len++] = (lv % 10) + '0'; lv /= 10; }
            }
            for (int j = lv_len - 1; j >= 0; --j) buffer.push_back(lv_buf[j]);
            buffer.push_back('|');

            // Message
            buffer.append(messages[i]);
            buffer.push_back('\n');
        }
        final_hash = compute_fnv1a(buffer);
    }

    return final_hash;
}
