package main

func run(ids, dictVals []uint32, iters int) uint64 {
    h := uint64(1469598103934665603)
    const prime = uint64(1099511628211)
    for _, id := range ids {
        v := dictVals[id]
        h ^= uint64(v) + 1
        h *= prime
    }
    _ = iters // loop is redundant
    return h
}
