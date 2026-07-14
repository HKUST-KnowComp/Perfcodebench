package main

import "math/bits"

func run(left, right []uint64, iters int) uint64 {
    n := len(left)
    var total uint64
    for iter := 0; iter < iters; iter++ {
        var t int
        i := 0
        // Unroll by 8 for better throughput
        for i+7 < n {
            t += bits.OnesCount64(left[i] & right[i])
            t += bits.OnesCount64(left[i+1] & right[i+1])
            t += bits.OnesCount64(left[i+2] & right[i+2])
            t += bits.OnesCount64(left[i+3] & right[i+3])
            t += bits.OnesCount64(left[i+4] & right[i+4])
            t += bits.OnesCount64(left[i+5] & right[i+5])
            t += bits.OnesCount64(left[i+6] & right[i+6])
            t += bits.OnesCount64(left[i+7] & right[i+7])
            i += 8
        }
        for i < n {
            t += bits.OnesCount64(left[i] & right[i])
            i++
        }
        total = uint64(t)
    }
    return total
}
