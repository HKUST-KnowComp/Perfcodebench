package main

import "math/bits"

func run(left, right []uint64, iters int) uint64 {
	// Assume matching lengths per task contract; iterate over the common packed words.
	n := len(left)
	var total uint64
	for iter := 0; iter < iters; iter++ {
		var sum uint64
		for i := 0; i < n; i++ {
			sum += uint64(bits.OnesCount64(left[i] & right[i]))
		}
		total = sum
	}
	return total
}
