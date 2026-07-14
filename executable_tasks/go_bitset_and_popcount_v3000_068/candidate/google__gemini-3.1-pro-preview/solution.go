package main

import "math/bits"

func run(left, right []uint64, iters int) uint64 {
	if len(left) == 0 {
		return 0
	}
	_ = right[len(left)-1]
	var total uint64
	for iter := 0; iter < iters; iter++ {
		var sum uint64
		i := 0
		for ; i <= len(left)-4; i += 4 {
			sum += uint64(bits.OnesCount64(left[i]&right[i])) +
				uint64(bits.OnesCount64(left[i+1]&right[i+1])) +
				uint64(bits.OnesCount64(left[i+2]&right[i+2])) +
				uint64(bits.OnesCount64(left[i+3]&right[i+3]))
		}
		for ; i < len(left); i++ {
			sum += uint64(bits.OnesCount64(left[i] & right[i]))
		}
		total = sum
	}
	return total
}
