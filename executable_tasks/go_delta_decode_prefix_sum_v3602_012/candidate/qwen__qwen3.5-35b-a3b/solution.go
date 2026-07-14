package main

func run(deltas []int32, iters int) uint64 {
	var current int64
	var totalPerIter uint64
	for _, d := range deltas {
		current += int64(d)
		totalPerIter += uint64(current + 0x9E3779B1)
	}
	return totalPerIter * uint64(iters)
}