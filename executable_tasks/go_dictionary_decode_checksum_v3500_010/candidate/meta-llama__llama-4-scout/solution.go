package main

func run(ids, dictVals []uint32, iters int) uint64 {
    var h uint64 = 1469598103934665603
    const prime uint64 = 1099511628211
    for iter := 0; iter < iters; iter++ {
        h = 1469598103934665603
        for i, id := range ids {
            h ^= uint64(dictVals[id]) + 1
            h *= prime
        }
    }
    return h
}