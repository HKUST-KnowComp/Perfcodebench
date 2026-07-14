package main

func run(left, right []uint64, iters int) uint64 {
    var total uint64
    for iter := 0; iter < iters; iter++ {
        tmpCount := uint64(0)
        for i := range left {
            word := left[i] & right[i]
            tmpCount += bitCount64(word)
        }
        total += tmpCount
    }
    return total
}

func bitCount64(n uint64) uint64 {
    n = (n & 0x5555555555555555) + ((n >> 1) & 0x5555555555555555)
    n = (n & 0x3333333333333333) + ((n >> 2) & 0x3333333333333333)
    n = (n & 0x0f0f0f0f0f0f0f0f) + ((n >> 4) & 0x0f0f0f0f0f0f0f0f)
    n = (n & 0x00ff00ff00ff00ff) + ((n >> 8) & 0x00ff00ff00ff00ff)
    n = (n & 0x0000ffff0000ffff) + ((n >> 16) & 0x0000ffff0000ffff)
    n = (n & 0x00000000ffffffff) + ((n >> 32) & 0x00000000ffffffff)
    return n
}