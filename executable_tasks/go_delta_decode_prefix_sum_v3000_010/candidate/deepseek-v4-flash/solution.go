package main

func run(deltas []int32, iters int) uint64 {
	const C int64 = 0x9E3779B1
	var current int64
	var totalOnce uint64
	for _, d := range deltas {
		current += int64(d)
		totalOnce += uint64(current + C)
	}
	return totalOnce * uint64(iters)
}