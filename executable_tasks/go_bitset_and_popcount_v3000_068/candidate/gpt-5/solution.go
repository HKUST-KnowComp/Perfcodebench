package main

import "math/bits"

func run(left, right []uint64, iters int) uint64 {
	l := left
	r := right
	n := len(l)
	var total uint64
	for iter := 0; iter < iters; iter++ {
		var t uint64
		i := 0
		for i+3 < n {
			w0 := l[i] & r[i]
			w1 := l[i+1] & r[i+1]
			w2 := l[i+2] & r[i+2]
			w3 := l[i+3] & r[i+3]
			t += uint64(bits.OnesCount64(w0))
			t += uint64(bits.OnesCount64(w1))
			t += uint64(bits.OnesCount64(w2))
			t += uint64(bits.OnesCount64(w3))
			i += 4
		}
		for i < n {
			t += uint64(bits.OnesCount64(l[i] & r[i]))
			i++
		}
		total = t
	}
	return total
}
