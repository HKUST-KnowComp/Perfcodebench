package main

func run(deltas []int32, iters int) uint64 {
	n := len(deltas)
	if n == 0 {
		return 0
	}

	var total uint64
	const offset = int64(0x9E3779B1)

	for iter := 0; iter < iters; iter++ {
		var current int64
		var sum uint64

		// Process 4 elements at a time
		i := 0
		limit := n - 3
		for i < limit {
			current += int64(deltas[i])
			s0 := current
			current += int64(deltas[i+1])
			s1 := current
			current += int64(deltas[i+2])
			s2 := current
			current += int64(deltas[i+3])
			s3 := current
			sum += uint64(s0+offset) + uint64(s1+offset) + uint64(s2+offset) + uint64(s3+offset)
			i += 4
		}

		// Handle remaining elements
		for ; i < n; i++ {
			current += int64(deltas[i])
			sum += uint64(current + offset)
		}

		total = sum
	}
	return total
}