#include "interface.h"

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    if (iters <= 0) return 0;

    const std::size_t n = packed.size();
    const std::size_t num_full_triplets = n / 3;
    std::vector<uint32_t> decoded(num_full_triplets * 2);

    const uint8_t* data = packed.data();
    uint32_t* out = decoded.data();

    for (std::size_t i = 0; i < num_full_triplets; ++i) {
        std::size_t idx = i * 3;
        const uint8_t b0 = data[idx];
        const uint8_t b1 = data[idx + 1];
        const uint8_t b2 = data[idx + 2];

        out[i * 2] = static_cast<uint32_t>(b0) | ((static_cast<uint32_t>(b1 & 0x0F)) << 8);
        out[i * 2 + 1] = (static_cast<uint32_t>(b1 >> 4)) | (static_cast<uint32_t>(b2) << 4);
    }

    const uint32_t threshold32 = static_cast<uint32_t>(threshold);
    uint64_t sum = 0;

    for (std::size_t j = 0; j < decoded.size(); ++j) {
        uint32_t v = decoded[j];
        if (v > threshold32) {
            sum += static_cast<uint64_t>(v);
        }
    }

    return sum;
}
