package main

import "math/bits"

func run(left, right []uint64, iters int) uint64 {
	// iters does not affect the result; we compute the popcount once
	var total uint64
	for i := range left {
		total += uint64(bits.OnesCount64(left[i] & right[i]))
	}
	return total
}