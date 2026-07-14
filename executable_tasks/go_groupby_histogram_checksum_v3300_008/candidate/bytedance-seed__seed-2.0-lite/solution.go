package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	const fnvOffset = 1469598103934665603
	const fnvPrime = 1099511628211
	var h uint64
	// Preallocate histogram once to avoid per-iteration heap allocation overhead
	hist := make([]uint64, buckets)
	numKeys := len(keys)

	for iter := 0; iter < iters; iter++ {
		// Aggregate values into reused histogram, maintains original correctness
		for i := 0; i < numKeys; i++ {
			hist[keys[i]] += uint64(vals[i])
		}

		// Combine checksum calculation and histogram reset into one pass to eliminate redundant memory scans
		h = fnvOffset
		for i := range hist {
			v := hist[i]
			h ^= v + 1
			h *= fnvPrime
			hist[i] = 0 // Reset bucket for next iteration, avoids separate zeroing pass
		}
	}
	return h
}