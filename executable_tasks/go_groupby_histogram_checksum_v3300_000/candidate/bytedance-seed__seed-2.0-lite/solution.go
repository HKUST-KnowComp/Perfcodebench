package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	var h uint64
	// Allocate histogram once to reuse across all iterations, eliminating repeated heap allocations
	hist := make([]uint64, buckets)
	for iter := 0; iter < iters; iter++ {
		// Efficiently zero histogram with built-in clear (optimized memzero operation)
		clear(hist)
		// Preserve original aggregation logic to maintain correctness
		for i := range keys {
			hist[keys[i]] += uint64(vals[i])
		}
		// Exact original checksum calculation to guarantee matching output
		h = 1469598103934665603
		for _, v := range hist {
			h ^= v + 1
			h *= 1099511628211
		}
	}
	return h
}