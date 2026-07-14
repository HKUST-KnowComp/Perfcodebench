package main

import "math/bits"

func run(left, right []uint64, iters int) uint64 {
	if len(left) == 0 || iters == 0 {
		return 0
	}
	var total uint64
	n := len(left)
	for iter := 0; iter < iters; iter++ {
		var sum uint64
		_ = right[n-1] // bounds check elimination
		i := 0
		for ; i <= n-4; i += 4 {
			s0 := uint64(bits.OnesCount64(left[i] & right[i]))
			s1 := uint64(bits.OnesCount64(left[i+1] & right[i+1]))
			s2 := uint64(bits.OnesCount64(left[i+2] & right[i+2]))
			s3 := uint64(bits.OnesCount64(left[i+3] & right[i+3]))
			sum += s0 + s1 + s2 + s3
		}
		for ; i < n; i++ {
			sum += uint64(bits.OnesCount64(left[i] & right[i]))
		}
		total = sum
	}
	return total
}
