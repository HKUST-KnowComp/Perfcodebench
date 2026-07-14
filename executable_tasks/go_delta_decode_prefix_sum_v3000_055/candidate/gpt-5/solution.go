package main

func run(deltas []int32, iters int) uint64 {
	const constU uint64 = 0x9E3779B1
	n := len(deltas)
	var total uint64
	for iter := 0; iter < iters; iter++ {
		var current int64
		var sumU uint64
		i := 0
		for ; i+7 < n; i += 8 {
			current += int64(deltas[i+0])
			sumU += uint64(current)
			current += int64(deltas[i+1])
			sumU += uint64(current)
			current += int64(deltas[i+2])
			sumU += uint64(current)
			current += int64(deltas[i+3])
			sumU += uint64(current)
			current += int64(deltas[i+4])
			sumU += uint64(current)
			current += int64(deltas[i+5])
			sumU += uint64(current)
			current += int64(deltas[i+6])
			sumU += uint64(current)
			current += int64(deltas[i+7])
			sumU += uint64(current)
		}
		for ; i < n; i++ {
			current += int64(deltas[i])
			sumU += uint64(current)
		}
		sumU += uint64(n) * constU
		total = sumU
	}
	return total
}
