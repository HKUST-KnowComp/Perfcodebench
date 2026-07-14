package main

func run(deltas []int32, iters int) uint64 {
	// Preserve baseline behavior for iters == 0
	if iters <= 0 {
		return 0
	}

	var total uint64
	var current int64
	const c int64 = 0x9E3779B1

	n := len(deltas)
	i := 0
	// Unroll by 4 for better throughput
	for ; i+3 < n; i += 4 {
		current += int64(deltas[i+0])
		total += uint64(current + c)

		current += int64(deltas[i+1])
		total += uint64(current + c)

		current += int64(deltas[i+2])
		total += uint64(current + c)

		current += int64(deltas[i+3])
		total += uint64(current + c)
	}
	for ; i < n; i++ {
		current += int64(deltas[i])
		total += uint64(current + c)
	}

	// Baseline resets and recomputes per iteration but returns the last iteration's total.
	// Computing once yields the same result for any iters >= 1.
	return total
}
