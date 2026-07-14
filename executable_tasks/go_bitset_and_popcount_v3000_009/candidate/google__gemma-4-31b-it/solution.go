package main

import "math/bits"

func run(left, right []uint64, iters int) uint64 {
	var total uint64
	for iter := 0; iter < iters; iter++ {
		var currentIterCount uint64
		for i := 0; i < len(left); i++ {
			currentIterCount += uint64(bits.OnesCount64(left[i] & right[i]))
		}
		total = currentIterCount
	}
	return total
}