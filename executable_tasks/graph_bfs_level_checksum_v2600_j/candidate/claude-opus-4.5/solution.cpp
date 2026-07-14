#include "interface.h"

#include <algorithm>
#include <atomic>
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
    
    const uint32_t* __restrict off_ptr = offsets.data();
    const uint32_t* __restrict edge_ptr = edges.data();
    
    std::vector<int32_t> levels(n);
    int32_t* __restrict lev_ptr = levels.data();
    
    // Frontier buffers - use two for ping-pong
    std::vector<uint32_t> frontier_buf1, frontier_buf2;
    frontier_buf1.reserve(n);
    frontier_buf2.reserve(n);
    
    // Bitmap for visited tracking in bottom-up phase
    const uint32_t bitmap_size = (n + 63) / 64;
    std::vector<uint64_t> visited_bitmap(bitmap_size);
    std::vector<uint64_t> frontier_bitmap(bitmap_size);
    
    uint64_t hash = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        // Reset levels to -1
        std::memset(lev_ptr, 0xFF, n * sizeof(int32_t));
        
        // Reset bitmaps
        std::memset(visited_bitmap.data(), 0, bitmap_size * sizeof(uint64_t));
        std::memset(frontier_bitmap.data(), 0, bitmap_size * sizeof(uint64_t));
        
        // Initialize source
        lev_ptr[source] = 0;
        visited_bitmap[source / 64] |= (1ULL << (source % 64));
        frontier_bitmap[source / 64] |= (1ULL << (source % 64));
        
        frontier_buf1.clear();
        frontier_buf1.push_back(source);
        
        uint32_t* frontier = frontier_buf1.data();
        uint32_t* next_frontier = frontier_buf2.data();
        uint32_t frontier_size = 1;
        uint32_t next_size = 0;
        
        int32_t depth = 0;
        
        // Count edges in frontier for direction optimization
        uint64_t total_edges = edges.size();
        uint64_t frontier_edges = off_ptr[source + 1] - off_ptr[source];
        uint64_t unvisited_count = n - 1;
        
        // Threshold for switching to bottom-up (alpha parameter)
        const double alpha = 14.0;
        const double beta = 24.0;
        
        bool use_bottom_up = false;
        
        while (frontier_size > 0) {
            ++depth;
            next_size = 0;
            
            // Decide direction based on frontier size
            // Switch to bottom-up when frontier is large relative to unvisited vertices
            bool should_bottom_up = (frontier_edges > unvisited_count / alpha);
            bool should_top_down = (frontier_size < n / beta);
            
            if (should_bottom_up && !should_top_down && unvisited_count > 0) {
                use_bottom_up = true;
            } else if (should_top_down) {
                use_bottom_up = false;
            }
            
            if (!use_bottom_up) {
                // Top-down BFS
                frontier_buf2.clear();
                
                for (uint32_t i = 0; i < frontier_size; ++i) {
                    uint32_t u = frontier[i];
                    uint32_t start = off_ptr[u];
                    uint32_t end = off_ptr[u + 1];
                    
                    for (uint32_t idx = start; idx < end; ++idx) {
                        uint32_t v = edge_ptr[idx];
                        if (lev_ptr[v] == -1) {
                            lev_ptr[v] = depth;
                            frontier_buf2.push_back(v);
                            
                            // Update bitmap
                            uint32_t word = v / 64;
                            uint64_t bit = 1ULL << (v % 64);
                            visited_bitmap[word] |= bit;
                        }
                    }
                }
                
                next_size = static_cast<uint32_t>(frontier_buf2.size());
                
                // Update frontier bitmap for potential bottom-up switch
                std::memset(frontier_bitmap.data(), 0, bitmap_size * sizeof(uint64_t));
                for (uint32_t i = 0; i < next_size; ++i) {
                    uint32_t v = frontier_buf2[i];
                    frontier_bitmap[v / 64] |= (1ULL << (v % 64));
                }
                
                // Swap buffers
                std::swap(frontier_buf1, frontier_buf2);
                frontier = frontier_buf1.data();
                
            } else {
                // Bottom-up BFS
                std::memset(frontier_bitmap.data(), 0, bitmap_size * sizeof(uint64_t));
                frontier_buf2.clear();
                
                for (uint32_t v = 0; v < n; ++v) {
                    if (lev_ptr[v] != -1) continue;
                    
                    uint32_t start = off_ptr[v];
                    uint32_t end = off_ptr[v + 1];
                    
                    for (uint32_t idx = start; idx < end; ++idx) {
                        uint32_t u = edge_ptr[idx];
                        if (lev_ptr[u] == depth - 1) {
                            lev_ptr[v] = depth;
                            frontier_buf2.push_back(v);
                            
                            uint32_t word = v / 64;
                            uint64_t bit = 1ULL << (v % 64);
                            visited_bitmap[word] |= bit;
                            frontier_bitmap[word] |= bit;
                            break;
                        }
                    }
                }
                
                next_size = static_cast<uint32_t>(frontier_buf2.size());
                std::swap(frontier_buf1, frontier_buf2);
                frontier = frontier_buf1.data();
            }
            
            frontier_size = next_size;
            unvisited_count -= frontier_size;
            
            // Count edges for next iteration's direction decision
            frontier_edges = 0;
            for (uint32_t i = 0; i < frontier_size; ++i) {
                uint32_t v = frontier[i];
                frontier_edges += off_ptr[v + 1] - off_ptr[v];
            }
        }
        
        hash = checksum_levels(lev_ptr, n);
    }
    
    return hash;
}