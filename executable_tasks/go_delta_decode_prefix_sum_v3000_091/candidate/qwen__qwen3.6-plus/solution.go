package main

func run(deltas []int32, iters int) uint64 {
	var current, sumValues int64
	for _, d := range deltas {
		current += int64(d)
		sumValues += current
	}
	return uint64(sumValues) + uint64(len(deltas))*0x9E3779B1
}