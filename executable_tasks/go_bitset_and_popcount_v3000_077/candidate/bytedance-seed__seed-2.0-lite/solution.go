package main

import "math/bits"

func run(left, right []uint64, iters int) uint64 {
	var total uint64
	for iter := 0; iter < iters; iter++ {
		var iterTotal uint64
		for i := range left {
			andWord := left[i] & right[i]
			iterTotal += bits.OnesCount64(andWord)
		}
		total = iterTotal
	}
	return total
}