#include "interface.h"

#include <string>
#include <vector>
#include <type_traits>

namespace {

template <typename T>
void append_uint(T val, uint64_t& hash) {
    static_assert(std::is_unsigned_v<T>, "append_uint requires unsigned integer type");
    if (val == 0) {
        hash ^= static_cast<uint64_t>('0');
        hash *= 1099511628211ULL;
        return;
    }

    T div = 1;
    T tmp = val;
    while (tmp >= 10) {
        tmp /= 10;
        div *= 10;
    }

    while (div > 0) {
        const T digit = val / div;
        hash ^= static_cast<uint64_t>('0' + static_cast<char>(digit));
        hash *= 1099511628211ULL;
        val -= digit * div;
        div /= 10;
    }
}

} // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
    const size_t num_entries = timestamps.size();
    constexpr uint64_t fnv_prime = 1099511628211ULL;
    constexpr uint64_t fnv_offset = 1469598103934665603ULL;
    uint64_t final_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_hash = fnv_offset;
        for (size_t i = 0; i < num_entries; ++i) {
            append_uint(timestamps[i], current_hash);
            current_hash ^= static_cast<uint64_t>('|');
            current_hash *= fnv_prime;

            append_uint(ids[i], current_hash);
            current_hash ^= static_cast<uint64_t>('|');
            current_hash *= fnv_prime;

            append_uint(levels[i], current_hash);
            current_hash ^= static_cast<uint64_t>('|');
            current_hash *= fnv_prime;

            for (const char c : messages[i]) {
                current_hash ^= static_cast<uint64_t>(static_cast<unsigned char>(c));
                current_hash *= fnv_prime;
            }

            current_hash ^= static_cast<uint64_t>('\n');
            current_hash *= fnv_prime;
        }
        final_hash = current_hash;
    }

    return final_hash;
}