#include "interface.h"
#include <vector>
#include <cstring>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

// Fast reset of 32-bit buffer using std::fill
void reset_counts(std::vector<uint32_t>& buffer, size_t bins) {
    std::fill(buffer.begin(), buffer.begin() + bins, 0u);
}

} // anonymous namespace

uint64_t histogram_checksum(const std::vector<uint32_t>& values,uint32_t bins,int iters){
    // reuse one dense buffer for all iterations (perfect match for dense histogram)
    std::vector<uint32_t> counts_data(bins);
    for(int iter=0;iter<iters;++iter){
        // zero accumulated counters
        reset_counts(counts_data,bins);
        // bin population  
        for(uint32_t v:values){
            ++counts_data[v]; // v < bins by task spec => dense range and no bounds check
        }
        // checksum calculation
        uint64_t hash = 1469598103934665603ULL;
        for(uint32_t b=0;b<bins;++b){
            hash=mix(hash,counts_data[b]);
        }
    }
    return hash;
}