package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	const prime = 1099511628211
	const offset = 1469598103934665603

	n := uint64(buckets)
	hist := make([]uint64, n)

	var h uint64
	for iter := 0; iter < iters; iter++ {
		// Reset histogram to zero
		for i := uint64(0); i < n; i++ {
			hist[i] = 0
		}

		// Accumulate values into histogram
		for i, k := range keys {
			hist[k] += uint64(vals[i])
		}

		// Compute checksum over histogram buckets
		h = offset
		for _, v := range hist {
			h ^= v + 1
			h *= prime
		}
	}
	return h
}