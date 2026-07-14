package main

func run(deltas []int32, iters int) uint64 {
	// Nothing to do for empty input.
	if len(deltas) == 0 {
		return 0
	}

	const magic = 0x9E3779B1

	// We keep the result outside the per-iteration loop so we can
	// accumulate into it without re-initialising to zero on every iteration.
	// After the final iteration its value is exactly what the caller expects.
	total := uint64(0)

	for iter := 0; iter < iters; iter++ {
		current := int64(0)
		acc := uint64(0)

		i := 0
		// Four-way unrolled inner loop: reduces loop-control overhead and
		// lets the CPU issue multiple independent adds per cycle.
		for ; i+4 <= len(deltas); i += 4 {
			current += int64(deltas[i])
			acc += uint64(current) + magic

			current += int64(deltas[i+1])
			acc += uint64(current) + magic

			current += int64(deltas[i+2])
			acc += uint64(current) + magic

			current += int64(deltas[i+3])
			acc += uint64(current) + magic
		}

		// Tail: process any remaining elements.
		for ; i < len(deltas); i++ {
			current += int64(deltas[i])
			acc += uint64(current) + magic
		}

		total = acc
	}

	return total
}