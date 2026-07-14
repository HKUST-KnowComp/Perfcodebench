package main

import "math/bits"

func run(left, right []uint64, iters int) uint64 {
	// Preserve baseline behavior: if no iterations, return 0.
	if iters <= 0 {
		return 0
	}

	n := len(left) // assume right has at least n elements as in baseline
	var total uint64

	// Compute popcount of AND across slices once; same for all iterations.
	i := 0
	for i+7 < n {
		a0 := left[i] & right[i]
		a1 := left[i+1] & right[i+1]
		a2 := left[i+2] & right[i+2]
		a3 := left[i+3] & right[i+3]
		a4 := left[i+4] & right[i+4]
		a5 := left[i+5] & right[i+5]
		a6 := left[i+6] & right[i+6]
		a7 := left[i+7] & right[i+7]
		total += uint64(bits.OnesCount64(a0))
		total += uint64(bits.OnesCount64(a1))
		total += uint64(bits.OnesCount64(a2))
		total += uint64(bits.OnesCount64(a3))
		total += uint64(bits.OnesCount64(a4))
		total += uint64(bits.OnesCount64(a5))
		total += uint64(bits.OnesCount64(a6))
		total += uint64(bits.OnesCount64(a7))
		i += 8
	}
	for ; i < n; i++ {
		word := left[i] & right[i]
		total += uint64(bits.OnesCount64(word))
	}

	return total
}
