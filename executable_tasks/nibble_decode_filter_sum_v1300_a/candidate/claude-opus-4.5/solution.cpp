#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
    const uint8_t* data = packed.data();
    const std::size_t n = packed.size();
    uint64_t sum = 0;

    // Precompute constants for SWAR
    const uint64_t mask_lo = 0x0F0F0F0F0F0F0F0FULL;
    const uint64_t ones    = 0x0101010101010101ULL;
    // For comparison: we want nibble > threshold, i.e., nibble >= threshold+1
    // Using saturating subtraction trick: if nibble >= (threshold+1), then (nibble - (threshold+1)) doesn't underflow
    // We'll use a different approach: compare via (threshold - nibble) and check sign bit
    // Actually simpler: nibble > threshold <=> nibble - threshold - 1 >= 0 when nibble in [0,15]
    // We'll broadcast threshold into each byte and use subtraction with borrow detection.
    
    // For each nibble n in [0,15], we want to keep it if n > threshold.
    // We'll compute (n + (15 - threshold)) and check if it overflows the nibble (>= 16).
    // If n > threshold, then n + (15 - threshold) >= 16.
    // The overflow bit (bit 4 of each nibble position) indicates n > threshold.
    
    const uint8_t add_val = 15 - threshold; // This works for threshold in [0,14]. For threshold=15, add_val=0, and no nibble > 15.
    const uint64_t add_broadcast = add_val * ones;
    const uint64_t overflow_mask = 0x1010101010101010ULL; // bit 4 of each nibble pair

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t local_sum = 0;
        std::size_t i = 0;

        // Process 8 bytes at a time (16 nibbles)
        const std::size_t n8 = n & ~7ULL;
        for (; i < n8; i += 8) {
            uint64_t chunk;
            // Load 8 bytes
            const uint8_t* p = data + i;
            chunk = static_cast<uint64_t>(p[0])
                  | (static_cast<uint64_t>(p[1]) << 8)
                  | (static_cast<uint64_t>(p[2]) << 16)
                  | (static_cast<uint64_t>(p[3]) << 24)
                  | (static_cast<uint64_t>(p[4]) << 32)
                  | (static_cast<uint64_t>(p[5]) << 40)
                  | (static_cast<uint64_t>(p[6]) << 48)
                  | (static_cast<uint64_t>(p[7]) << 56);

            // Extract low nibbles (even indices in output)
            uint64_t lo = chunk & mask_lo;
            // Extract high nibbles (odd indices in output)
            uint64_t hi = (chunk >> 4) & mask_lo;

            // For low nibbles: check which are > threshold
            // Add (15 - threshold) to each nibble; if result >= 16, the nibble was > threshold
            uint64_t lo_add = lo + add_broadcast;
            uint64_t lo_overflow = lo_add & overflow_mask; // non-zero in positions where nibble > threshold
            // Create mask: if overflow bit is set, we want to keep the nibble
            // Shift overflow bit down and subtract 1 to create 0x0F mask, or 0 if no overflow
            // Actually, let's use a different approach: spread the overflow bit to mask the nibble
            // lo_overflow has bit 4 set for each byte where low nibble > threshold
            // We want a mask that is 0x0F where bit 4 is set, 0x00 otherwise
            // (lo_overflow >> 4) gives 0x01 where condition is true
            // ((lo_overflow >> 4) * 0x0F) doesn't work directly...
            // Let's use: mask = (lo_overflow >> 4) - ((lo_overflow >> 4) >> 4) ... no
            // Simpler: mask = lo_overflow - (lo_overflow >> 4) = 0x10 - 0x01 = 0x0F where set
            uint64_t lo_mask = lo_overflow - (lo_overflow >> 4);
            uint64_t lo_filtered = lo & lo_mask;

            // Same for high nibbles
            uint64_t hi_add = hi + add_broadcast;
            uint64_t hi_overflow = hi_add & overflow_mask;
            uint64_t hi_mask = hi_overflow - (hi_overflow >> 4);
            uint64_t hi_filtered = hi & hi_mask;

            // Sum all bytes in lo_filtered and hi_filtered
            // Each byte contains a value 0-15, sum of 16 such values fits in 16 bits easily
            // Use horizontal sum via multiplication trick
            // Sum bytes: (x * 0x0101010101010101) >> 56 gives sum of all 8 bytes
            uint64_t lo_sum = ((lo_filtered * ones) >> 56) & 0xFF;
            uint64_t hi_sum = ((hi_filtered * ones) >> 56) & 0xFF;
            // Actually the multiply-shift gives the sum in the top byte, but we need to be careful
            // Let's verify: if lo_filtered = 0x0102030405060708, then
            // lo_filtered * 0x0101010101010101 = sum appears in top byte
            // This works for bytes, but we have nibbles stored in bytes (values 0-15)
            // The sum of 8 bytes each 0-15 is at most 120, fits in a byte.
            // The multiply trick: (a0 + a1*256 + ... + a7*256^7) * (1 + 256 + ... + 256^7)
            // The coefficient of 256^7 in the product is a0 + a1 + ... + a7 (mod 256)
            // Since max sum is 120 < 256, no overflow, so this works.
            
            local_sum += lo_sum + hi_sum;
        }

        // Scalar tail
        for (; i < n; ++i) {
            uint8_t byte = data[i];
            uint8_t lo = byte & 0x0F;
            uint8_t hi = byte >> 4;
            if (lo > threshold) local_sum += lo;
            if (hi > threshold) local_sum += hi;
        }

        sum = local_sum;
    }

    return sum;
}
