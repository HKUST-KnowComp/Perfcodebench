package main

func run(deltas []int32, iters int) uint64 {
    var total uint64
    for iter := 0; iter < iters; iter++ {
        var current int64
        total = 0
        for _, d := range deltas {
            current += int64(d)
            total += uint64(current + 0x9E3779B1)
        }
    }
    return total
}
