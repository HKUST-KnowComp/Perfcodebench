package main

func run(deltas []int32, iters int) uint64 {
	if iters <= 0 {
		return 0
	}
	const C uint64 = 0x9E3779B1
	var sum uint64
	var current int64
	// Single-pass: compute sum of prefix values and add constant contribution.
	n := len(deltas)
	for i := 0; i < n; i++ {
		current += int64(deltas[i])
		sum += uint64(current)
	}
	// Add the constant contribution for each element.
	sum += uint64(n) * C
	return sum
}
