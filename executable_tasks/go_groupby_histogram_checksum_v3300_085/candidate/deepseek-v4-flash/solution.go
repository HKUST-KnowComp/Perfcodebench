package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	// Pre-allocate histogram once (zeroed by make)
	hist := make([]uint64, buckets)
	const prime = 1099511628211
	const base = 1469598103934665603

	var h uint64
	for iter := 0; iter < iters; iter++ {
		// Reset histogram using a loop (avoids allocation each iteration)
		for i := 0; i < int(buckets); i++ {
			hist[i] = 0
		}

		// Accumulate values into histogram
		for i, k := range keys {
			hist[k] += uint64(vals[i])
		}

		// Compute checksum over histogram (same as baseline)
		h = base
		for _, v := range hist {
			h ^= v + 1
			h *= prime
		}
	}
	return h
}