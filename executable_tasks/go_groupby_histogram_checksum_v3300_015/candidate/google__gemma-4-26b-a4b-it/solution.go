package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	h := uint64(1469598103934665603)
	hist := make([]uint64, buckets)

	for iter := 0; iter < iters; iter++ {
		// Reset histogram without reallocating
		for i := range hist {
			hist[i] = 0
		}

		// Aggregate values
		for i := 0; i < len(keys); i++ {
			hist[keys[i]] += uint64(vals[i])
		}

		// Calculate FNV-1a style checksum
		h = 1469598103934665603
		for i := 0; i < len(hist); i++ {
			v := hist[i]
			h ^= v + 1
			h *= 1099511628211
		}
	}
	return h
}