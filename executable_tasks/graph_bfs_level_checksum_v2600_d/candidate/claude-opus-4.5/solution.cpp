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
    
    const uint64_t m = edges.size();
    
    std::vector<int32_t> levels(n);
    std::vector<uint32_t> frontier;
    std::vector<uint32_t> next_frontier;
    frontier.reserve(n);
    next_frontier.reserve(n);
    
    // Bitmap for frontier membership (for bottom-up)
    const uint32_t bitmap_size = (n + 63) / 64;
    std::vector<uint64_t> frontier_bitmap(bitmap_size);
    std::vector<uint64_t> visited_bitmap(bitmap_size);
    
    uint64_t hash = 0;
    
    // Threshold for switching to bottom-up
    const uint64_t alpha = 14;  // edges_to_check / remaining_edges threshold
    const uint32_t beta = 24;   // frontier size threshold for switching back to top-down
    
    for (int iter = 0; iter < iters; ++iter) {
        std::memset(levels.data(), -1, n * sizeof(int32_t));
        std::memset(visited_bitmap.data(), 0, bitmap_size * sizeof(uint64_t));
        std::memset(frontier_bitmap.data(), 0, bitmap_size * sizeof(uint64_t));
        
        frontier.clear();
        frontier.push_back(source);
        levels[source] = 0;
        visited_bitmap[source / 64] |= (1ULL << (source % 64));
        frontier_bitmap[source / 64] |= (1ULL << (source % 64));
        
        int32_t depth = 0;
        uint64_t edges_to_check = offsets[source + 1] - offsets[source];
        uint64_t unvisited_count = n - 1;
        bool use_bottom_up = false;
        
        while (!frontier.empty()) {
            next_frontier.clear();
            
            // Decide direction
            // edges_to_check = sum of degrees of frontier nodes
            // Switch to bottom-up when frontier is large relative to unvisited
            uint64_t remaining_edges = 0;
            for (uint32_t i = 0; i < n; ++i) {
                if (levels[i] == -1) {
                    remaining_edges += offsets[i + 1] - offsets[i];
                }
            }
            
            if (!use_bottom_up && edges_to_check > remaining_edges / alpha && frontier.size() > beta) {
                use_bottom_up = true;
            } else if (use_bottom_up && frontier.size() < beta) {
                use_bottom_up = false;
            }
            
            if (use_bottom_up) {
                // Bottom-up BFS: iterate over unvisited nodes
                for (uint32_t v = 0; v < n; ++v) {
                    if (levels[v] != -1) continue;
                    
                    // Check if any neighbor is in frontier
                    bool found = false;
                    for (uint32_t idx = offsets[v]; idx < offsets[v + 1]; ++idx) {
                        uint32_t u = edges[idx];
                        if ((frontier_bitmap[u / 64] >> (u % 64)) & 1) {
                            found = true;
                            break;
                        }
                    }
                    
                    if (found) {
                        levels[v] = depth + 1;
                        visited_bitmap[v / 64] |= (1ULL << (v % 64));
                        next_frontier.push_back(v);
                    }
                }
            } else {
                // Top-down BFS: iterate over frontier nodes
                for (uint32_t u : frontier) {
                    for (uint32_t idx = offsets[u]; idx < offsets[u + 1]; ++idx) {
                        uint32_t v = edges[idx];
                        if (levels[v] == -1) {
                            levels[v] = depth + 1;
                            visited_bitmap[v / 64] |= (1ULL << (v % 64));
                            next_frontier.push_back(v);
                        }
                    }
                }
                
                // Remove duplicates from next_frontier
                std::sort(next_frontier.begin(), next_frontier.end());
                next_frontier.erase(std::unique(next_frontier.begin(), next_frontier.end()), next_frontier.end());
                
                // Fix levels for duplicates that were added multiple times
                // (they all got the same level, so this is fine)
            }
            
            // Update frontier bitmap
            std::memset(frontier_bitmap.data(), 0, bitmap_size * sizeof(uint64_t));
            edges_to_check = 0;
            for (uint32_t v : next_frontier) {
                frontier_bitmap[v / 64] |= (1ULL << (v % 64));
                edges_to_check += offsets[v + 1] - offsets[v];
            }
            
            unvisited_count -= next_frontier.size();
            frontier.swap(next_frontier);
            ++depth;
        }
        
        hash = checksum_levels(levels.data(), n);
    }
    
    return hash;
}