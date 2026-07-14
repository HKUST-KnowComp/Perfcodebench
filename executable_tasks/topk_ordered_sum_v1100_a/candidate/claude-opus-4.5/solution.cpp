#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
    const std::size_t n = scores.size();
    const std::size_t kk = static_cast<std::size_t>(k);
    
    if (n == 0 || k <= 0) return 0;
    
    uint64_t sum = 0;
    
    // For small k relative to n, partial_sort_copy avoids full copy overhead
    if (kk <= 64 && kk * 16 < n) {
        std::vector<uint32_t> topk(kk);
        for (int iter = 0; iter < iters; ++iter) {
            std::partial_sort_copy(scores.begin(), scores.end(),
                                   topk.begin(), topk.end(),
                                   std::greater<uint32_t>());
            sum = 0;
            for (std::size_t i = 0; i < kk; ++i) {
                sum += topk[i];
            }
        }
    } else {
        // Use nth_element for O(n) partitioning
        std::vector<uint32_t> tmp;
        tmp.reserve(n);
        
        for (int iter = 0; iter < iters; ++iter) {
            tmp.assign(scores.begin(), scores.end());
            
            if (kk < n) {
                std::nth_element(tmp.begin(), tmp.begin() + static_cast<std::ptrdiff_t>(kk), 
                                 tmp.end(), std::greater<uint32_t>());
            }
            
            sum = 0;
            const uint32_t* ptr = tmp.data();
            
            // Prefetch for better cache behavior
            if (kk >= 16) {
                __builtin_prefetch(ptr + 16, 0, 0);
            }
            
            // Unrolled summation for better ILP
            std::size_t i = 0;
            uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
            
            for (; i + 4 <= kk; i += 4) {
                if (i + 20 < kk) {
                    __builtin_prefetch(ptr + i + 20, 0, 0);
                }
                s0 += ptr[i];
                s1 += ptr[i + 1];
                s2 += ptr[i + 2];
                s3 += ptr[i + 3];
            }
            
            sum = s0 + s1 + s2 + s3;
            
            for (; i < kk; ++i) {
                sum += ptr[i];
            }
        }
    }
    
    return sum;
}
