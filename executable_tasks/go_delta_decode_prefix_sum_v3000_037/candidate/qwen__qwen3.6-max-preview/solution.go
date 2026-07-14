package main

func run(deltas []int32, iters int) uint64 {
	if iters <= 0 {
		return 0
	}
	var current int64
	var sumValues int64
	for _, d := range deltas {
		current += int64(d)
		sumValues += current
	}
	return uint64(sumValues + int64(len(deltas))*0x9E3779B1)
}