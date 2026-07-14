package main

func run(deltas []int32, iters int) uint64 {
	var current int64
	var sum int64
	for _, d := range deltas {
		current += int64(d)
		sum += current
	}
	return uint64(sum)
}
