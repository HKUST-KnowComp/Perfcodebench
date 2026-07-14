#include "interface.h"

#include <cstring>
#include <string>
#include <vector>

namespace {

uint64_t checksum_text(const char* data, const std::size_t len) {
    uint64_t hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
        hash *= 1099511628211ULL;
    }
    return hash;
}

// write decimal into buf, return number of chars written
inline unsigned write_u32(char* buf, uint32_t v) {
    if (v == 0) { buf[0] = '0'; return 1; }
    char tmp[10];
    unsigned len = 0;
    while (v) {
        tmp[len++] = static_cast<char>('0' + (v % 10));
        v /= 10;
    }
    for (unsigned i = 0; i < len; ++i) buf[i] = tmp[len - 1 - i];
    return len;
}

inline unsigned write_u16(char* buf, uint16_t v) {
    if (v == 0) { buf[0] = '0'; return 1; }
    char tmp[5];
    unsigned len = 0;
    while (v) {
        tmp[len++] = static_cast<char>('0' + (v % 10));
        v = static_cast<uint16_t>(v / 10);
    }
    for (unsigned i = 0; i < len; ++i) buf[i] = tmp[len - 1 - i];
    return len;
}

}  // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters)
{
    const std::size_t rows = ids.size();
    // pre-size output buffer to avoid reallocations
    std::size_t total = 0;
    for (std::size_t i = 0; i < rows; ++i) total += messages[i].size();
    total += rows * 26; // upper bound for numbers plus three '|' and '\n'
    std::string out;
    out.reserve(total);

    char num_buf[32];
    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        out.clear();
        const char* ts_data = reinterpret_cast<const char*>(timestamps.data());
        const char* id_data = reinterpret_cast<const char*>(ids.data());
        const char* lvl_data = reinterpret_cast<const char*>(levels.data());
        const char* const* msg_beg = reinterpret_cast<const char* const*>(messages.data());
        for (std::size_t i = 0; i < rows; ++i) {
            uint32_t ts = *reinterpret_cast<const uint32_t*>(ts_data + i * sizeof(uint32_t));
            uint32_t id = *reinterpret_cast<const uint32_t*>(id_data + i * sizeof(uint32_t));
            uint16_t lvl = *reinterpret_cast<const uint16_t*>(lvl_data + i * sizeof(uint16_t));
            const std::string& msg = messages[i];

            unsigned ts_len = write_u32(num_buf, ts);
            out.append(num_buf, ts_len);
            out.push_back('|');
            unsigned id_len = write_u32(num_buf, id);
            out.append(num_buf, id_len);
            out.push_back('|');
            unsigned lvl_len = write_u16(num_buf, lvl);
            out.append(num_buf, lvl_len);
            out.push_back('|');
            out.append(msg);
            out.push_back('\n');
        }
        hash = checksum_text(out.data(), out.size());
    }
    return hash;
}