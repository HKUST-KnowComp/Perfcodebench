#include "interface.h"
#include <vector>
#include <cstdint>
#include <algorithm>
#include <omp.h>

namespace{

uint64_t checksum_u32(const std::vector<uint32_t>& data){
    const std::size_t n = data.size();
    const std::size_t vl = (n/16)*16;
    uint64_t hash = 1469598103934665603ULL;
    std::size_t i=0;
    for(; i<vl; i+=16){
        for(std::size_t j=0;j<16;++j){
            hash ^= static_cast<uint64_t>(data[i+j]);
            hash *= 1099511628211ULL;
        }
    }
    for(; i<n; ++i){
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}

}

uint64_t jacobi_checksum(const std::vector<uint32_t>& input, int rows, int cols, int steps){
    const std::size_t r = static_cast<std::size_t>(rows);
    const std::size_t c = static_cast<std::size_t>(cols);
    const std::size_t stride = c + 2;               // includes left+right halo

    std::vector<uint32_t> a( (r+2)*stride, 0u );
    std::vector<uint32_t> b( (r+2)*stride, 0u );

    #pragma omp parallel for schedule(static)
    for(std::size_t i=0;i<r;++i){
        const uint32_t* src = input.data() + i*c;
        uint32_t* dst = a.data() + (i+1)*stride + 1;
        std::memcpy(dst, src, c*sizeof(uint32_t));
    }

    for(int step=0; step<steps; ++step){
        #pragma omp parallel for schedule(static)
        for(std::ptrdiff_t rr=1; rr<static_cast<std::ptrdiff_t>(r+1); ++rr){
            const uint32_t* __restrict prev = a.data() + (rr-1)*stride;
            const uint32_t* __restrict cur  = a.data() +  rr   *stride;
            const uint32_t* __restrict next = a.data() + (rr+1)*stride;
            uint32_t* __restrict out      = b.data() +  rr   *stride;

            #pragma simd
            for(std::size_t cc=1; cc<=c; ++cc){
                out[cc] = (cur[cc]*4u + cur[cc-1] + cur[cc+1] + prev[cc] + next[cc]) / 8u;
            }
        }
        std::swap(a,b);
    }

    std::vector<uint32_t> final(r*c);
    #pragma omp parallel for schedule(static)
    for(std::size_t i=0;i<r;++i){
        std::memcpy(final.data()+i*c, a.data()+(i+1)*stride+1, c*sizeof(uint32_t));
    }
    return checksum_u32(final);
}