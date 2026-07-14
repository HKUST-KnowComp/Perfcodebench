package main

func run(deltas []int32, iters int) uint64 {
	const bias = 0x9E3779B1
	total := uint64(0)
	for i := 0; i < iters; i++ {
		current := int64(0)
		for _, d := range deltas {
			current += int64(d)
			// accumulate the biased prefix value without ever writing it back
			total += uint64(current + bias)
		}
	}
	return total
}