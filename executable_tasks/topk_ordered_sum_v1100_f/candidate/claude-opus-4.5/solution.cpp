#include "interface.h"

#include <algorithm>
#include <cstdint>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
    const size_t n = scores.size();
    if (n == 0 || k <= 0) {
        return 0;
    }
    
    const size_t kk = static_cast<size_t>(k);
    const size_t actual_k = (kk > n) ? n : kk;
    
    std::vector<uint32_t> tmp(n);
    uint64_t sum = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        // Copy scores to tmp
        std::copy(scores.begin(), scores.end(), tmp.begin());
        
        // Use nth_element to partition: elements before position (actual_k-1) 
        // are >= the element at that position (using greater comparator)
        // This gives us the top k elements in tmp[0..actual_k-1]
        std::nth_element(tmp.begin(), tmp.begin() + static_cast<ptrdiff_t>(actual_k), 
                         tmp.end(), std::greater<uint32_t>());
        
        // Sum the first actual_k elements (they are the top k)
        sum = 0;
        const uint32_t* ptr = tmp.data();
        for (size_t i = 0; i < actual_k; ++i) {
            sum += ptr[i];
        }
    }
    
    return sum;
}