package main

func run(deltas []int32, iters int) uint64 {
	n := len(deltas)
	if n == 0 {
		return 0
	}
	var sumPrefixSums int64
	for i, d := range deltas {
		sumPrefixSums += int64(d) * int64(n - i)
	}
	total := uint64(sumPrefixSums + int64(n) * 0x9E3779B1)
	return total * uint64(iters)
}