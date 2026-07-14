package main

import "unsafe"

func run(deltas []int32, iters int) uint64 {
    var total uint64
    for iter := 0; iter < iters; iter++ {
        var current int64
        var prefixSum int64
        for _, d := range deltas {
            current += int64(d)
            prefixSum += current
        }
        total = uint64(int64(total) + prefixSum + int64(len(deltas))*0x9E3779B1)
    }
    return total
}
