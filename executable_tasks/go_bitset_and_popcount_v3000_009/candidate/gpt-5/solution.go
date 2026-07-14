package main

import "math/bits"

func run(left, right []uint64, iters int) uint64 {
	var total uint64
	n := len(left)
	for iter := 0; iter < iters; iter++ {
		var sum uint64
		// Unroll by 8 for better ILP and fewer loop overheads
		i := 0
		limit := n - n%8
		for ; i < limit; i += 8 {
			w0 := left[i+0] & right[i+0]
			w1 := left[i+1] & right[i+1]
			w2 := left[i+2] & right[i+2]
			w3 := left[i+3] & right[i+3]
			w4 := left[i+4] & right[i+4]
			w5 := left[i+5] & right[i+5]
			w6 := left[i+6] & right[i+6]
			w7 := left[i+7] & right[i+7]
			local := bits.OnesCount64(w0) + bits.OnesCount64(w1) + bits.OnesCount64(w2) + bits.OnesCount64(w3) +
				bits.OnesCount64(w4) + bits.OnesCount64(w5) + bits.OnesCount64(w6) + bits.OnesCount64(w7)
			sum += uint64(local)
		}
		for ; i < n; i++ {
			sum += uint64(bits.OnesCount64(left[i] & right[i]))
		}
		total = sum
	}
	return total
}
