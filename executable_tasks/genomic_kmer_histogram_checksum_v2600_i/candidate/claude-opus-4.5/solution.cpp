#include "interface.h"

#include <cstdint>
#include <string>
#include <cstring>

namespace {

alignas(64) constexpr uint8_t BASE_TO_BITS[256] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,1,0,0,0,2,0,0,0,0,0,0,0,0, // A=0, C=1, G=2
    0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0, // T=3
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

}  // namespace

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
    const size_t n = dna.size();
    if (n < static_cast<size_t>(k)) {
        uint64_t hash = 1469598103934665603ULL;
        const size_t bins = 1u << (2 * k);
        for (size_t i = 0; i < bins; ++i) {
            hash *= 1099511628211ULL;
        }
        return hash;
    }
    
    const size_t bins = 1u << (2 * k);
    const uint32_t mask = static_cast<uint32_t>(bins - 1);
    const char* data = dna.data();
    const size_t num_kmers = n - static_cast<size_t>(k) + 1;
    
    uint64_t hash = 0;
    
    // Specialized path for k=4 (256 bins, fits in cache)
    if (k == 4) {
        alignas(64) uint32_t counts[256];
        
        for (int iter = 0; iter < iters; ++iter) {
            std::memset(counts, 0, sizeof(counts));
            
            // Build initial k-mer
            uint32_t kmer = 0;
            for (int i = 0; i < 4; ++i) {
                kmer = (kmer << 2) | BASE_TO_BITS[static_cast<unsigned char>(data[i])];
            }
            counts[kmer]++;
            
            // Rolling hash for remaining k-mers
            for (size_t i = 1; i < num_kmers; ++i) {
                kmer = ((kmer << 2) | BASE_TO_BITS[static_cast<unsigned char>(data[i + 3])]) & 0xFF;
                counts[kmer]++;
            }
            
            // Compute checksum
            hash = 1469598103934665603ULL;
            for (size_t i = 0; i < 256; ++i) {
                hash ^= static_cast<uint64_t>(counts[i]);
                hash *= 1099511628211ULL;
            }
        }
    } else {
        // General path for other k values
        uint32_t* counts = new uint32_t[bins];
        
        for (int iter = 0; iter < iters; ++iter) {
            std::memset(counts, 0, bins * sizeof(uint32_t));
            
            // Build initial k-mer
            uint32_t kmer = 0;
            for (int i = 0; i < k; ++i) {
                kmer = (kmer << 2) | BASE_TO_BITS[static_cast<unsigned char>(data[i])];
            }
            counts[kmer]++;
            
            // Rolling hash for remaining k-mers
            for (size_t i = 1; i < num_kmers; ++i) {
                kmer = ((kmer << 2) | BASE_TO_BITS[static_cast<unsigned char>(data[i + k - 1])]) & mask;
                counts[kmer]++;
            }
            
            // Compute checksum
            hash = 1469598103934665603ULL;
            for (size_t i = 0; i < bins; ++i) {
                hash ^= static_cast<uint64_t>(counts[i]);
                hash *= 1099511628211ULL;
            }
        }
        
        delete[] counts;
    }
    
    return hash;
}