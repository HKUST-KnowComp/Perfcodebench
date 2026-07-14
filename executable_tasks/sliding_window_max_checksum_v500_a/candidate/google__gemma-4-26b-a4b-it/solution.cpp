#include "interface.h"
#include <vector>
#include <deque>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

// Using a fixed-size buffer for the deque to avoid dynamic allocations inside the loop.
// The deque stores indices of elements in the current window.
struct FastDeque {
    int* data;
    int head;
    int tail;

    void init(int* buffer) {
        data = buffer;
        head = 0;
        tail = 0;
    }

    inline void clear() {
        head = 0;
        tail = 0;
    }

    inline bool empty() const {
        return head == tail;
    }

    inline void push_back(int val) {
        data[tail++] = val;
    }

    inline void pop_back() {
        tail--;
    }

    inline void pop_front() {
        head++;
    }

    inline int front() const {
        return data[head];
    }

    inline int back() const {
        return data[tail - 1];
    }
};

} // namespace

uint64_t sliding_window_max_hash(const std::vector<uint32_t>& values, int window, int iters) {
    if (values.empty() || window <= 0 || static_cast<size_t>(window) > values.size()) {
        return 0;
    }

    const size_t n = values.size();
    const size_t w = static_cast<size_t>(window);
    const uint32_t* val_ptr = values.data();
    
    // Pre-allocate deque buffer to avoid allocation in the loop
    std::vector<int> deque_buffer(n);
    FastDeque dq;
    dq.init(deque_buffer.data());

    uint64_t total_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t hash = 1469598103934665603ULL;
        dq.clear();

        for (size_t i = 0; i < n; ++i) {
            // Remove indices that are out of the current window
            if (!dq.empty() && dq.front() <= static_cast<int>(i - w)) {
                dq.pop_front();
            }

            // Remove elements smaller than the current element from the back
            while (!dq.empty() && val_ptr[dq.back()] <= val_ptr[i]) {
                dq.pop_back();
            }

            dq.push_back(static_cast<int>(i));

            // If we have reached the first full window
            if (i >= w - 1) {
                hash = mix(hash, static_cast<uint64_t>(val_ptr[dq.front()]));
            }
        }
        total_hash = hash;
    }

    return total_hash;
}
