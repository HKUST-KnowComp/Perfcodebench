#include "interface.h"

#include <cstdint>
#include <cstdlib>
#include <vector>
#include <omp.h>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    if (iters <= 0) return 0;
    
    const std::size_t n = xs.size();
    if (n < 2) return 0;
    
    const int32_t* __restrict px = xs.data();
    const int32_t* __restrict py = ys.data();
    
    int64_t sum = 0;
    
    #pragma omp parallel reduction(+:sum)
    {
        #pragma omp for schedule(static)
        for (std::size_t i = 0; i < n; ++i) {
            int32_t xi = px[i];
            int32_t yi = py[i];
            int64_t local_sum = 0;
            for (std::size_t j = i + 1; j < n; ++j) {
                int32_t dx = px[j] - xi;
                int32_t dy = py[j] - yi;
                local_sum += static_cast<int64_t>((dx >= 0 ? dx : -dx)) +
                             static_cast<int64_t>((dy >= 0 ? dy : -dy));
            }
            sum += local_sum;
        }
    }
    
    return sum;
}