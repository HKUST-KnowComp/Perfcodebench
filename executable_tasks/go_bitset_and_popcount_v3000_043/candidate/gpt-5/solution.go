package main

import "math/bits"

func run(left, right []uint64, iters int) uint64 {
	// Ensure we only process up to the common length to avoid bounds checks.
	n := len(left)
	if m := len(right); m < n {
		n = m
	}
	left = left[:n]
	right = right[:n]

	var total uint64
	for iter := 0; iter < iters; iter++ {
		var sum uint64
		// Manual unroll for better ILP and fewer loop overheads
		i := 0
		for ; i+4 <= n; i += 4 {
			sum += uint64(bits.OnesCount64(left[i] & right[i]))
			sum += uint64(bits.OnesCount64(left[i+1] & right[i+1]))
			sum += uint64(bits.OnesCount64(left[i+2] & right[i+2]))
			sum += uint64(bits.OnesCount64(left[i+3] & right[i+3]))
		}
		for ; i < n; i++ {
			sum += uint64(bits.OnesCount64(left[i] & right[i]))
		}
		total = sum
	}
	return total
}
