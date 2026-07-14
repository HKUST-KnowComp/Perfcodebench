#include "interface.h"

#include <cstdint>
#include <cstdlib>
#include <vector>
#include <omp.h>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    const int32_t* __restrict__ px = xs.data();
    const int32_t* __restrict__ py = ys.data();
    const std::size_t n = xs.size();
    int64_t total = 0;

    for (int iter = 0; iter < iters; ++iter) {
        int64_t sum = 0;

        #pragma omp parallel reduction(+:sum)
        {
            #pragma omp for schedule(dynamic, 64) nowait
            for (std::size_t i = 0; i < n; ++i) {
                const int32_t xi = px[i];
                const int32_t yi = py[i];
                int64_t local_sum = 0;
                std::size_t j = i + 1;
                
                // Process 4 elements at a time
                const std::size_t end4 = i + 1 + ((n - i - 1) & ~3ULL);
                for (; j < end4; j += 4) {
                    int32_t dx0 = xi - px[j];
                    int32_t dy0 = yi - py[j];
                    int32_t dx1 = xi - px[j+1];
                    int32_t dy1 = yi - py[j+1];
                    int32_t dx2 = xi - px[j+2];
                    int32_t dy2 = yi - py[j+2];
                    int32_t dx3 = xi - px[j+3];
                    int32_t dy3 = yi - py[j+3];
                    
                    local_sum += static_cast<int64_t>(std::abs(dx0)) + static_cast<int64_t>(std::abs(dy0))
                               + static_cast<int64_t>(std::abs(dx1)) + static_cast<int64_t>(std::abs(dy1))
                               + static_cast<int64_t>(std::abs(dx2)) + static_cast<int64_t>(std::abs(dy2))
                               + static_cast<int64_t>(std::abs(dx3)) + static_cast<int64_t>(std::abs(dy3));
                }
                
                // Handle remaining elements
                for (; j < n; ++j) {
                    local_sum += static_cast<int64_t>(std::abs(xi - px[j])) +
                                 static_cast<int64_t>(std::abs(yi - py[j]));
                }
                
                sum += local_sum;
            }
        }
        total = sum;
    }
    return total;
}