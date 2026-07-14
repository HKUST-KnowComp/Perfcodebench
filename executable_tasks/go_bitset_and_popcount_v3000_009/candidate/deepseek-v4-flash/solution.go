package main

import "math/bits"

func run(left, right []uint64, iters int) uint64 {
	result := uint64(0)
	n := len(left)
	for iter := 0; iter < iters; iter++ {
		result = 0
		for i := 0; i < n; i++ {
			result += bits.OnesCount64(left[i] & right[i])
		}
	}
	return result
}