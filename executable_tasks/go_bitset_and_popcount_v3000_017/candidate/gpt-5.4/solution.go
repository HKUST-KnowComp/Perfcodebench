package main

import "math/bits"

func run(left, right []uint64, iters int) uint64 {
	// Assume matching lengths as required by the task/harness.
	// Compute the exact popcount of the bitwise AND directly per word,
	// avoiding all intermediate allocations and per-bit expansion.
	var total uint64
	for iter := 0; iter < iters; iter++ {
		var sum uint64
		for i, lv := range left {
			sum += uint64(bits.OnesCount64(lv & right[i]))
		}
		total = sum
	}
	return total
}
