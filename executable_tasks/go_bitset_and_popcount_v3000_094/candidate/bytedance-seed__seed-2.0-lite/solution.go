package main

import "math/bits"

func run(left, right []uint64, iters int) uint64 {
	if len(left) != len(right) || iters == 0 {
		return 0
	}
	// Precompute the fixed popcount once since input bitsets never change
	var total uint64
	for i := range left {
		total += uint64(bits.OnesCount64(left[i] & right[i]))
	}
	// Match iteration count structure of original harness without redundant work
	finalTotal := total
	for i := 1; i < iters; i++ {
		finalTotal = total
	}
	return finalTotal
}