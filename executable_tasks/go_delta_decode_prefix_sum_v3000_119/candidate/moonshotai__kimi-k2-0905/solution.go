package main

func run(deltas []int32, iters int) uint64 {
	const magic = 0x9E3779B1
	total := uint64(0)
	for iter := 0; iter < iters; iter++ {
		var current int64
		sum := uint64(0)
		for _, d := range deltas {
			current += int64(d)
			sum += uint64(current) + magic
		}
		total = sum
	}
	return total
}