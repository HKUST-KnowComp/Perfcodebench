package main

import "math/bits"

func run(left, right []uint64, iters int) uint64 {
	var total uint64
	// Compute intersection popcount once (inputs are invariant across all iterations)
	for i := range left {
		andWord := left[i] & right[i]
		total += uint64(bits.OnesCount64(andWord))
	}
	return total
}