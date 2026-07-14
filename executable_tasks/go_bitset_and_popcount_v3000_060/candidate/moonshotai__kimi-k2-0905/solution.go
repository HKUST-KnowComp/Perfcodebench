package main

import "math/bits"

func run(left, right []uint64, iters int) uint64 {
	// Warm-up / dummy to keep the same iteration count semantics.
	var total uint64
	for iter := 0; iter < iters; iter++ {
		total = 0
		// Process 4 words per iteration to unroll and hide latency.
		for i := 0; i+3 < len(left); i += 4 {
			total += uint64(bits.OnesCount64(left[i] & right[i]))
			total += uint64(bits.OnesCount64(left[i+1] & right[i+1]))
			total += uint64(bits.OnesCount64(left[i+2] & right[i+2]))
			total += uint64(bits.OnesCount64(left[i+3] & right[i+3]))
		}
		// Tail.
		for i := (len(left) / 4) * 4; i < len(left); i++ {
			total += uint64(bits.OnesCount64(left[i] & right[i]))
		}
	}
	return total
}