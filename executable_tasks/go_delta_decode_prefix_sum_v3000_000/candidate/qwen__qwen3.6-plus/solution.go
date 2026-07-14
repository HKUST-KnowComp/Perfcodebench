package main

func run(deltas []int32, iters int) uint64 {
	var current, sumP int64
	for _, d := range deltas {
		current += int64(d)
		sumP += current
	}
	return uint64(sumP) + uint64(len(deltas))*0x9E3779B1
}