package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	var h uint64
	hist := make([]uint64, buckets)
	
	for iter := 0; iter < iters; iter++ {
		// Reset histogram for this iteration
		for i := range hist {
			hist[i] = 0
		}
		
		// Aggregate values into buckets
		for i := 0; i < len(keys); i++ {
			hist[keys[i]] += uint64(vals[i])
		}
		
		// Calculate checksum
		h = 1469598103934665603
		for _, v := range hist {
			h ^= v + 1
			h *= 1099511628211
		}
	}
	return h
}