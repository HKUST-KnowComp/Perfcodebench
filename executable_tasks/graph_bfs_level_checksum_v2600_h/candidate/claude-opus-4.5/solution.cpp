#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <vector>

namespace {

inline uint64_t checksum_levels(const int32_t* levels, uint32_t n) {
    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t i = 0; i < n; ++i) {
        hash ^= static_cast<uint64_t>(static_cast<uint32_t>(levels[i] + 1));
        hash *= 1099511628211ULL;
    }
    return hash;
}

}  // namespace

uint64_t bfs_level_checksum(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& edges,
    uint32_t source,
    int iters) {
    
    const uint32_t n = static_cast<uint32_t>(offsets.size() - 1u);
    if (n == 0) return 1469598103934665603ULL;
    
    const uint32_t m = static_cast<uint32_t>(edges.size());
    const uint32_t* __restrict off_ptr = offsets.data();
    const uint32_t* __restrict edge_ptr = edges.data();
    
    std::vector<int32_t> levels(n);
    int32_t* __restrict lev_ptr = levels.data();
    
    std::vector<uint32_t> frontier;
    std::vector<uint32_t> next_frontier;
    frontier.reserve(n);
    next_frontier.reserve(n);
    
    // Bit vector for frontier membership (for bottom-up)
    const uint32_t bv_size = (n + 63) / 64;
    std::vector<uint64_t> frontier_bits(bv_size);
    uint64_t* __restrict fb_ptr = frontier_bits.data();
    
    uint64_t hash = 0;
    
    // Threshold for switching to bottom-up
    const uint32_t bu_threshold = n / 16;
    
    for (int iter = 0; iter < iters; ++iter) {
        std::memset(lev_ptr, 0xFF, n * sizeof(int32_t)); // -1
        std::memset(fb_ptr, 0, bv_size * sizeof(uint64_t));
        
        frontier.clear();
        frontier.push_back(source);
        lev_ptr[source] = 0;
        fb_ptr[source >> 6] |= (1ULL << (source & 63));
        
        int32_t depth = 0;
        uint64_t edges_in_frontier = off_ptr[source + 1] - off_ptr[source];
        
        while (!frontier.empty()) {
            next_frontier.clear();
            int32_t next_depth = depth + 1;
            
            // Decide direction based on frontier size
            bool use_bottom_up = (frontier.size() > bu_threshold) && (edges_in_frontier > m / 8);
            
            if (!use_bottom_up) {
                // Top-down BFS
                for (uint32_t u : frontier) {
                    uint32_t start = off_ptr[u];
                    uint32_t end = off_ptr[u + 1];
                    for (uint32_t idx = start; idx < end; ++idx) {
                        uint32_t v = edge_ptr[idx];
                        if (lev_ptr[v] == -1) {
                            lev_ptr[v] = next_depth;
                            next_frontier.push_back(v);
                        }
                    }
                }
            } else {
                // Bottom-up BFS
                for (uint32_t v = 0; v < n; ++v) {
                    if (lev_ptr[v] != -1) continue;
                    uint32_t start = off_ptr[v];
                    uint32_t end = off_ptr[v + 1];
                    for (uint32_t idx = start; idx < end; ++idx) {
                        uint32_t u = edge_ptr[idx];
                        if (fb_ptr[u >> 6] & (1ULL << (u & 63))) {
                            lev_ptr[v] = next_depth;
                            next_frontier.push_back(v);
                            break;
                        }
                    }
                }
            }
            
            // Update frontier bits for next iteration
            std::memset(fb_ptr, 0, bv_size * sizeof(uint64_t));
            edges_in_frontier = 0;
            for (uint32_t v : next_frontier) {
                fb_ptr[v >> 6] |= (1ULL << (v & 63));
                edges_in_frontier += off_ptr[v + 1] - off_ptr[v];
            }
            
            frontier.swap(next_frontier);
            ++depth;
        }
        
        hash = checksum_levels(lev_ptr, n);
    }
    
    return hash;
}