package main

func run(deltas []int32, iters int) uint64 {
	var total uint64
	const key = 0x9E3779B1
	
	for iter := 0; iter < iters; iter++ {
		sum := uint64(0)
		var current int64
		for _, d := range deltas {
			current += int64(d)
			sum += uint64(current) + key  // decode and accumulate
		}
		total = sum  // only last iteration is returned
	}
	return total
}