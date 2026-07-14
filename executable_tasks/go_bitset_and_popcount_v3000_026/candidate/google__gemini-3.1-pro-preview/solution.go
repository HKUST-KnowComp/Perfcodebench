package main

import (
	"math/bits"
)

// run calculates the population count (number of set bits) of the bitwise AND 
// of two uint64 slices. It repeats the calculation 'iters' times and returns 
// the result of the final iteration.
func run(left, right []uint64, iters int) uint64 {
	if len(left) == 0 {
		return 0
	}

	var total uint64
	// The baseline implementation resets total inside the loop and returns the last result.
	// Since left and right are constant, we compute the popcount efficiently.
	for iter := 0; iter < iters; iter++ {
		currentTotal := uint64(0)
		// Unrolling or simple range loop; math/bits.OnesCount64 is intrinsic-optimized.
		for i := 0; i < len(left); i++ {
			currentTotal += uint64(bits.OnesCount64(left[i] & right[i]))
		}
		total = currentTotal
	}

	return total
}