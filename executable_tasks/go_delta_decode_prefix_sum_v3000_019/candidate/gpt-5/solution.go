package main

func run(deltas []int32, iters int) uint64 {
	var total uint64
	n := len(deltas)
	addTotal := uint64(0x9E3779B1) * uint64(n)
	for iter := 0; iter < iters; iter++ {
		var current int64
		var sum uint64
		i := 0
		for n-i >= 8 {
			current += int64(deltas[i+0]); sum += uint64(current)
			current += int64(deltas[i+1]); sum += uint64(current)
			current += int64(deltas[i+2]); sum += uint64(current)
			current += int64(deltas[i+3]); sum += uint64(current)
			current += int64(deltas[i+4]); sum += uint64(current)
			current += int64(deltas[i+5]); sum += uint64(current)
			current += int64(deltas[i+6]); sum += uint64(current)
			current += int64(deltas[i+7]); sum += uint64(current)
			i += 8
		}
		for ; i < n; i++ {
			current += int64(deltas[i])
			sum += uint64(current)
		}
		sum += addTotal
		total = sum
	}
	return total
}
