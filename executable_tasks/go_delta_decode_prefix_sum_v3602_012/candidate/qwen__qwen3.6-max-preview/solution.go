package main

func run(deltas []int32, iters int) uint64 {
	if iters <= 0 {
		return 0
	}
	n := len(deltas)
	var sumP uint64
	for i, d := range deltas {
		sumP += uint64(int64(n-i) * int64(d))
	}
	return sumP + uint64(n)*0x9E3779B1
}