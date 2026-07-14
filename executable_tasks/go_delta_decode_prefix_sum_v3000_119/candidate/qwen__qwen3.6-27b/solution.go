package main

func run(deltas []int32, iters int) uint64 {
	n := int64(len(deltas))
	const C = 0x9E3779B1
	base := uint64(n * C)
	var total uint64
	for iter := 0; iter < iters; iter++ {
		var sum int64
		mult := n
		for _, d := range deltas {
			sum += mult * int64(d)
			mult--
		}
		total = uint64(sum) + base
	}
	return total
}