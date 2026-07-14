#include "interface.h"

#include <cstdint>
#include <cstdlib>
#include <vector>
#include <omp.h>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    const int64_t n = static_cast<int64_t>(xs.size());
    if (n <= 1) return 0;
    
    const int32_t* __restrict px = xs.data();
    const int32_t* __restrict py = ys.data();
    
    int64_t total = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        int64_t sum = 0;
        
        #pragma omp parallel reduction(+:sum)
        {
            #pragma omp for schedule(dynamic, 64) nowait
            for (int64_t i = 0; i < n - 1; ++i) {
                const int32_t xi = px[i];
                const int32_t yi = py[i];
                int64_t local_sum = 0;
                
                int64_t j = i + 1;
                
                // Process 4 elements at a time for better vectorization
                const int64_t end4 = i + 1 + ((n - i - 1) & ~3LL);
                
                #pragma omp simd reduction(+:local_sum)
                for (; j < end4; j += 4) {
                    int32_t dx0 = px[j] - xi;
                    int32_t dy0 = py[j] - yi;
                    int32_t dx1 = px[j+1] - xi;
                    int32_t dy1 = py[j+1] - yi;
                    int32_t dx2 = px[j+2] - xi;
                    int32_t dy2 = py[j+2] - yi;
                    int32_t dx3 = px[j+3] - xi;
                    int32_t dy3 = py[j+3] - yi;
                    
                    local_sum += static_cast<int64_t>(dx0 < 0 ? -dx0 : dx0) + 
                                 static_cast<int64_t>(dy0 < 0 ? -dy0 : dy0);
                    local_sum += static_cast<int64_t>(dx1 < 0 ? -dx1 : dx1) + 
                                 static_cast<int64_t>(dy1 < 0 ? -dy1 : dy1);
                    local_sum += static_cast<int64_t>(dx2 < 0 ? -dx2 : dx2) + 
                                 static_cast<int64_t>(dy2 < 0 ? -dy2 : dy2);
                    local_sum += static_cast<int64_t>(dx3 < 0 ? -dx3 : dx3) + 
                                 static_cast<int64_t>(dy3 < 0 ? -dy3 : dy3);
                }
                
                // Handle remainder
                for (; j < n; ++j) {
                    int32_t dx = px[j] - xi;
                    int32_t dy = py[j] - yi;
                    local_sum += static_cast<int64_t>(dx < 0 ? -dx : dx) + 
                                 static_cast<int64_t>(dy < 0 ? -dy : dy);
                }
                
                sum += local_sum;
            }
        }
        
        total = sum;
    }
    
    return total;
}