package main

func run(deltas []int32, iters int) uint64 {
	n := len(deltas)
	var total uint64
	n64 := uint64(n)
	c := uint64(0x9E3779B1)
	for iter := 0; iter < iters; iter++ {
		var sumP uint64
		for i, d := range deltas {
			sumP += uint64(d) * (n64 - uint64(i))
		}
		total = sumP + n64*c
	}
	return total
}