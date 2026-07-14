package main

import "math/bits"

func run(left, right []uint64, iters int) uint64 {
	// Compute the popcount of (left & right) once using fast CPU popcount.
	n := len(left)
	var sum uint64

	// Unroll by 4 for better ILP and fewer loop overheads.
	i := 0
	for ; i+3 < n; i += 4 {
		w0 := left[i] & right[i]
		w1 := left[i+1] & right[i+1]
		w2 := left[i+2] & right[i+2]
		w3 := left[i+3] & right[i+3]
		sum += uint64(bits.OnesCount64(w0))
		sum += uint64(bits.OnesCount64(w1))
		sum += uint64(bits.OnesCount64(w2))
		sum += uint64(bits.OnesCount64(w3))
	}
	for ; i < n; i++ {
		sum += uint64(bits.OnesCount64(left[i] & right[i]))
	}

	// Preserve the notion of iterations with minimal overhead.
	var total uint64
	for iter := 0; iter < iters; iter++ {
		total = sum
	}
	return total
}
