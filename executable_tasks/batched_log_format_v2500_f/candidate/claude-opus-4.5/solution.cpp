#include "interface.h"

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace {

// Lookup table for two-digit numbers
static const char digit_pairs[201] =
    "00010203040506070809"
    "10111213141516171819"
    "20212223242526272829"
    "30313233343536373839"
    "40414243444546474849"
    "50515253545556575859"
    "60616263646566676869"
    "70717273747576777879"
    "80818283848586878889"
    "90919293949596979899";

// Fast uint32 to string, returns pointer past last written char
inline char* u32_to_str(uint32_t val, char* buf) {
    if (val == 0) {
        *buf = '0';
        return buf + 1;
    }
    
    char temp[10];
    char* p = temp + 10;
    
    while (val >= 100) {
        uint32_t idx = (val % 100) * 2;
        val /= 100;
        *--p = digit_pairs[idx + 1];
        *--p = digit_pairs[idx];
    }
    
    if (val >= 10) {
        uint32_t idx = val * 2;
        *--p = digit_pairs[idx + 1];
        *--p = digit_pairs[idx];
    } else {
        *--p = '0' + val;
    }
    
    size_t len = temp + 10 - p;
    memcpy(buf, p, len);
    return buf + len;
}

// Fast uint16 to string
inline char* u16_to_str(uint16_t val, char* buf) {
    if (val == 0) {
        *buf = '0';
        return buf + 1;
    }
    
    char temp[5];
    char* p = temp + 5;
    
    while (val >= 100) {
        uint32_t idx = (val % 100) * 2;
        val /= 100;
        *--p = digit_pairs[idx + 1];
        *--p = digit_pairs[idx];
    }
    
    if (val >= 10) {
        uint32_t idx = val * 2;
        *--p = digit_pairs[idx + 1];
        *--p = digit_pairs[idx];
    } else {
        *--p = '0' + val;
    }
    
    size_t len = temp + 5 - p;
    memcpy(buf, p, len);
    return buf + len;
}

inline uint64_t checksum_buffer(const char* data, size_t len) {
    uint64_t hash = 1469598103934665603ULL;
    const char* end = data + len;
    
    while (data != end) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(*data++));
        hash *= 1099511628211ULL;
    }
    return hash;
}

}  // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
    
    const size_t n = ids.size();
    if (n == 0 || iters <= 0) {
        return 1469598103934665603ULL; // hash of empty string
    }
    
    // Estimate buffer size: max 10 digits for timestamp + 1 + max 10 for id + 1 + max 5 for level + 1 + msg + 1
    // = 28 + msg.size() per line
    size_t estimated_size = 0;
    for (size_t i = 0; i < n; ++i) {
        estimated_size += 28 + messages[i].size();
    }
    
    std::vector<char> buffer(estimated_size);
    char* ptr = buffer.data();
    
    // Format all lines once
    for (size_t i = 0; i < n; ++i) {
        ptr = u32_to_str(timestamps[i], ptr);
        *ptr++ = '|';
        ptr = u32_to_str(ids[i], ptr);
        *ptr++ = '|';
        ptr = u16_to_str(levels[i], ptr);
        *ptr++ = '|';
        size_t msg_len = messages[i].size();
        memcpy(ptr, messages[i].data(), msg_len);
        ptr += msg_len;
        *ptr++ = '\n';
    }
    
    size_t total_len = ptr - buffer.data();
    
    // Compute checksum iters times on the same buffer
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = checksum_buffer(buffer.data(), total_len);
    }
    
    return hash;
}