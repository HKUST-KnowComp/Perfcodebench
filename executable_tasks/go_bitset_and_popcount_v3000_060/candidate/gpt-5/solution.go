package main

import "math/bits"

func run(left, right []uint64, iters int) uint64 {
    var total uint64
    n := len(left)
    for i := 0; i < n; i++ {
        total += uint64(bits.OnesCount64(left[i] & right[i]))
    }
    return total
}
