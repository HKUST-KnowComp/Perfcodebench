#include "interface.h"
#include <vector>
#include <cstddef>
#include <cstdint>
#include <cstring>

static inline uint64_t mix(uint64_t h,uint64_t v){
    h^=v;
    h*=1099511628211ULL;
    return h;
}

uint64_t sparse_accumulator_hash(
    const std::vector<uint32_t>& indices,
    const std::vector<uint32_t>& values,
    uint32_t domain,
    int iters
){
    static thread_local uint64_t pool[8192];
    static thread_local bool init_pool=false;
    if(!init_pool){
        std::memset(pool,0,sizeof(pool));
        init_pool=true;
    }
    const size_t S=indices.size();
    uint64_t hash=14695981039346656037ULL;
    for(int it=0;it<iters;++it){
        for(size_t i=0;i<S;++i){
            pool[indices[i]]+=values[i];
        }
        for(uint32_t k=0;k<domain;++k){
            const uint64_t v=pool[k];
            hash=mix(hash,v);
            pool[k]=0;
        }
    }
    return hash;