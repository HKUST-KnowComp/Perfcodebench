package main

import "math/bits"

func run(left, right []uint64, iters int) uint64 {
    // Compute popcount of (left & right) once; result is identical each iteration
    n := len(left)
    if len(right) < n {
        n = len(right)
    }
    if n == 0 {
        return 0
    }

    l := left[:n]
    r := right[:n]

    var sum uint64
    i := 0

    // Unroll by 8 for better ILP and fewer loop overheads
    for i+7 < n {
        w0 := l[i] & r[i]
        w1 := l[i+1] & r[i+1]
        w2 := l[i+2] & r[i+2]
        w3 := l[i+3] & r[i+3]
        w4 := l[i+4] & r[i+4]
        w5 := l[i+5] & r[i+5]
        w6 := l[i+6] & r[i+6]
        w7 := l[i+7] & r[i+7]
        cnt := bits.OnesCount64(w0) + bits.OnesCount64(w1) + bits.OnesCount64(w2) + bits.OnesCount64(w3) +
            bits.OnesCount64(w4) + bits.OnesCount64(w5) + bits.OnesCount64(w6) + bits.OnesCount64(w7)
        sum += uint64(cnt)
        i += 8
    }

    for i < n {
        sum += uint64(bits.OnesCount64(l[i] & r[i]))
        i++
    }

    return sum
}
