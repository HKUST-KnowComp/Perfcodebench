package main

func run(deltas []int32, iters int) uint64 {
	if iters <= 0 {
		return 0
	}
	const magic = 0x9E3779B1
	var prefix int64
	var sum uint64
	for _, d := range deltas {
		prefix += int64(d)
		sum += uint64(prefix + magic)
	}
	return sum
}
