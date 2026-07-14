package main

import "math/bits"

func run(left, right []uint64, iters int) uint64 {
	// Compute the AND-popcount once; result is identical across iterations.
	n := len(left)
	if rn := len(right); rn < n {
		n = rn
	}

	var total uint64
	// Unrolled loop for better throughput.
	i := 0
	for i+8 <= n {
		w0 := left[i] & right[i]
		w1 := left[i+1] & right[i+1]
		w2 := left[i+2] & right[i+2]
		w3 := left[i+3] & right[i+3]
		w4 := left[i+4] & right[i+4]
		w5 := left[i+5] & right[i+5]
		w6 := left[i+6] & right[i+6]
		w7 := left[i+7] & right[i+7]
		total += uint64(bits.OnesCount64(w0)) +
			uint64(bits.OnesCount64(w1)) +
			uint64(bits.OnesCount64(w2)) +
			uint64(bits.OnesCount64(w3)) +
			uint64(bits.OnesCount64(w4)) +
			uint64(bits.OnesCount64(w5)) +
			uint64(bits.OnesCount64(w6)) +
			uint64(bits.OnesCount64(w7))
		i += 8
	}
	for ; i < n; i++ {
		total += uint64(bits.OnesCount64(left[i] & right[i]))
	}

	return total
}
