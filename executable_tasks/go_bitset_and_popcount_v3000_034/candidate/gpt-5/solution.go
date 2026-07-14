package main

import "math/bits"

func run(left, right []uint64, iters int) uint64 {
	// Compute the popcount of (left & right) once; result independent of iters
	n := len(left)
	if n == 0 {
		return 0
	}
	// Encourage bounds-check elimination for both slices
	_ = left[n-1]
	_ = right[n-1]

	var sum uint64
	i := 0
	for ; i+7 < n; i += 8 {
		a0 := left[i+0] & right[i+0]
		a1 := left[i+1] & right[i+1]
		a2 := left[i+2] & right[i+2]
		a3 := left[i+3] & right[i+3]
		a4 := left[i+4] & right[i+4]
		a5 := left[i+5] & right[i+5]
		a6 := left[i+6] & right[i+6]
		a7 := left[i+7] & right[i+7]
		sum += uint64(bits.OnesCount64(a0))
		sum += uint64(bits.OnesCount64(a1))
		sum += uint64(bits.OnesCount64(a2))
		sum += uint64(bits.OnesCount64(a3))
		sum += uint64(bits.OnesCount64(a4))
		sum += uint64(bits.OnesCount64(a5))
		sum += uint64(bits.OnesCount64(a6))
		sum += uint64(bits.OnesCount64(a7))
	}
	for ; i < n; i++ {
		sum += uint64(bits.OnesCount64(left[i] & right[i]))
	}
	return sum
}
