package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	hist := make([]uint64, buckets)
	var h uint64
	
	for iter := 0; iter < iters; iter++ {
		// Reset histogram without reallocating
		for i := range hist {
			hist[i] = 0
		}

		// Aggregate values
		for i := 0; i < len(keys); i++ {
			hist[keys[i]] += uint64(vals[i])
		}

		// FNV-1a checksum
		h = 1469598103934665603
		for i := 0; i < int(buckets); i++ {
			v := hist[i]
			h ^= v + 1
			h *= 1099511628211
		}
	}
	return h
}