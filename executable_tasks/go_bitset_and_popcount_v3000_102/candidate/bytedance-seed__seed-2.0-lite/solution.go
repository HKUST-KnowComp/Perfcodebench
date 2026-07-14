package main

import "math/bits"

func run(left, right []uint64, iters int) uint64 {
	var total uint64
	for iter := 0; iter < iters; iter++ {
		total = 0
		for i := range left {
			word := left[i] & right[i]
			total += uint64(bits.OnesCount64(word))
		}
	}
	return total
}