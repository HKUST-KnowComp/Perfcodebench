#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
    uint64_t total_sum = 0;
    const uint8_t* data = encoded.data();
    const size_t size = encoded.size();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_iter_sum = 0;
        size_t i = 0;
        while (i < size) {
            uint32_t value = data[i];
            if (value < 128) {
                current_iter_sum += value;
                i += 1;
            } else {
                value &= 127;
                uint8_t byte = data[i + 1];
                value |= (static_cast<uint32_t>(byte & 127) << 7);
                if (byte < 128) {
                    current_iter_sum += value;
                    i += 2;
                } else {
                    uint32_t shift = 14;
                    i += 2;
                    while (data[i - 1] & 128) {
                        value |= (static_cast<uint32_t>(data[i] & 127) << shift);
                        shift += 7;
                        i++;
                    }
                    current_iter_sum += value;
                }
            }
        }
        total_sum = current_iter_sum;
    }
    return total_sum;
}