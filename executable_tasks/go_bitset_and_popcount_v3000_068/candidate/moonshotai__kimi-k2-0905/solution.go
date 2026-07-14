package main

func run(left, right []uint64, iters int) uint64 {
	// SWAR popcount for a single uint64.
	pop64 := func(x uint64) uint64 {
		x -= (x >> 1) & 0x5555555555555555
		x = (x & 0x3333333333333333) + ((x >> 2) & 0x3333333333333333)
		x = (x + (x >> 4)) & 0x0f0f0f0f0f0f0f0f
		return (x * 0x0101010101010101) >> 56
	}

	var total uint64
	for iter := 0; iter < iters; iter++ {
		total = 0
		for i := range left {
			total += pop64(left[i] & right[i])
		}
	}
	return total
}