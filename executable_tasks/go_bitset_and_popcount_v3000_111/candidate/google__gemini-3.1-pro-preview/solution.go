package main

import "math/bits"

func run(left, right []uint64, iters int) uint64 {
	n := len(left)
	if n == 0 {
		return 0
	}
	_ = right[n-1] // bounds check elimination
	var total uint64
	for iter := 0; iter < iters; iter++ {
		var sum uint64
		i := 0
		for ; i <= n-4; i += 4 {
			sum += uint64(bits.OnesCount64(left[i] & right[i]))
			sum += uint64(bits.OnesCount64(left[i+1] & right[i+1]))
			sum += uint64(bits.OnesCount64(left[i+2] & right[i+2]))
			sum += uint64(bits.OnesCount64(left[i+3] & right[i+3]))
		}
		for ; i < n; i++ {
			sum += uint64(bits.OnesCount64(left[i] & right[i]))
		}
		total = sum
	}
	return total
}
