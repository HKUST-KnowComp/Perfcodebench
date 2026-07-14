package main

func run(deltas []int32, iters int) uint64 {
	var current int64
	var sumPrefix int64
	for _, d := range deltas {
		current += int64(d)
		sumPrefix += current
	}
	return uint64(sumPrefix + int64(len(deltas))*0x9E3779B1)
}