package main

func run(deltas []int32, iters int) uint64 {
	var total uint64
	n := len(deltas)
	const magic uint64 = 0x9E3779B1
	for iter := 0; iter < iters; iter++ {
		var current int64
		var sum uint64
		for i := 0; i < n; i++ {
			current += int64(deltas[i])
			sum += uint64(current) + magic
		}
		total = sum
	}
	return total
}
