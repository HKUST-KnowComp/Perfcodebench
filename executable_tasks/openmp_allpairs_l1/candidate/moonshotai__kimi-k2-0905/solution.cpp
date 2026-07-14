#include "interface.h"
#include <omp.h>
#include <vector>
#include <cstdint>
#include <algorithm>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs,
                        const std::vector<int32_t>& ys,
                        int iters) {
    const std::size_t n = xs.size();
    if (n == 0) return 0;

    // Determine coordinate range
    int32_t minx = xs[0], maxx = xs[0];
    int32_t miny = ys[0], maxy = ys[0];
    #pragma omp parallel for reduction(min:minx,miny) reduction(max:maxx,maxy)
    for (std::size_t i = 0; i < n; ++i) {
        minx = std::min(minx, xs[i]);
        maxx = std::max(maxx, xs[i]);
        miny = std::min(miny, ys[i]);
        maxy = std::max(maxy, ys[i]);
    }
    const int32_t rangex = maxx - minx + 1;
    const int32_t rangey = maxy - miny + 1;

    // Allocate histograms
    std::vector<int64_t> histx(rangex, 0);
    std::vector<int64_t> histy(rangey, 0);

    int64_t total = 0;
    for (int iter = 0; iter < iters; ++iter) {
        // Clear histograms
        std::fill(histx.begin(), histx.end(), 0);
        std::fill(histy.begin(), histy.end(), 0);

        // Build histograms in parallel
        #pragma omp parallel
        {
            std::vector<int64_t> localx(rangex, 0);
            std::vector<int64_t> localy(rangey, 0);
            #pragma omp for nowait
            for (std::size_t i = 0; i < n; ++i) {
                localx[xs[i] - minx]++;
                localy[ys[i] - miny]++;
            }
            #pragma omp critical
            {
                for (int32_t v = 0; v < rangex; ++v) histx[v] += localx[v];
                for (int32_t v = 0; v < rangey; ++v) histy[v] += localy[v];
            }
        }

        // Compute L1 contribution via prefix sums
        int64_t sumx = 0, sumy = 0;
        int64_t cntx = 0, cnty = 0;
        #pragma omp parallel for reduction(+:sumx,cntx)
        for (int32_t v = 0; v < rangex; ++v) {
            sumx += histx[v] * static_cast<int64_t>(v);
            cntx += histx[v];
        }
        #pragma omp parallel for reduction(+:sumy,cnty)
        for (int32_t v = 0; v < rangey; ++v) {
            sumy += histy[v] * static_cast<int64_t>(v);
            cnty += histy[v];
        }

        int64_t partx = 0, party = 0;
        int64_t prefix = 0;
        #pragma omp parallel for reduction(+:partx)
        for (int32_t v = 0; v < rangex; ++v) {
            prefix += histx[v];
            partx += prefix * static_cast<int64_t>(v);
        }
        prefix = 0;
        #pragma omp parallel for reduction(+:party)
        for (int32_t v = 0; v < rangey; ++v) {
            prefix += histy[v];
            party += prefix * static_cast<int64_t>(v);
        }

        total = (sumx * cntx - 2 * partx) + (sumy * cnty - 2 * party);
    }
    return total;
}