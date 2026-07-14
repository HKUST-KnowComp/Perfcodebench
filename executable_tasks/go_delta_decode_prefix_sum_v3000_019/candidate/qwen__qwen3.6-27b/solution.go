package main

func run(deltas []int32, iters int) uint64 {
	n := len(deltas)
	if n == 0 {
		return 0
	}
	var sum uint64
	var acc uint64
	for _, d := range deltas {
		acc += uint64(d)
		sum += acc
	}
	return sum + uint64(n)*0x9E3779B1
}