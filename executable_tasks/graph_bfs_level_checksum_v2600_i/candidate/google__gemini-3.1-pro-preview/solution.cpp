#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

namespace {

/**
 * Computes the FNV-1a hash of the levels array.
 * The level array uses -1 for unvisited nodes.
 * The checksum logic: hash ^= (level + 1); hash *= FNV_PRIME;
 */
inline uint64_t checksum_levels(const std::vector<int32_t>& levels) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;
    for (int32_t level : levels) {
        hash ^= static_cast<uint64_t>(static_cast<uint32_t>(level + 1));
        hash *= prime;
    }
    return hash;
}

}  // namespace

uint64_t bfs_level_checksum(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& edges,
    uint32_t source,
    int iters) {
    
    if (offsets.empty()) return 0;
    const uint32_t n = static_cast<uint32_t>(offsets.size() - 1u);
    if (source >= n) return 0;

    std::vector<int32_t> levels(n);
    std::vector<uint32_t> current_frontier;
    std::vector<uint32_t> next_frontier;
    
    // Pre-allocate to avoid repeated reallocations
    current_frontier.reserve(n);
    next_frontier.reserve(n);

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        // Reset levels to -1
        std::fill(levels.begin(), levels.end(), -1);
        
        current_frontier.clear();
        next_frontier.clear();

        // Initial state
        levels[source] = 0;
        current_frontier.push_back(source);

        int32_t current_depth = 0;
        
        while (!current_frontier.empty()) {
            int32_t next_depth = current_depth + 1;
            
            for (uint32_t u : current_frontier) {
                const uint32_t start = offsets[u];
                const uint32_t end = offsets[u + 1];
                
                for (uint32_t i = start; i < end; ++i) {
                    uint32_t v = edges[i];
                    if (levels[v] == -1) {
                        levels[v] = next_depth;
                        next_frontier.push_back(v);
                    }
                }
            }
            
            current_frontier.swap(next_frontier);
            next_frontier.clear();
            current_depth = next_depth;
        }
        
        hash = checksum_levels(levels);
    }

    return hash;
}