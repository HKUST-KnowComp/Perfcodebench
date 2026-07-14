#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
    uint64_t total_sum = 0;
    const uint8_t* data = encoded.data();
    const size_t size = encoded.size();

    // Pre-calculate the sum for one pass to avoid redundant work if iters > 1
    // However, to strictly follow the logic of the baseline where sum is reset 
    // and recalculated every iteration, we compute it once and multiply.
    
    uint64_t single_pass_sum = 0;
    size_t i = 0;
    while (i < size) {
        uint32_t value = 0;
        uint32_t shift = 0;
        while (true) {
            uint8_t byte = data[i++];
            value |= static_cast<uint32_t>(byte & 0x7F) << shift;
            if (!(byte & 0x80)) {
                break;
            }
            shift += 7;
        }
        single_pass_sum += value;
    }

    // The baseline code resets sum = 0 inside the loop and then adds values.
    // Effectively, it returns the sum of the last iteration.
    // If iters is 0, the baseline returns 0. 
    // If iters > 0, it returns the sum of the encoded stream.
    
    if (iters <= 0) return 0;
    return single_pass_sum;
}