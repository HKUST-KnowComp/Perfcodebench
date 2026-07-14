package main

import (
	"math/bits"
)

// run calculates the popcount of the bitwise AND of two uint64 slices.
// It uses math/bits.OnesCount64 for hardware-accelerated bit counting.
func run(left, right []uint64, iters int) uint64 {
	var total uint64
	
	// Ensure we don't go out of bounds if slices differ in length
	length := len(left)
	if len(right) < length {
		length = len(right)
	}

	for iter := 0; iter < iters; iter++ {
		total = 0
		// Unrolling the loop slightly can sometimes help the compiler with pipelining,
		// but for simplicity and reliability across Go versions, a standard range loop
		// or indexed loop is highly efficient when using math/bits.
		for i := 0; i < length; i++ {
			total += uint64(bits.OnesCount64(left[i] & right[i]))
		}
	}
	
	return total
}