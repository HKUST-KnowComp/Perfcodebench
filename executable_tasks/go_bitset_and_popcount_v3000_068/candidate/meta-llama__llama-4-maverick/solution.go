package main

func run(left, right []uint64, iters int) uint64 {
	var total uint64
	for iter := 0; iter < iters; iter++ {
		total = 0
		for i := range left {
			word := left[i] & right[i]
			total += popcount(word)
		}
	}
	return total
}

func popcount(x uint64) uint64 {
	const m1 = 0x5555555555555555
	const m2 = 0x3333333333333333
	const m4 = 0x0f0f0f0f0f0f0f0f
	x -= (x >> 1) & m1
	x = (x & m2) + ((x >> 2) & m2)
	x = (x + (x >> 4)) & m4
	x += x >> 8
	x += x >> 16
	x += x >> 32
	return x & 0x7f
}
