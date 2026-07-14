#include "interface.h"

#include <string>
#include <vector>

namespace {

template <typename T>
int write_uint(char* buf, T value) {
    if (value == 0) {
        *buf = '0';
        return 1;
    }
    char tmp[20];
    int len = 0;
    T v = value;
    while (v > 0) {
        tmp[len++] = '0' + (v % 10);
        v /= 10;
    }
    for (int i = 0; i < len; ++i) {
        buf[i] = tmp[len - 1 - i];
    }
    return len;
}

void update_hash(uint64_t& hash, unsigned char c) {
    hash ^= static_cast<uint64_t>(c);
    hash *= 1099511628211ULL;
}

} // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
    const size_t n = ids.size();
    std::string out;
    size_t total_cap = 0;
    for (size_t i = 0; i < n; ++i) {
        total_cap += messages[i].size() + 28; // 10+1+10+1+5+1 max numeric + separators
    }
    out.reserve(total_cap);
    uint64_t final_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        out.clear();
        out.resize(out.capacity());
        char* write_ptr = out.data();
        uint64_t hash = 1469598103934665603ULL;

        for (size_t i = 0; i < n; ++i) {
            // Write timestamp
            int ts_len = write_uint(write_ptr, timestamps[i]);
            for (int j = 0; j < ts_len; ++j) update_hash(hash, static_cast<unsigned char>(write_ptr[j]));
            write_ptr += ts_len;
            // Write separator
            *write_ptr = '|';
            update_hash(hash, '|');
            write_ptr += 1;
            // Write id
            int id_len = write_uint(write_ptr, ids[i]);
            for (int j = 0; j < id_len; ++j) update_hash(hash, static_cast<unsigned char>(write_ptr[j]));
            write_ptr += id_len;
            // Write separator
            *write_ptr = '|';
            update_hash(hash, '|');
            write_ptr += 1;
            // Write level
            int lvl_len = write_uint(write_ptr, levels[i]);
            for (int j = 0; j < lvl_len; ++j) update_hash(hash, static_cast<unsigned char>(write_ptr[j]));
            write_ptr += lvl_len;
            // Write separator
            *write_ptr = '|';
            update_hash(hash, '|');
            write_ptr += 1;
            // Write message
            const std::string& msg = messages[i];
            for (char c : msg) {
                *write_ptr = c;
                update_hash(hash, static_cast<unsigned char>(c));
                write_ptr += 1;
            }
            // Write newline
            *write_ptr = '\n';
            update_hash(hash, '\n');
            write_ptr += 1;
        }
        out.resize(write_ptr - out.data());
        final_hash = hash;
    }
    return final_hash;
}