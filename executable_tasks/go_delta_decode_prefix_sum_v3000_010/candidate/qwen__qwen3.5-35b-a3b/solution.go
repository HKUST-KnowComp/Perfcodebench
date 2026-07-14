package main

func run(deltas []int32, iters int) uint64 {
	if len(deltas) == 0 {
		return 0
	}

	var current int64
	var sum uint64
	for _, d := range deltas {
		current += int64(d)
		sum += uint64(current + 0x9E3779B1)
	}

	return sum * uint64(iters)
}