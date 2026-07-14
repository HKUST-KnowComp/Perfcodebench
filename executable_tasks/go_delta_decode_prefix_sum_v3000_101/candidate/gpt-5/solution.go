package main

func run(deltas []int32, iters int) uint64 {
    n := len(deltas)
    if n == 0 {
        return 0
    }

    const c int64 = 0x9E3779B1
    // Precompute unroll limit once since n is constant across iterations.
    limit := n - (n % 8)

    var total uint64
    for iter := 0; iter < iters; iter++ {
        var current int64
        var t uint64
        i := 0
        for ; i < limit; i += 8 {
            current += int64(deltas[i+0])
            t += uint64(current + c)

            current += int64(deltas[i+1])
            t += uint64(current + c)

            current += int64(deltas[i+2])
            t += uint64(current + c)

            current += int64(deltas[i+3])
            t += uint64(current + c)

            current += int64(deltas[i+4])
            t += uint64(current + c)

            current += int64(deltas[i+5])
            t += uint64(current + c)

            current += int64(deltas[i+6])
            t += uint64(current + c)

            current += int64(deltas[i+7])
            t += uint64(current + c)
        }
        for ; i < n; i++ {
            current += int64(deltas[i])
            t += uint64(current + c)
        }
        // As in the baseline, only the final iteration's total is returned.
        total = t
    }
    return total
}
