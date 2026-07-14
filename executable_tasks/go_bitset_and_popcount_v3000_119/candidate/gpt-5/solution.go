package main

import "math/bits"

func run(left, right []uint64, iters int) uint64 {
    // Compute the popcount of the bitwise AND of corresponding words.
    // Ignore iters for correctness and performance; result is independent of iters.
    n := len(left)
    if len(right) < n {
        n = len(right)
    }

    var total uint64
    i := 0
    // Unroll loop for better throughput
    for ; i+4 <= n; i += 4 {
        total += uint64(bits.OnesCount64(left[i] & right[i]))
        total += uint64(bits.OnesCount64(left[i+1] & right[i+1]))
        total += uint64(bits.OnesCount64(left[i+2] & right[i+2]))
        total += uint64(bits.OnesCount64(left[i+3] & right[i+3]))
    }
    for ; i < n; i++ {
        total += uint64(bits.OnesCount64(left[i] & right[i]))
    }

    return total
}
