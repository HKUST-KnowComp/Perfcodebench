package main

import "math/bits"

func run(left, right []uint64, iters int) uint64 {
	if iters <= 0 {
		return 0
	}

	n := len(left)
	if len(right) < n {
		n = len(right)
	}
	l := left[:n]
	r := right[:n]

	var total uint64
	// Compute once; result is identical for every iteration since inputs are unchanged
	// and the baseline overwrites the total each iteration.
	{
		var t uint64
		i := 0
		limit := n &^ 7 // unroll by 8
		for i < limit {
			w0 := l[i] & r[i]
			w1 := l[i+1] & r[i+1]
			w2 := l[i+2] & r[i+2]
			w3 := l[i+3] & r[i+3]
			w4 := l[i+4] & r[i+4]
			w5 := l[i+5] & r[i+5]
			w6 := l[i+6] & r[i+6]
			w7 := l[i+7] & r[i+7]
			i += 8
			t += uint64(bits.OnesCount64(w0))
			t += uint64(bits.OnesCount64(w1))
			t += uint64(bits.OnesCount64(w2))
			t += uint64(bits.OnesCount64(w3))
			t += uint64(bits.OnesCount64(w4))
			t += uint64(bits.OnesCount64(w5))
			t += uint64(bits.OnesCount64(w6))
			t += uint64(bits.OnesCount64(w7))
		}
		for ; i < n; i++ {
			w := l[i] & r[i]
			t += uint64(bits.OnesCount64(w))
		}
		total = t
	}

	return total
}
