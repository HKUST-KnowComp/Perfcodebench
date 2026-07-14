package main

import "math/bits"

func run(left, right []uint64, iters int) uint64 {
	var total uint64
	n := len(left)
	for iter := 0; iter < iters; iter++ {
		var count uint64
		i := 0
		// Unroll by 4 for better throughput
		for ; i+3 < n; i += 4 {
			count += uint64(bits.OnesCount64(left[i] & right[i]))
			count += uint64(bits.OnesCount64(left[i+1] & right[i+1]))
			count += uint64(bits.OnesCount64(left[i+2] & right[i+2]))
			count += uint64(bits.OnesCount64(left[i+3] & right[i+3]))
		}
		// Handle remainder
		for ; i < n; i++ {
			count += uint64(bits.OnesCount64(left[i] & right[i]))
		}
		total = count
	}
	return total
}