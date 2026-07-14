package main

import "math/bits"

func run(left, right []uint64, iters int) uint64 {
	var total uint64
	n := len(left)
	for iter := 0; iter < iters; iter++ {
		var count uint64
		for i := 0; i < n; i++ {
			count += uint64(bits.OnesCount64(left[i] & right[i]))
		}
		total = count
	}
	return total
}