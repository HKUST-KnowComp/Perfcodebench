#include "interface.h"

#include <immintrin.h>
#include <cstdint>
#include <cstddef>

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters)
{
    (void)ends;
    const uint32_t*  s = starts.data();
    const uint64_t*  p = reinterpret_cast<const uint64_t*>(payloads.data());
    const uint32_t*  pr= probes.data();
    const uint32_t*  w = weights.data();
    const size_t     n = starts.size();
    const size_t     m = probes.size();

    uint64_t sum = 0;
    for(int it=0; it<iters; ++it){
        uint64_t local=0;
        size_t i=0;
        /* unroll 8x to hide latency and expose ILP */
        for(; i+8<=m; i+=8){
            uint32_t q0 = pr[i+0];
            uint32_t q1 = pr[i+1];
            uint32_t q2 = pr[i+2];
            uint32_t q3 = pr[i+3];
            uint32_t q4 = pr[i+4];
            uint32_t q5 = pr[i+5];
            uint32_t q6 = pr[i+6];
            uint32_t q7 = pr[i+7];

            /* binary search for q*; identical code for each */
            size_t lo=0, hi=n;
            while(lo<hi){
                size_t mid=(lo+hi)>>1;
                if(s[mid]<=q0) lo=mid+1; else hi=mid;
            }
            uint64_t idx0 = lo-1;

            lo=0; hi=n;
            while(lo<hi){
                size_t mid=(lo+hi)>>1;
                if(s[mid]<=q1) lo=mid+1; else hi=mid;
            }
            uint64_t idx1 = lo-1;

            lo=0; hi=n;
            while(lo<hi){
                size_t mid=(lo+hi)>>1;
                if(s[mid]<=q2) lo=mid+1; else hi=mid;
            }
            uint64_t idx2 = lo-1;

            lo=0; hi=n;
            while(lo<hi){
                size_t mid=(lo+hi)>>1;
                if(s[mid]<=q3) lo=mid+1; else hi=mid;
            }
            uint64_t idx3 = lo-1;

            lo=0; hi=n;
            while(lo<hi){
                size_t mid=(lo+hi)>>1;
                if(s[mid]<=q4) lo=mid+1; else hi=mid;
            }
            uint64_t idx4 = lo-1;

            lo=0; hi=n;
            while(lo<hi){
                size_t mid=(lo+hi)>>1;
                if(s[mid]<=q5) lo=mid+1; else hi=mid;
            }
            uint64_t idx5 = lo-1;

            lo=0; hi=n;
            while(lo<hi){
                size_t mid=(lo+hi)>>1;
                if(s[mid]<=q6) lo=mid+1; else hi=mid;
            }
            uint64_t idx6 = lo-1;

            lo=0; hi=n;
            while(lo<hi){
                size_t mid=(lo+hi)>>1;
                if(s[mid]<=q7) lo=mid+1; else hi=mid;
            }
            uint64_t idx7 = lo-1;

            local += p[idx0]*w[i+0];
            local += p[idx1]*w[i+1];
            local += p[idx2]*w[i+2];
            local += p[idx3]*w[i+3];
            local += p[idx4]*w[i+4];
            local += p[idx5]*w[i+5];
            local += p[idx6]*w[i+6];
            local += p[idx7]*w[i+7];
        }
        /* tail */
        for(;i<m;++i){
            uint32_t q = pr[i];
            size_t lo=0, hi=n;
            while(lo<hi){
                size_t mid=(lo+hi)>>1;
                if(s[mid]<=q) lo=mid+1; else hi=mid;
            }
            local += p[lo-1]*w[i];
        }
        sum = local;
    }
    return sum;
}