package main

import "math/bits"

func run(left, right []uint64, iters int) uint64 {
	// Assume matching lengths per task contract; use the shorter length defensively.
	n := len(left)
	if len(right) < n {
		n = len(right)
	}

	var total uint64
	for iter := 0; iter < iters; iter++ {
		var sum uint64

		// Unroll to reduce loop overhead and improve ILP.
		i := 0
		for ; i+7 < n; i += 8 {
			sum += uint64(bits.OnesCount64(left[i] & right[i]))
			sum += uint64(bits.OnesCount64(left[i+1] & right[i+1]))
			sum += uint64(bits.OnesCount64(left[i+2] & right[i+2]))
			sum += uint64(bits.OnesCount64(left[i+3] & right[i+3]))
			sum += uint64(bits.OnesCount64(left[i+4] & right[i+4]))
			sum += uint64(bits.OnesCount64(left[i+5] & right[i+5]))
			sum += uint64(bits.OnesCount64(left[i+6] & right[i+6]))
			sum += uint64(bits.OnesCount64(left[i+7] & right[i+7]))
		}
		for ; i < n; i++ {
			sum += uint64(bits.OnesCount64(left[i] & right[i]))
		}

		total = sum
	}
	return total
}
