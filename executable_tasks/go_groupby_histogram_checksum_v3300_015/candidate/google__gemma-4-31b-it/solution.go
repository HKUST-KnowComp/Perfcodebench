package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	var h uint64
	// Pre-allocate the histogram slice once to avoid repeated allocations in the loop
	hist := make([]uint64, buckets)
	
	for iter := 0; iter < iters; iter++ {
		// Reset the histogram for the current iteration
		for i := range hist {
			hist[i] = 0
		}
		
		// Aggregate values into buckets
		for i := 0; i < len(keys); i++ {
			hist[keys[i]] += uint64(vals[i])
		}
		
		// Calculate FNV-1a-like checksum
		h = 1469598103934665603
		for i := 0; i < len(hist); i++ {
			h ^= hist[i] + 1
			h *= 1099511628211
		}
	}
	return h
}