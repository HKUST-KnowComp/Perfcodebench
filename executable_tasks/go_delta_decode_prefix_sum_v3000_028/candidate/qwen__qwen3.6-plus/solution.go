package main

func run(deltas []int32, iters int) uint64 {
	var total uint64
	const magic int64 = 0x9E3779B1
	for iter := 0; iter < iters; iter++ {
		var current int64
		var sum int64
		for _, d := range deltas {
			current += int64(d)
			sum += current + magic
		}
		total = uint64(sum)
	}
	return total
}