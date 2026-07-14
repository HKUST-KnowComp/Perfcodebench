package main

import (
	"math/bits"
)

// run calculates the population count (number of set bits) of the bitwise AND 
// of two uint64 slices. It repeats the calculation 'iters' times as required 
// by the benchmark harness and returns the result of the final iteration.
func run(left, right []uint64, iters int) uint64 {
	if len(left) == 0 || len(right) == 0 {
		return 0
	}

	var total uint64
	// The harness expects the logic to be executed 'iters' times.
	for iter := 0; iter < iters; iter++ {
		total = 0
		// Use a local variable for length to help the compiler with bounds check elimination.
		n := len(left)
		if len(right) < n {
			n = len(right)
		}

		// Iterate through the slices and compute the popcount of the bitwise AND.
		// math/bits.OnesCount64 is optimized by the Go compiler to use the 
		// hardware POPCNT instruction where available.
		for i := 0; i < n; i++ {
			total += uint64(bits.OnesCount64(left[i] & right[i]))
		}
	}

	return total
}